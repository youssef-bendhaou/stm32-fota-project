---
title: STM32 FOTA Project
---


## 📌 Overview
This project demonstrates a robust Firmware Over-The-Air (FOTA) update mechanism. It allows developers to push new firmware binaries to a remote STM32 device via a cloud interface, eliminating the need for physical ST-Link/JTAG connections.

## 🧠 System Architecture
The system follows a Gateway-Node architecture:
  1/ Management Console (Node-RED): The user uploads the .bin file and sends an update trigger via MQTT.
  2/ Connectivity Bridge (ESP32): Acts as the internet gateway. It downloads the binary in chunks and buffers them.
  3/ Target Device (STM32): Receives the data via UART/SPI, verifies integrity, and writes it to the internal Flash memory using a custom Bootloader.
![Architecture](assets/images/arrr.png)


## 🔧 Technologies Used
- **STM32** – Custom bootloader
- **ESP32**
- **MQTT**
- **Node-RED**

## ⚙️ Key Features
- STM32 bootloader implementation
- Flash memory erase and write
- Remote firmware update
- Secure MQTT communication

## 🎯 Objective
Improve **maintenance and scalability** of IoT systems without physical access.

## 📂 Source Code
🔗 [GitHub Repository](https://github.com/yourusername/stm32-fota-project)

## 🚀 What I Learned
- Bootloader design on STM32
- Flash memory management
- IoT communication using MQTT
- OTA system architecture

## 🔮 Future Improvements
- Firmware encryption
- Version management
- Rollback mechanism
