# ZenGrow – Smart Environmental Monitoring System

ZenGrow is a fully simulated smart greenhouse project built as part of our university course *Electronics and Physical Computing*. It uses sensors and ESP32 to monitor environmental parameters and simulate automated control of devices through LED indicators, with real-time data displayed on a responsive web dashboard.

## Project Overview

ZenGrow monitors:
- Temperature (via DHT11)
- Humidity (via DHT11)
- Water Level (via Ultrasonic Sensor)
- Light Intensity (via LDR)

It then simulates automatic activation of:
- Fan (via LED)
- Sprinkler (via LED)
- Grow Lights (via LED)
- Water Pump (via LED)

All data is sent to a web dashboard hosted directly on the ESP32 and updates every 2 seconds.

## Features

- *Real-time sensor data collection*
- *Automated device control simulation*
- *Live web dashboard*
- *Built-in alerts via buzzer *
- *Stylish and responsive UI with live status updates*

## Technologies Used

- *ESP32*
- *DHT11 Sensor*
- *HC-SR04 Ultrasonic Sensors*
- *LDR (Light Dependent Resistor)*
- *LEDs and Buzzer*
- *HTML/CSS/JavaScript*
- *WiFi & WebServer Library (ESP32)*

## Components List

- ESP32
- DHT11 Sensor – 1
- Ultrasonic Sensors – 1
- LDR – 1
- Buzzer – 1
- Potentiometer – 1
- LEDs – 5
- Breadboard, wires, resistors

## How It Works

1. The ESP32 reads sensor data every 2 seconds.
2. Based on threshold values:
   - Turns ON/OFF respective LEDs (simulated devices).
   - Triggers buzzer alerts.
3. Hosts a local web dashboard accessible via IP (shown in Serial Monitor).
4. JavaScript fetches and displays data dynamically on the dashboard.


## Setup Instructions

1. Connect the components as per the circuit.
2. Upload the Arduino code to ESP32.
3. Change your WiFi SSID and password in the code.
4. Monitor the Serial for IP address and access the dashboard.

