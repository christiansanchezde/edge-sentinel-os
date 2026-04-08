## 🌐 Full Stack Execution & UI Testing

Edge Sentinel OS consists of a C++ data acquisition backend and a Python/Flask UI frontend. They communicate via a shared SQLite database. 

### Option A: Local Testing (Ubuntu WSL)
You can run the entire pipeline on your development machine using the Simulated AI and mock I2C driver.

1. **Start the C++ Backend:**
   In your WSL terminal, compile and run the project locally. This will create and begin writing data to `edge_data.sqlite`.
   ```bash
   cd edge-sentinel-os
   ./manage.sh build
   ./build/src/edge-sentinel-os
   ```
2. **Start the Python Flask Server:**
   Open a *second* WSL terminal window. Install Flask (if not already installed) and run the backend API.
   ```bash
   cd edge-sentinel-os
   pip install flask
   python3 web_ui/backend/app.py
   ```
3. **View the Dashboard:**
   Open your host machine's web browser and navigate to: `http://localhost:5000`

### Option B: Target Execution (Orange Pi 4 Pro)
To run the production build on the edge device using the real hardware NPU and BME280 sensor.

1. **Deploy and Run the C++ Backend:**
   Sync your code via VS Code (`Ctrl+Shift+B`). SSH into the Orange Pi and start the binary with root privileges (required for hardware access).
   ```bash
   cd ~/edge-sentinel-os
   sudo ./build/src/edge-sentinel-os
   ```
2. **Start the Python Flask Server:**
   Open a *second* SSH session to the Orange Pi and start the web UI.
   ```bash
   cd ~/edge-sentinel-os
   python3 web_ui/backend/app.py
   ```
3. **View the Dashboard from any device on the network:**
   Open a web browser on your PC or phone and enter the Orange Pi's IP address:
   `http://<ORANGE_PI_IP_ADDRESS>:5000`

### Option C: VS Code Live Preview (UI Editing Mode)
If you are modifying the HTML/CSS/JS and want real-time feedback without running the Python server:
1. Install the **"Live Preview"** extension by Microsoft in VS Code.
2. Right-click `web_ui/frontend/index.html` in the file explorer and select **"Show Preview"**.
3. The UI will render locally in VS Code and populate with simulated mock data automatically.
