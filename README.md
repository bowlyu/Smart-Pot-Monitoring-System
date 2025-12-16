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

**1. The Micro Problem (Daily Plant Care)**
* **Busy Lifestyles:** Due to busy schedules, people often struggle to keep their plants alive.
* **Lack of Data:** Traditional care is based on guesswork and lacks the precise data (moisture, temperature) needed for optimal plant health.

**2. The Macro Challenge (IIoT Testbed)**
* **Industrial Scale:** Modern industries rely on large-scale IIoT systems for monitoring and automation.
* **Low-Risk Prototyping:** Developing these systems in real-world environments is expensive and risky. Our project serves as a "Mini-Lab" to test the IoT lifecycle in a cost-effective way.

### Ⅲ. Objectives

Our goals are centered on delivering both practical utility and technical validation:

**1. Application Value**
* **Smart Home Integration:** To deliver a smart home application that integrates sensor data, live visuals, and remote control.
* **Enhanced Utility:** Showcasing the practical value of IoT beyond simple automated watering, creating a holistic plant-care experience.

**2. Technical Value**
* **Architecture Validation:** To validate a highly reliable and scalable cloud data architecture.
* **Scalable Foundation:** Serving as a technical foundation for future expansion into complex industrial scenarios, such as large-scale greenhouse automation.


## Product Design
(這裡放成品的外觀設計草圖&實際照片)
`![Product Design](連結到圖片路徑)`



## System Architecture
### Ⅰ. Main Components（內容待更改）
* **Controller:** ESP8266 (NodeMCU), ESP32-CAM（內容待更改）
* **Cloud Platform:** Blynk IoT Cloud（內容待更改）
* **Protocol:** MQTT / HTTP（內容待更改）

### Ⅱ. Functions（內容待更改）
1. **Auto-Irrigation:** 當土壤濕度低於設定值時，自動啟動水泵。
2. **Environmental Sensing:** 監測溫度、濕度與光照。
3. **Live Streaming:** 透過網頁端查看植物即時影像。



## Hardware
### Ⅰ. Components and Specifications（內容待更改）
| Component | Specification |
| :--- | :--- |
| ESP8266 | Wi-Fi Module for Logic Control |
| Capacitive Soil Moisture Sensor | Corrosion resistant |
| DHT11 | Temp & Humidity Sensor |
| 5V Relay & Pump | For Irrigation |

### Ⅱ. Connections and System Design (Wiring Diagram)
(上傳接線圖圖片到 GitHub 檔案庫，然後在這裡引用)
`![Wiring Diagram](docs/wiring-diagram.png)`



## Software
### Ⅰ. Features（內容待更改）
* **Threshold Alerts:** 當濕度過低時發送手機推播。
* **History Data:** 記錄並顯示過去 24 小時的環境變化圖表。

### Ⅱ. Requirements（內容待更改）
* Arduino IDE (with ESP8266 & Blynk libraries)
* Blynk Mobile App

### Ⅲ. Visualization (Mobile & Web Interface)
(貼 Blynk 截圖)



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

