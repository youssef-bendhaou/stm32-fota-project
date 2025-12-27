# STM32 FOTA Project (Firmware Over-The-Air)

## 📌 Overview
This project implements a **Firmware Over-The-Air (FOTA)** update system using an **STM32 microcontroller** and an **ESP32** as a communication gateway.  
The system allows remote firmware updates without physical access to the device, which is essential for modern **IoT and embedded systems**.

The STM32 runs a **custom bootloader** responsible for receiving, verifying, and flashing new firmware images into internal Flash memory.

---

## 🎯 Objectives
- Design a custom STM32 bootloader
- Enable remote firmware updates (FOTA)
- Ensure firmware integrity using CRC verification
- Separate bootloader and application memory areas
- Simulate a real-world IoT update scenario

---

## 🧠 System Architecture
- **ESP32**
  - Connects to the Internet (Wi-Fi)
  - Retrieves firmware from a server (HTTP / MQTT)
  - Sends firmware to STM32 via UART

- **STM32**
  - Runs a custom bootloader
  - Receives firmware packets
  - Verifies data integrity (CRC)
  - Writes firmware into Flash memory
  - Jumps to the updated application

