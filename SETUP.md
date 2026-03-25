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
sudo apt install -y build-essential cmake gdb rsync sqlite3 libsqlite3-dev i2c-tools libi2c-dev python3 python3-venv python3-pip
```

## 🔄 3. Synchronization Workflow

To keep GitHub credentials strictly on the Host, this project does not use Git on the Target. 

1. **Write Code:** All development occurs in the WSL environment.
2. **Sync & Build:** VS Code tasks use `rsync` to mirror the source directory to the Orange Pi over SSH (excluding `.git` and local `build` directories).
3. **Remote Execution:** VS Code sends SSH commands to the Orange Pi to run `cmake` and execute the compiled binaries.
4. **Version Control:** All `git commit` and `git push` commands are executed locally in WSL.

*Note: This workflow requires passwordless SSH keys established between the WSL Host and the Orange Pi Target.*