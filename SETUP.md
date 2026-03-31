# Environment Setup

This document outlines the setup for the Development Host (WSL) and the Target Edge Device (Orange Pi 4 Pro). The project uses a "Local Git + Remote Compile" architecture.

## 🖥️ 1. Host Setup (Windows 11 / WSL Ubuntu 24.04)

The host machine stores the master repository, handles version control, and runs the IDE.

**Prerequisites:**
* Windows Subsystem for Linux (WSL2) running Ubuntu 24.04 LTS.
* Visual Studio Code installed on Windows with the "WSL" extension.

**Required Packages (WSL):**
```bash
sudo apt update && sudo apt upgrade -y
```
```bash
sudo apt install -y git rsync ssh cmake
```

## 🍓 2. Target Setup (Orange Pi 4 Pro)

The target machine receives the source code via `rsync`, compiles the C++ application, and executes the AI inference and Web UI.

**Prerequisites:**
* Official Ubuntu Server 24.04 LTS image flashed to the Orange Pi.
* Device connected to the local network.

**Required Packages (Orange Pi):**
```bash
sudo apt update && sudo apt upgrade -y
```
```bash
sudo apt install -y build-essential cmake gdb rsync sqlite3 libsqlite3-dev i2c-tools libi2c-dev python3 python3-venv python3-pip
```

## 🔄 3. Synchronization Workflow

To keep GitHub credentials strictly on the Host, this project does not use Git on the Target. 

1. **Write Code:** All development occurs in the WSL environment.
2. **Sync & Build:** VS Code tasks use `rsync` to mirror the source directory to the Orange Pi over SSH (excluding `.git` and local `build` directories).
3. **Remote Execution:** VS Code sends SSH commands to the Orange Pi to run `cmake` and execute the compiled binaries.
4. **Version Control:** All `git commit` and `git push` commands are executed locally in WSL.

*Note: This workflow requires passwordless SSH keys established between the WSL Host and the Orange Pi Target.*

## 🔑 4. Passwordless SSH Configuration

To allow VS Code tasks to seamlessly sync files (`rsync`) and compile (`cmake`) in the background without constantly pausing to ask for a password, you must configure a passwordless SSH keypair.

**Run these commands strictly on your WSL Host:**

- **Generate the SSH Keypair:**
   Use the modern `ed25519` algorithm. When prompted for a passphrase, press **Enter** twice to leave it empty (this is required for background automation).
   ```bash
   ssh-keygen -t ed25519 -C "wsl-to-orangepi"
   ```
- Copy the Public Key to the Target:
- Send the public key to your Orange Pi. Replace user and <orange-pi-ip> with your device's actual login credentials. You will be asked for your Orange Pi password one last time.
    ```bash
    ssh-copy-id user@<orange-pi-ip>
    ```
- Verify the Connection:
    Test the SSH login. You should be instantly dropped into the Orange Pi terminal without a password prompt.
    ```bash
    ssh user@<orange-pi-ip>
    ```

## 🔌 5. Hardware Wiring (Orange Pi 4 Pro & BME280)

The BME280 sensor communicates via the I2C protocol. Ensure the Orange Pi is completely powered off before connecting the pins to the 40-pin GPIO header.

| BME280 Pin | Orange Pi 4 Pro Pin | Function |
| :--- | :--- | :--- |
| **VCC** | Pin 1 | 3.3V Power *(Note: Do not use 5V)* |
| **SDA** | Pin 3 | I2C SDA |
| **SCL** | Pin 5 | I2C SCL |
| **GND** | Pin 6 | Ground |

> **TODO:** schematic

**Verifying the Connection:**
Once wired and the I2C bus is enabled via `orangepi-config`, boot the Pi and use `i2cdetect` to verify the sensor address. The BME280 typically resides at address `0x76` or `0x77`.

```bash
sudo i2cdetect -y 0
```