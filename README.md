# Smart-Pot-Monitoring-System 🪴

This project integrates environmental sensing and automation using an ESP8266 controller. It transmits soil moisture and temperature data to Blynk via MQTT. The system triggers auto-irrigation when moisture drops below a threshold and enables real-time visual monitoring via ESP32-CAM.



## Project Description
### Motivation
（動機內容）

### Objectives
* 目標一
* 目標二
* 目標三



## Product Design
(這裡放成品的外觀設計草圖&實際照片)
`![Product Design](連結到圖片路徑)`



## System Architecture
### Main Components（內容待更改）
* **Controller:** ESP8266 (NodeMCU), ESP32-CAM（內容待更改）
* **Cloud Platform:** Blynk IoT Cloud（內容待更改）
* **Protocol:** MQTT / HTTP（內容待更改）

### Functions（內容待更改）
1. **Auto-Irrigation:** 當土壤濕度低於設定值時，自動啟動水泵。
2. **Environmental Sensing:** 監測溫度、濕度與光照。
3. **Live Streaming:** 透過網頁端查看植物即時影像。



## Hardware
### Components and Specifications（內容待更改）
| Component | Specification |
| :--- | :--- |
| ESP8266 | Wi-Fi Module for Logic Control |
| Capacitive Soil Moisture Sensor | Corrosion resistant |
| DHT11 | Temp & Humidity Sensor |
| 5V Relay & Pump | For Irrigation |

### Connections and System Design (Wiring Diagram)
(上傳接線圖圖片到 GitHub 檔案庫，然後在這裡引用)
`![Wiring Diagram](docs/wiring-diagram.png)`



## Software
### Features（內容待更改）
* **Threshold Alerts:** 當濕度過低時發送手機推播。
* **History Data:** 記錄並顯示過去 24 小時的環境變化圖表。

### Requirements（內容待更改）
* Arduino IDE (with ESP8266 & Blynk libraries)
* Blynk Mobile App

### Visualization (Mobile & Web Interface)
(貼 Blynk 截圖)

---

## Marketing
### Usage Scenarios
* 情境一：辦公室懶人養植。
* 情境二：出國度假時的遠端植栽照護。

### Competitive Analysis
與市售產品相比，本方案整合了 **ESP32-CAM 即時影像**，且具有更高的客製化彈性。

---

## Website & Source Code
* **Project Page:** [Link to your GitHub Pages]
* **Source Code:** [Found in /src folder](./src)
* **BOM (Bill of Materials):** [View CSV File](./docs/bom.csv)

---

## About Us
| Team Member | Stedunt ID | Responsibilities |
| :--- | :--- | :--- |
| 徐文彬 | M11451013 | Hardware |
| 王才瑜 | M11451015 | Website |
| 呂羿樺 | M11451017 | Software |
| 蔡婉煜 | M11451028 | Github |

