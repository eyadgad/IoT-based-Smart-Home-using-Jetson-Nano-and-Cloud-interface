# IoT-Based Smart Home System Using Jetson Nano and Cloud Interface

This project implements a Smart Home System using a **Jetson Nano** as the primary controller and cloud platforms such as **Adafruit IO** and **Blynk** for remote monitoring and control. The system provides functionalities for controlling devices like lights, fans, and doors, and includes features for remote access, security alerts, and email notifications.

## Features

1. **Remote Control via Cloud**:
   - Adafruit IO dashboard for managing devices.
   - Blynk mobile app integration for remote access.

2. **Device Automation**:
   - Control and monitor devices like lights, fans, doors, and a buzzer.
   - Real-time updates via shared memory between processes.

3. **Security**:
   - Passcode-based access.
   - Alert notifications via email.

4. **Platform Integration**:
   - Integration with **Adafruit IO** for web-based controls.
   - Integration with **Blynk** for mobile app functionality.

5. **Jetson Nano GPIO Management**:
   - Control hardware devices directly using Jetson Nano GPIO pins.

## System Overview

The system is divided into the following key modules:

1. **Main Controller**:
   - Manages shared memory for inter-process communication (IPC).
   - Interfaces with the cloud platforms (Adafruit IO and Blynk).
   - Sends security alerts via email.

2. **Jetson Nano GPIO Controller**:
   - Reads shared memory.
   - Toggles GPIO pins to control connected devices (lights, fans, door lock, buzzer).

3. **Cloud Platforms**:
   - Adafruit IO for web dashboard controls.
   - Blynk for mobile app-based controls.

## Hardware Requirements

1. **Jetson Nano Developer Kit**.
2. Devices to control:
   - Light (LED or bulb).
   - Fan.
   - Door lock (electromagnetic or servo motor-based).
   - Buzzer.
3. Push buttons for manual controls.
4. Wi-Fi module (for internet connectivity).
5. Cloud Accounts:
   - Adafruit IO account.
   - Blynk account.

## Software Requirements

1. **Operating System**: Ubuntu 18.04+ or JetPack for Jetson Nano.
2. **Libraries and Tools**:
   - **CURL** for HTTP requests.
   - **Jetson.GPIO** for GPIO control.
   - **Adafruit IO** and **Blynk** APIs for cloud integration.
3. **C++ Compiler** (e.g., `g++`).

## Setup and Installation

### 1. Hardware Setup
- Connect devices (light, fan, door lock, buzzer) to the Jetson Nano GPIO pins as per the pin assignments in `jetson_main.cpp`.

### 2. Software Setup
#### Install Dependencies
```bash
sudo apt update
sudo apt install libcurl4-openssl-dev
pip3 install Jetson.GPIO
```

#### Compile the Programs
```bash
g++ -o main main.cpp -lcurl
g++ -o jetson_main jetson_main.cpp -lJetsonGPIO
```

### 3. Configure Cloud Platforms
#### Adafruit IO
1. Create an Adafruit IO account.
2. Create feeds for each device (`numberpad`, `lockind`, `buzzind`, etc.).
3. Update the Adafruit API key in `main.cpp`.

#### Blynk
1. Create a Blynk account and set up a project.
2. Add widgets for each device.
3. Update the Blynk API token in `main.cpp`.

### 4. Run the System
1. Start the Jetson Nano GPIO Controller:
   ```bash
   ./jetson_main
   ```
2. Start the Main Controller:
   ```bash
   ./main
   ```

## Usage Instructions

### Device Controls

| **Action**          | **Adafruit IO Feed** | **Blynk Widget** | **Description**                |
|----------------------|----------------------|------------------|--------------------------------|
| Open Door           | `doorind`           | Button (V1)      | Unlocks the door              |
| Turn on/off Light   | `ledind`            | Button (V3)      | Toggles light on/off          |
| Turn on/off Fan     | `fanind`            | Button (V2)      | Toggles fan on/off            |
| Close System        | `system`            | Button (V4)      | Shuts down the system         |

### Security Features
- Enter a 3-digit passcode to access the system.
- Incorrect attempts trigger the buzzer and send an alert email.

## Demo Video

Watch the system in action: [Demo Video](https://drive.google.com/file/d/1haFWrUfN50wMM1XocKvSu_0WO9gYV2Us/view).


## Future Enhancements

1. Add voice control integration (e.g., Alexa, Google Assistant).
2. Extend device compatibility for smart appliances.
3. Implement MQTT protocol for better real-time performance.

---

**Author**: Eyad Gad    
**Contact**: [egad@uwo.ca](mailto:egad@uwo.ca)
