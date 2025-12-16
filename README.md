# Smart-Pot-Monitoring-System 🪴

This project integrates environmental sensing and automation using an ESP8266 controller. It transmits soil moisture and temperature data to Blynk via MQTT. The system triggers auto-irrigation when moisture drops below a threshold and enables real-time visual monitoring via ESP32-CAM.



## Project Description

### Ⅰ. Core Functions
   1. **Real-time Environmental Sensing:**  
   Utilizes high-precision temperature and soil moisture sensors to continuously collect environmental data.

   2. **Automated Irrigation System:**  
   Features an integrated microcontroller that analyzes sensor data and automatically activates the water pump when moisture levels fall below a specific threshold.

   3. **Live Visual Monitoring:**  
   Employs a camera module (ESP32-CAM) to capture and track the plant’s growth and health status in real-time.

   4. **Cross-Platform Data Visualization:**  
   Integrates with the Blynk IoT platform, enabling users to monitor sensor readings and live video feeds via a mobile app or web interface from anywhere.

### Ⅱ. Motivation

We chose this project based on two perspectives:

1. **The Micro Problem (Daily Plant Care)**  
   * **Busy Lifestyles:** Due to busy schedules, people often struggle to keep their plants alive.
   * **Lack of Data:** Traditional care is based on guesswork and lacks the precise data (moisture, temperature) needed for optimal plant health.

2. **The Macro Challenge (IIoT Testbed)**
   * **Industrial Scale:** Modern industries rely on large-scale IIoT systems for monitoring and automation.
   * **Low-Risk Prototyping:** Developing these systems in real-world environments is expensive and risky. Our project serves as a "Mini-Lab" to test the IoT lifecycle in a cost-effective way.

### Ⅲ. Objectives

Our goals are centered on delivering both practical utility and technical validation:

1. **Application Value**
   * **Smart Home Integration:** To deliver a smart home application that integrates sensor data, live visuals, and remote control.
   * **Enhanced Utility:** Showcasing the practical value of IoT beyond simple automated watering, creating a holistic plant-care experience.

2. **Technical Value**
   * **Architecture Validation:** To validate a highly reliable and scalable cloud data architecture.
   * **Scalable Foundation:** Serving as a technical foundation for future expansion into complex industrial scenarios, such as large-scale greenhouse automation.



## Product Design
![草圖](images/Product design draft.png)
![成品照](images/Plot.JPG)



## System Architecture

Our system is built on a standard three-layer IoT architecture, ensuring seamless integration between hardware sensing, logic control, and cloud visualization.

### Ⅰ. Main Components
   * **System Logic Controller (NodeMCU ESP8266):** The central brain of the project, responsible for executing automation logic, sensor data processing, and cloud synchronization.
   * **Independent Vision Unit (ESP32-CAM):** A dedicated module for real-time MJPEG video streaming and snapshots, operating independently to ensure network stability.
   * **Perception Module:** Includes soil moisture and ambient environmental sensors for continuous data collection.
   * **Irrigation Actuator:** A high-reliability relay and pump system that performs physical watering based on controller commands.

### Ⅱ. System Functions
To ensure a robust IIoT testbed, the functions are organized as follows:

   * **Monitor:** Real-time environmental tracking and data normalization (mapping raw sensor signals to percentage values).
   * **Actuation:** Automated irrigation triggered by thresholds, featuring a **Safety Check mechanism** to prevent hardware failure or over-watering.
   * **Communication:**
      * **MQTT:** For telemetry and command exchange with Blynk Cloud.
      * **HTTP:** Local server hosting on ESP32-CAM for low-latency visual feeds.
   * **Data Platform:** Cloud-based dashboard for visualization, historical logging, and remote manual override.

### Ⅲ. Architecture Diagram
The system integrates complex logic flows to ensure the plant receives care even under varying network conditions:
![System Architecture](連結到圖片路徑)
*(Note: Please upload your 3-layer diagram to the 'images' folder)*

* **Core Control Logic:** The ESP8266 executes dual asynchronous timers. Timer #1 polls sensors every 5 seconds, while Timer #2 performs a high-frequency (200ms) **Safety Check** to monitor the relay and pump status.
* **Vision & Streaming Logic:** The ESP32-CAM captures frames every 40ms to maintain a smooth MJPEG stream for the mobile app, while simultaneously supporting snapshot triggers for the web dashboard.
* **State Synchronization:** Using the `BLYNK_CONNECTED` trigger, the system automatically synchronizes local variables (thresholds, modes) with the cloud during startup or reconnection.



## Hardware

### Ⅰ. Components and Specifications
* **ESP8266**
    * **Specifications**: 80MHz CPU, 4MB Flash, 802.11 b/g/n Wi-Fi.
    * **Operation**: Executes the main firmware, manages Wi-Fi reconnection, and handles SSL/TLS for cloud security.
* **ESP32-CAM**
    * **Specifications**: OV2640 lens, 1600 x 1200 UXGA resolution, built-in LED flash.
    * **Operation**: Captures images and encodes MJPEG streams at 25-30 FPS for real-time monitoring.
* **Capacitive Soil Moisture Sensor (v1.2)**
    * **Specifications**: Operating voltage 3.3V - 5.5V, Output 0 - 3.0V DC.
    * **Operation**: Uses capacitive sensing to prevent probe corrosion, ensuring long-term reliability in wet soil.
* **DHT11 Sensor**
    * **Specifications**: Temp range 0-50°C (±2°C), Humidity 20-90% (±5%).
    * **Operation**: Provides supplementary climate data to assess evaporation rates.
* **1-Channel Relay & 5V Water Pump**
    * **Specifications**: Relay triggers at 5V, Pump flow rate 1.5-2.0 L/min.
    * **Operation**: Provides physical isolation between the low-power MCU and the high-current motor.

### Ⅱ. Connections and System Design
The hardware design emphasizes power isolation and precise signal routing.

#### **Pin Mapping Table**
| Component | Microcontroller Pin | Function |
| :--- | :--- | :--- |
| Soil Moisture Sensor | ESP8266 - **A0** | Analog Data Input |
| Relay Module (IN) | ESP8266 - **D1 (GPIO 5)** | Pump Control Signal |
| DHT11 Sensor | ESP8266 - **D2 (GPIO 4)** | Digital Data Input |
| ESP32-CAM | **Independent Power** | Vision Streaming |

#### **Wiring Diagram**
![Wiring Diagram](images/wiring-diagram.jpg)
*(Note: Please ensure the image is uploaded to the 'images' folder)*

#### **Simplified Connection Explanation**
* **Signal Integrity**: The soil moisture sensor is connected to the A0 analog-to-digital converter (ADC) for precise humidity mapping.
* **Power Isolation**: The ESP32-CAM is powered through a separate 5V line to avoid voltage drops during Wi-Fi transmission, which could otherwise cause the ESP8266 to reset.
* **Circuit Protection**: The relay module acts as a buffer, ensuring that back-EMF from the water pump motor does not damage the sensitive GPIO pins of the microcontroller.








## Software

#### Ⅰ. Features
* **Smart Automated Irrigation:** Threshold-based triggering with real-time moisture feedback.
* **Safety Fail-safe System:** High-frequency (200ms) safety timer to prevent pump over-execution.
* **Real-Time Analytics:** Continuous tracking of moisture, temperature, and humidity.
* **Live Video Feed:** 25 FPS MJPEG streaming and instant snapshots via ESP32-CAM.
* **Dual-Platform Access:** Synchronized control via mobile and web interfaces.

#### Ⅱ. Requirements
* **Software:** Arduino IDE (v2.0+), Blynk IoT Cloud account.
* **Libraries:** `BlynkSimpleEsp8266`, `DHT sensor library`, `ESP32_Camera`.
* **Hardware Environment:** ESP8266 & ESP32-CAM setup as defined in [Hardware](#hardware).
* **Network:** Stable 2.4GHz Wi-Fi connection.

#### Ⅲ. Visualization
The system utilizes the Blynk IoT platform to provide a seamless monitoring experience across devices.

**Mobile App Interface**
（ Blynk 手機截圖）
* Provides real-time gauges for environmental data and tactile buttons for manual pump control and mode switching.

**Web Dashboard**
（ Blynk 網頁截圖）
* Offers high-resolution "Super Charts" for long-term trend analysis and an image gallery for ESP32-CAM snapshots.




## Marketing
### Ⅰ. Usage Scenarios
* 情境一：辦公室懶人養植。
* 情境二：出國度假時的遠端植栽照護。

### Ⅱ. Competitive Analysis
與市售產品相比，本方案整合了 **ESP32-CAM 即時影像**，且具有更高的客製化彈性。



## Website & Source Code
* **Project Page:** [Link to your GitHub Pages]
* **Source Code:** [Found in /src folder](./src)
* **BOM (Bill of Materials):** [View CSV File](./docs/bom.csv)



## About Us
| Team Member | Stedunt ID | Responsibilities |
| :--- | :--- | :--- |
| 徐文彬 | M11451013 | Hardware |
| 王才瑜 | M11451015 | Website |
| 呂羿樺 | M11451017 | Software |
| 蔡婉煜 | M11451028 | Github |

