<div align="center">

# STM32 FOTA Project

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-STM32%20%7C%20ESP32-green.svg)
![Language](https://img.shields.io/badge/language-C%20%7C%20C++-orange.svg)
![Status](https://img.shields.io/badge/status-production--ready-success.svg)

**A robust Firmware Over-The-Air (FOTA) update system for embedded IoT devices**

[Live Demo](https://youssef-bendhaou.github.io/stm32-fota-project/) • [Documentation](#-documentation) • [Contributing](CONTRIBUTING.md)

</div>

---

## 📌 Overview

This project implements a production-ready **Firmware Over-The-Air (FOTA)** update system using an **STM32 microcontroller** and an **ESP32** as a communication gateway.  

The system enables remote firmware updates without physical access to the device, which is essential for modern **IoT and embedded systems**. The STM32 runs a **custom bootloader** responsible for receiving, verifying, and flashing new firmware images into internal Flash memory with comprehensive safety mechanisms.

### 🎯 Key Features

- ✅ **Custom Bootloader** with safe memory management
- 🔄 **Remote Firmware Updates** via MQTT/HTTP
- 🔐 **CRC-32 Integrity Verification** for all firmware
- 🛡️ **Fail-safe Operations** preventing device bricking
- 📊 **Real-time Monitoring** via Node-RED Dashboard
- 🚀 **Chunked Transfer** supporting large firmware files
- 🌐 **MQTT-based Communication** for IoT integration
- ⚡ **Fast and Reliable** firmware deployment

---

## 🧠 System Architecture

### High-Level Overview

```
┌─────────────────┐         ┌─────────────────┐         ┌─────────────────┐
│   Node-RED      │  MQTT   │     ESP32       │  UART   │     STM32       │
│   Dashboard     │ ◄──────► │   Gateway       │ ◄──────► │   Target        │
│                 │         │  (Wi-Fi/MQTT)   │         │  (Bootloader)   │
└─────────────────┘         └─────────────────┘         └─────────────────┘
       Cloud                      Connectivity               Edge Device
```

### Components

| Component | Role | Technologies |
|-----------|------|--------------|
| **Management Console** | Firmware upload & update triggering | Node-RED + MQTT Broker |
| **Connectivity Bridge** | Internet gateway & data relay | ESP32 + WiFi + MQTT |
| **Target Device** | Firmware execution & Flash management | STM32 + Custom Bootloader |

### ESP32 (Connectivity Bridge)
- Connects to the Internet via WiFi
- Subscribes to MQTT update commands
- Retrieves firmware from server (HTTP/MQTT)
- Transmits firmware chunks to STM32 via UART
- Manages reconnection and error handling

### STM32 (Target Device)
- Executes custom bootloader on startup
- Receives firmware packets via UART
- Verifies data integrity using CRC-32
- Writes firmware into Flash memory with sector management
- Safely jumps to the updated application
- Implements fail-safe update logic

---

## 🔧 Technical Deep Dive

### 1️⃣ Memory Mapping (STM32)

The internal Flash memory is carefully partitioned to guarantee safe firmware updates and system stability:

```
┌─────────────────────────────────────────────┐
│  0x0800 0000  │  Bootloader (Sector 0-1)    │
│                │  - Startup code            │
│                │  - Command decoding        │
│                │  - Flash operations        │
├─────────────────────────────────────────────┤
│  0x0800 8000  │  Application Area          │
│                │  - Main firmware           │
│                │  - User code               │
├─────────────────────────────────────────────┤
│  Last Sectors │  Flags & Metadata          │
│                │  - Update status           │
│                │  - Firmware version        │
│                │  - Integrity indicators    │
└─────────────────────────────────────────────┘
```

> ⚠️ **Important**: The Bootloader region is protected and never overwritten during firmware updates.

### 2️⃣ Firmware Update Process (State Machine)

```
┌─────────┐     ┌──────────┐     ┌──────┐     ┌──────────────┐     ┌──────┐
│  Idle   │ ──► │ Download │ ──► │ Write│ ──► │ Verification │ ──► │ Jump │
└─────────┘     └──────────┘     └──────┘     └──────────────┘     └──────┘
     │                                   ▲               │
     └───────────────────────────────────┴───────────────┘
                    (On Error - Rollback/Retry)
```

**States:**
- **Idle** - Normal application execution
- **Download** - ESP32 receives firmware chunks via MQTT/HTTP
- **Write** - STM32 erases sectors and writes new firmware
- **Verification** - CRC-32 checksum validation
- **Jump** - Bootloader sets MSP and jumps to new application

### 3️⃣ Boot Process & Control Logic

On every reset or power-up:

1. **Initialize** essential peripherals (clock, UART)
2. **Check update flag** in Flash metadata
3. **If update requested:**
   - Validate new firmware
   - Program to application area
   - Verify CRC-32 checksum
4. **Otherwise:**
   - Verify existing application
   - Jump safely to application reset handler

This ensures the device always boots into a valid firmware image.

### 4️⃣ Safety Mechanisms

Multiple protection layers ensure reliability:

| Mechanism | Purpose |
|-----------|---------|
| CRC-32 Verification | Ensures firmware integrity |
| Firmware Header Validation | Pre-check before Flash erase |
| Fail-safe Update Logic | Preserves Bootloader on failure |
| Controlled Application Jump | MSP and reset handler verification |
| Sector Protection | Prevents overwriting critical areas |

---

## 📦 Installation & Setup

### Prerequisites

- STM32 development board (tested on STM32F103)
- ESP32 development board
- STM32CubeIDE or Keil MDK
- PlatformIO or Arduino IDE for ESP32
- MQTT Broker (e.g., Mosquitto)
- Node-RED installed and running

### STM32 Setup

1. **Clone the repository:**
   ```bash
   git clone https://github.com/youssef-bendhaou/stm32-fota-project.git
   cd stm32-fota-project/stm32-code
   ```

2. **Open in STM32CubeIDE**
   - Import the project
   - Configure clock settings if needed
   - Build the bootloader

3. **Flash the Bootloader:**
   - Connect ST-Link/JTAG debugger
   - Flash the bootloader binary to address `0x08000000`
   - Ensure Bootloader occupies first 32KB

### ESP32 Setup

1. **Navigate to ESP32 code:**
   ```bash
   cd esp32-code
   ```

2. **Configure WiFi and MQTT:**
   - Edit `main.cpp` with your WiFi credentials
   - Set MQTT broker address and credentials

3. **Build and Upload:**
   ```bash
   # Using PlatformIO
   pio run --target upload
   ```

### Node-RED Setup

1. **Install Node-RED** (if not already installed)
   ```bash
   npm install -g node-red
   node-red
   ```

2. **Import the flow:**
   - Access Node-RED dashboard at `http://localhost:1880`
   - Import the provided flow configuration
   - Configure MQTT broker settings

---

## 🚀 Usage

### Updating Firmware

1. **Prepare Firmware:**
   - Build your STM32 application
   - Ensure it starts at address `0x08008000`
   - Export as `.bin` file

2. **Upload via Node-RED:**
   - Open Node-RED dashboard
   - Select firmware file (.bin)
   - Click "Update Firmware"

3. **Monitor Progress:**
   - Watch real-time progress indicators
   - Verify success status
   - Check device logs for errors

### Troubleshooting

**Issue:** Device not booting after update
- **Solution:** Check CRC-32 verification and try again

**Issue:** MQTT connection failures
- **Solution:** Verify broker credentials and network connectivity

**Issue:** Flash write errors
- **Solution:** Ensure proper sector addresses and permissions

---

## 📊 Node-RED Dashboard

The web-based interface provides:

- ✅ **Firmware Upload** - Drag and drop `.bin` files
- 🎮 **Update Control** - Trigger and manage updates
- 📈 **Real-time Progress** - Visual progress indicators
- 📝 **Status Feedback** - Idle, Writing, Success, Error states

![Node-RED Dashboard](assets/images/dashboard.png)

---

## 📁 Project Structure

```
stm32-fota-project/
├── stm32-code/              # STM32 bootloader and firmware
│   ├── Bootloader.c         # Bootloader implementation
│   ├── Bootloader.h         # Bootloader header
│   ├── main.c               # Application code
│   └── README.md            # STM32-specific docs
├── esp32-code/              # ESP32 gateway code
│   ├── main.cpp             # ESP32 firmware
│   └── README.md            # ESP32-specific docs
├── assets/images/           # Documentation images
├── index.html              # Project website
├── README.md               # This file
├── CONTRIBUTING.md         # Contribution guidelines
└── LICENSE                 # MIT License
```

---

## 🔬 Testing

### Unit Tests
- Bootloader command parsing
- CRC-32 verification
- Flash write operations

### Integration Tests
- End-to-end firmware update flow
- MQTT communication reliability
- Error handling and recovery

### Manual Tests
- Successful firmware update
- Failed update recovery
- Power loss during update

---

## 🔮 Future Developments

Planned enhancements:

- 🔒 Firmware encryption and authentication (AES/RSA)
- 📋 Version control and compatibility checks
- ↩️ Automatic rollback mechanism for failed updates
- 🌍 Multi-device management
- 📊 Usage analytics and telemetry
- 🔄 Delta updates for bandwidth optimization
- 🛡️ Secure boot implementation

---

## 📚 Documentation

- [STM32 Code Guide](stm32-code/README.md)
- [ESP32 Code Guide](esp32-code/README.md)
- [Live Website](https://youssef-bendhaou.github.io/stm32-fota-project/)
- [Contributing Guidelines](CONTRIBUTING.md)

---

## 🤝 Contributing

Contributions are welcome! Please read our [Contributing Guidelines](CONTRIBUTING.md) before submitting pull requests.

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 👨‍💻 Author

**Yousef Bendhaou**

- GitHub: [@youssef-bendhaou](https://github.com/youssef-bendhaou)
- Project: [STM32 FOTA Project](https://github.com/youssef-bendhaou/stm32-fota-project)

---

## 🙏 Acknowledgments

- STM32 HAL library and documentation
- ESP32 Arduino core and platform
- Node-RED community
- MQTT protocol developers

---

<div align="center">

**If this project helped you, please consider giving it a ⭐**

[⬆ Back to Top](#stm32-fota-project)

</div>
