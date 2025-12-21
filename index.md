---
title: STM32 FOTA Project
---

# STM32 FOTA Project

## 📌 Overview
This project presents a robust **Firmware Over-The-Air (FOTA)** update system for embedded devices.  
It enables remote firmware updates on an STM32-based target via a cloud interface, removing the need for physical **ST-Link / JTAG** connections.

The system is designed with **reliability, safety, and scalability** in mind, following real-world industrial constraints in embedded and IoT systems.

<p align="center">
  <img src="assets/images/page1.png" width="400">
</p>

---

## 🧠 System Architecture
The system follows a **Gateway–Node architecture**:

1. **Management Console (Node-RED)**  
   Provides a web-based dashboard where users upload the firmware `.bin` file and trigger updates using MQTT.

2. **Connectivity Bridge (ESP32)**  
   Acts as an internet gateway. It subscribes to update commands, downloads the firmware in chunks, and forwards them to the STM32.

3. **Target Device (STM32)**  
   Receives firmware data via UART, verifies its integrity, and programs it into internal Flash memory using a custom Bootloader.

<p align="center">
  <img src="assets/images/arrr.png" width="700">
</p>

---

## 🔧 Technical Deep Dive

### 1️⃣ Memory Mapping (STM32)
The internal Flash memory is carefully partitioned to guarantee safe firmware updates and system stability:

- **Sector 0 (0x0800 0000)**  
  Bootloader – Startup code, command decoding, and update decision logic.

- **Sector 1 (0x0800 4000)**  
  Bootloader – Flash erase/write routines, CRC verification, and controlled jump to application.

- **Application Area (from 0x0800 8000)**  
  Main user firmware.

- **Flags / Metadata Section**  
  Stores update status, firmware version, and integrity indicators.

<p align="center">
  <img src="assets/images/flash_memory.png" width="200">
</p>

> ⚠️ The Bootloader region is protected and never overwritten during firmware updates.

---

### 2️⃣ Firmware Update Process (State Machine)

- **Idle** – Normal execution of the current application  
- **Download** – ESP32 receives firmware chunks via MQTT / HTTP  
- **Write** – STM32 enters Bootloader mode, erases application sectors, and writes new firmware  
- **Verification** – CRC-32 checksum validation  
- **Jump** – Bootloader sets the MSP and jumps to the new application reset handler  

This deterministic state-machine approach ensures safe execution and prevents system corruption.

---

## 🔁 Boot Process & Control Logic
On every reset or power-up, the STM32 begins execution from the **Bootloader**.

The Bootloader:
1. Initializes essential peripherals (clock, UART).
2. Checks the update flag stored in Flash metadata.
3. If an update is requested, validates and programs the new firmware.
4. Otherwise, safely jumps to the existing application.

This mechanism guarantees that the device always boots into a valid firmware image.

---

## 🛡️ Firmware Integrity & Safety Mechanisms
To avoid firmware corruption or device bricking, multiple safety mechanisms were implemented:

- **CRC-32 integrity verification**
- **Firmware header validation** before Flash erase
- **Fail-safe update logic** preserving the Bootloader
- **Controlled application jump** with MSP and reset handler verification

These protections ensure high reliability even in case of power loss or communication failure.

---

## 🌐 Communication Protocol & MQTT Workflow
The system uses **MQTT** for lightweight, scalable communication.

**Update flow:**
1. Node-RED publishes an update command.
2. ESP32 subscribes and downloads the firmware.
3. Firmware chunks are transmitted to STM32 via UART.
4. STM32 responds with ACK/NACK and progress status.

---

## 🖥️ Node-RED Dashboard
The Node-RED interface provides:
- Firmware upload (`.bin`)
- Update trigger control
- Real-time progress visualization
- Status feedback (Idle, Writing, Success, Error)

This allows secure firmware deployment even by non-technical users.

<p align="center">
  <img src="assets/images/dashboard.png" width="700">
</p>

---

## ⚙️ Technologies & Protocols

| Component        | Technology                    | Role |
|------------------|-------------------------------|------|
| Microcontroller | STM32 (ARM Cortex-M)           | Firmware execution & Flash management |
| Connectivity    | ESP32                          | Wi-Fi & MQTT communication |
| Cloud / UI      | Node-RED + MQTT Broker         | Firmware deployment dashboard |
| Communication   | UART                           | ESP32 ↔ STM32 data transfer |
| Language        | C (HAL / LL Drivers)           | Low-level embedded firmware |

---

## ⚙️ Key Features
- [x] **Custom Bootloader** with vector table relocation and safe jump
- [x] **Fail-safe Flash erase** based on firmware validation
- [x] **Chunk-based firmware transfer** supporting large binaries
- [x] **Real-time update monitoring** via Node-RED

---

## 📂 Source Code
🔗 [GitHub Repository](https://github.com/youssef-bendhaou/stm32-fota-project)

---

## 🚀 What I Learned
- STM32 boot sequence and Flash memory architecture
- Bootloader design and firmware update strategies
- IoT communication using MQTT
- End-to-end OTA system implementation

---

## 🔮 Future Developments
<p align="center">
  <img src="assets/images/perspective.png" width="700">
</p>

Planned improvements include:
- Firmware encryption and authentication
- Version control and compatibility checks
- Rollback mechanism for failed updates

---

## 🏁 Conclusion
This project provided hands-on experience in designing a **real-world FOTA system** combining embedded firmware, IoT communication, and cloud-based management.

It reflects industrial requirements such as **reliability, safety, and scalability**, and forms a solid foundation for future work in **embedded systems and IoT engineering**.
