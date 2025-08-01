#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// WiFi Credentials
const char* ssid = "Pasandi";
const char* password = "pasi@123";

// Define Pins
#define DHTPIN 4             // DHT11 sensor pin
#define DHTTYPE DHT11       
#define TRIG_PIN 12          // Ultrasonic Trig pin
#define ECHO_PIN 13          // Ultrasonic Echo pin
#define FAN_PIN 5            // Fan control pin
#define PUMP_PIN 22  //19        // Water pump control pin
#define LDR_PIN 34
#define LL_PIN 33 
#define SPR_PIN 19
#define BUZ_PIN 23

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// Variables
float temperature = 0;
float waterLevel = 0;
float light = 0;
bool fanStatus = false;
bool sprinklerStatus = false;
bool lightStatus = false;
bool pumpStatus = false;
float humidity = 0;
float lightSensitivity = 0;


// Function to read water level using ultrasonic sensor
float getWaterLevel() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    float duration = pulseIn(ECHO_PIN, HIGH);
    float distance = (duration * 0.0343) / 2;  // Convert to cm
    return distance;  
}

// Read sensors and control devices
void updateSensors() {
    // Read and update sensor values
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    waterLevel = getWaterLevel();
    lightSensitivity = analogRead(LDR_PIN);

    // Print sensor values
    Serial.println("\n----- Greenhouse Status -----");
    Serial.printf("Temperature: %.2f°C\n", temperature);
    Serial.printf("Humidity: %.2f\n", humidity);
    Serial.printf("Light Sensitivity: %.2f \n", lightSensitivity);
    Serial.printf("Water Level: %.2f cm\n", waterLevel);

    // Fan control and status
    if (temperature > 25) {
        digitalWrite(FAN_PIN, HIGH);
        fanStatus = true;
        Serial.println("Fan: ON (Temperature > 30°C)");
        tone(BUZ_PIN , HIGH);
        delay(2000);
        noTone(BUZ_PIN );
    } else {
        digitalWrite(FAN_PIN, LOW);
        fanStatus = false;
        Serial.println("Fan: OFF (Temperature <= 30°C)");
    }
    // humidity control and status
    if (humidity < 65) {
        digitalWrite(SPR_PIN, HIGH);
        sprinklerStatus = true;
        Serial.println("Sprinkler: ON (Humidity > 30°C)");
        tone(BUZ_PIN , HIGH);
        delay(2000);
        noTone(BUZ_PIN );
    } else {
        digitalWrite(SPR_PIN, LOW);
        sprinklerStatus = false;
        Serial.println("Sprinkler: OFF (Humidity <= 30°C)");
    }
    // lightSensitivity control and status
    if (lightSensitivity < 1000) {
        digitalWrite(LL_PIN, HIGH);
        lightStatus = true;
        Serial.println("Glow Lights: ON ");
        tone(BUZ_PIN , HIGH);
        delay(2000);
        noTone(BUZ_PIN );
    } else {
        digitalWrite(LL_PIN, LOW);
        lightStatus = false;
        Serial.println("Glow Lights: OFF ");
    }

    // Pump control and status
    if (waterLevel > 10) {
        digitalWrite(PUMP_PIN, HIGH);
        pumpStatus = true;
        Serial.println("Water Pump: ON (Water Level < 10cm)");
        tone(BUZ_PIN , HIGH);
        delay(2000);
        noTone(BUZ_PIN );
    } else {
        digitalWrite(PUMP_PIN, LOW);
        pumpStatus = false;
        Serial.println("Water Pump: OFF (Water Level >= 10cm)");
    }
    Serial.println("---------------------------");
}

// Web page HTML
const char html_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Greenhouse Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Segoe UI', Arial, sans-serif;
            min-height: 100vh;
            background: linear-gradient(135deg, #1a1a1a, #2d2d2d);
            color: #ffffff;
            padding: 2rem;
            display: grid;
            place-items: center;
        }

        .dashboard {
            background: rgba(255, 255, 255, 0.05);
            backdrop-filter: blur(10px);
            padding: 2rem;
            border-radius: 20px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
            width: 100%;
            max-width: 800px;
        }

        h1 {
            text-align: center;
            margin-bottom: 2rem;
            font-size: 2.5rem;
            font-weight: 600;
            background: linear-gradient(45deg, #ffffff, #b3b3b3);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        .grid-container {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 2rem;
        }

        .status-card {
            background: rgba(255, 255, 255, 0.05);
            padding: 1.5rem;
            border-radius: 15px;
            display: flex;
            align-items: center;
            gap: 1rem;
            transition: transform 0.3s ease;
        }

        .status-card:hover {
            transform: translateY(-5px);
        }

        .icon {
            width: 50px;
            height: 50px;
            // filter: invert(1);
        }

        .status-info {
            flex: 1;
        }

        h2 {
            font-size: 1.1rem;
            font-weight: 500;
            margin-bottom: 0.5rem;
            color: #cccccc;
        }

        .status {
            padding: 0.5rem 1rem;
            border-radius: 8px;
            font-weight: 600;
            font-size: 1.2rem;
            text-align: center;
            transition: all 0.3s ease;
        }

        .on {
            background: rgba(46, 213, 115, 0.2);
            color: #2ed573;
            border: 1px solid rgba(46, 213, 115, 0.3);
        }

        .off {
            background: rgba(255, 71, 87, 0.2);
            color: #ff4757;
            border: 1px solid rgba(255, 71, 87, 0.3);
        }

        #temp, #water {
            font-size: 1.4rem;
            font-weight: 600;
            color: #ffffff;
        }

        @media (max-width: 768px) {
            .grid-container {
                grid-template-columns: 1fr;
            }
            
            .dashboard {
                padding: 1.5rem;
            }
            
            h1 {
                font-size: 2rem;
            }
            h3 {
                font-size: 1rem;
                color: rgba(255, 255, 255, 0.4);
                text-align: center;
                margin-top: -1rem;
                margin-bottom: 2rem;
            }
        }
    </style>
</head>
<body>
    <div class="dashboard">
      <h1>ZenGrow</h1>
        <h3>Smart Environmental Monitering System</h3>
        <div class="grid-container">
            <div class="status-card">
                <img src="https://img.icons8.com/?size=100&id=32690&format=png&color=000000" alt="Temperature Icon" class="icon">
                <div class="status-info">
                    <h2>Temperature</h2>
                    <div id="temp">--°C</div>
                </div>
            </div>
            <div class="status-card">
                <img src="https://img.icons8.com/?size=100&id=ebhYZuRS01Tm&format=png&color=000000" alt="Water Level Icon" class="icon">
                <div class="status-info">
                    <h2>Water Level</h2>
                    <div id="water">-- cm</div>
                </div>
            </div>
            <div class="status-card">
                <img src="https://img.icons8.com/?size=100&id=82200&format=png&color=000000" alt="Humidity Icon" class="icon">
                <div class="status-info">
                    <h2>Humidity</h2>
                    <div id="humidity">--%</div>
                </div>
            </div>
            <div class="status-card">
                <img src="https://img.icons8.com/?size=100&id=115943&format=png&color=000000" alt="Light Intensity Icon" class="icon">
                <div class="status-info">
                    <h2>Light Intensity</h2>
                    <div id="light">-- lux</div>
                </div>
            </div>

            <div class="status-card">
                <img src="https://img.icons8.com/?size=100&id=64113&format=png&color=000000" alt="Fan Icon" class="icon">
                <div class="status-info">
                    <h2>Fan Status</h2>
                    <div id="fan" class="status off">OFF</div>
                </div>
            </div>
            <div class="status-card">
                <img src="https://img.icons8.com/?size=100&id=K3QUl33TjZHS&format=png&color=000000" alt="Water Pump Icon" class="icon">
                <div class="status-info">
                    <h2>Water Pump</h2>
                    <div id="pump" class="status off">OFF</div>
                </div>
            </div>
            <div class="status-card">
                <img src="https://img.icons8.com/?size=100&id=17200&format=png&color=000000" alt="Sprinkler Icon" class="icon">
                <div class="status-info">
                    <h2>Sprinkler</h2>
                    <div id="sprinkler" class="status off">OFF</div>
                </div>
            </div>
            <div class="status-card">
                <img src="https://img.icons8.com/?size=100&id=F7YVFKLA0AP7&format=png&color=000000" alt="Glow Lights Icon" class="icon">
                <div class="status-info">
                    <h2>Glow Lights</h2>
                    <div id="glowLights" class="status off">OFF</div>
                </div>
            </div>
        </div>
    </div>

    <script>
        function updateData() {
    console.log('Fetching data from /status...');
    fetch('/status')
        .then(response => {
            console.log('Response status:', response.status);
            return response.json();
        })
        .then(data => {
            // Log received data
            console.log('Received data:', data);
            
            // Temperature update
            const tempValue = data.temperature + "°C";
            console.log('Setting temperature:', tempValue);
            document.getElementById("temp").innerHTML = tempValue;
            
            // Water level update
            const waterValue = data.waterLevel + " cm";
            console.log('Setting water level:', waterValue);
            document.getElementById("water").innerHTML = waterValue;
            
            // Humidity update
            const humidityValue = data.humidity + "%";
            console.log('Setting humidity:', humidityValue);
            document.getElementById("humidity").innerHTML = humidityValue;
            
            // Light intensity update
            const lightValue = data.light + " lux";
            console.log('Setting light intensity:', lightValue);
            document.getElementById("light").innerHTML = lightValue;

            // Status updates with logging
            const devices = {
                'fan': data.fanStatus,
                'pump': data.pumpStatus,
                'sprinkler': data.sprinklerStatus,
                'glowLights': data.lightStatus  // Note: changed from glowLightsStatus to match backend
            };

            // Update each device status
            for (const [device, status] of Object.entries(devices)) {
                console.log(Updating ${device} status:, status);
                const element = document.getElementById(device);
                if (element) {
                    element.className = "status " + (status ? "on" : "off");
                    element.innerHTML = status ? "ON" : "OFF";
                } else {
                    console.error(Element with id "${device}" not found);
                }
            }
        })
        .catch(error => {
            console.error('Error fetching data:', error);
            console.error('Error details:', {
                message: error.message,
                stack: error.stack
            });
        });
}

// Clear any existing intervals first
if (window.updateInterval) {
    clearInterval(window.updateInterval);
}

// Set up new interval and store reference
window.updateInterval = setInterval(updateData, 2000);
updateData(); // Initial update
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", html_page);
}

void handleStatus() {
    String json = "{";
    json += "\"temperature\":" + String(temperature) + ",";
    json += "\"humidity\":" + String(humidity) + ",";
    json += "\"light\":" + String(lightSensitivity) + ",";
    json += "\"waterLevel\":" + String(waterLevel) + ",";
    json += "\"fanStatus\":" + String(fanStatus ? "true" : "false") + ",";
    json += "\"pumpStatus\":" + String(pumpStatus ? "true" : "false")+ ",";
    json += "\"lightStatus\":" + String(lightStatus ? "true" : "false")+ ",";
    json += "\"sprinklerStatus\":" + String(sprinklerStatus ? "true" : "false");
    json += "}";
    server.send(200, "application/json", json);
}

void setup() {
    Serial.begin(115200);
    Serial.println("\nGreenhouse Monitoring System Starting...");
    
    dht.begin();
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(SPR_PIN, OUTPUT);
    pinMode(LL_PIN, OUTPUT);
    pinMode(LDR_PIN, INPUT);
    pinMode(BUZ_PIN , OUTPUT);
    
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Set up web server routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/status", HTTP_GET, handleStatus);
    server.begin();
    Serial.println("HTTP server started");
    Serial.println("System ready!");
}

void loop() {
    server.handleClient();  // Handle web server requests
    updateSensors();       // Update sensor readings and control devices
    delay(2000);          // Delay between readings
}
