---
title: STM32 FOTA Project
---

# STM32 FOTA Project

## 📌 Overview
This project demonstrates a robust **Firmware Over-The-Air (FOTA)** update mechanism.  
It allows developers to push new firmware binaries to a remote STM32 device via a cloud interface, eliminating the need for physical **ST-Link / JTAG** connections.

---

## 🧠 System Architecture
The system follows a **Gateway–Node architecture**:

1. **Management Console (Node-RED)**  
   The user uploads the `.bin` file and sends an update trigger via MQTT.

2. **Connectivity Bridge (ESP32)**  
   Acts as the internet gateway. It downloads the firmware in chunks and buffers them.

3. **Target Device (STM32)**  
   Receives data via UART/SPI, verifies integrity, and writes it to internal Flash using a custom Bootloader.

<p align="center">
  <img src="assets/images/arrr.png" width="700">
</p>

---

## 🔧 Technical Deep Dive

### 1️⃣ Memory Mapping (STM32)
The internal Flash is partitioned to ensure system stability:

- **Sector 0 (0x0800 0000)**  
  Custom Bootloader (manages update logic)

- **Sector 1 (0x0800 4000)**  
  Application space (main firmware)

- **Flags Section**  
  Stores update status and firmware version

---

### 2️⃣ Update Process (State Machine)

- **Idle** – Running the current application  
- **Download** – ESP32 receives firmware chunks via MQTT/HTTP  
- **Write** – STM32 enters Bootloader mode, erases Application sectors, writes new data  
- **Verification** – CRC-32 integrity check  
- **Jump** – Bootloader sets MSP and jumps to new application address  

---

## ⚙️ Technologies & Protocols

| Component          | Technology                     | Role |
|-------------------|--------------------------------|------|
| Microcontroller   | STM32 (ARM Cortex-M)            | Main execution & Flash management |
| Connectivity      | ESP32                           | Wi-Fi & MQTT stack |
| Cloud / UI        | Node-RED + MQTT Broker          | Firmware deployment dashboard |
| Communication     | UART                            | ESP32 ↔ STM32 data transfer |
| Language           | C (HAL / LL Drivers)           | Low-level firmware |

---

## ⚙️ Key Features

- [x] **Custom Bootloader** – Vector table relocation & safe jump
- [x] **Fail-Safe Erasure** – Flash erased only if binary header is valid
- [x] **Chunk Handling** – Supports large binaries (1KB / 2KB chunks)
- [x] **Real-Time Progress** – Node-RED dashboard shows update percentage

---

## 📂 Source Code
🔗 [GitHub Repository](https://github.com/yourusername/stm32-fota-project)

---

## 🚀 What I Learned
- Bootloader design on STM32
- Flash memory management
- IoT communication using MQTT
- OTA system architecture

---

## 🔮 Future Improvements
- Firmware encryption
- Version management
- Rollback mechanism
