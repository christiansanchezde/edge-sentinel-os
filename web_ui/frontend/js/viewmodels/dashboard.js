class DashboardViewModel {
    constructor() {
        this.model = new DataModel();
        this.view = new UI();
        
        this.timeWindowMinutes = 5; // Default matches HTML
        this.sensorBuffer = [];     // Holds our live graph data
        this.lastTimestamp = null;  // Tracks the last row we received
        
        this.pollingInterval = null;

        this.bindEvents();
        this.startPolling();
    }

bindEvents() {
        // Safe binding helper
        const bindClick = (id, action) => {
            const el = document.getElementById(id);
            if (el) el.addEventListener('click', action);
        };

        bindClick('btnReadouts', () => this.view.switchView('view-readouts'));
        bindClick('btnStatus', () => this.view.switchView('view-status'));
        bindClick('btnToggleConfig', () => this.view.toggleSidebar());

        const themeSel = document.getElementById('themeSelect');
        if (themeSel) themeSel.addEventListener('change', (e) => this.view.setTheme(e.target.value));

        const timeSel = document.getElementById('timeWindowSelect');
        if (timeSel) timeSel.addEventListener('change', (e) => {
            this.timeWindowMinutes = parseInt(e.target.value);
            this.sensorBuffer = [];
            this.lastTimestamp = null;
            this.refreshData(); 
        });
    }

    async startPolling() {
        // Safely fetch and apply SysInfo
        const sysRes = await this.model.fetchSysInfo();
        if (sysRes.status === 'success') {
            const fwBadge = document.getElementById('badge-fw');
            const aiBadge = document.getElementById('badge-ai');
            if (fwBadge) fwBadge.innerText = `FW: ${sysRes.data.FW_VERSION || '1.0.0'}`;
            if (aiBadge) aiBadge.innerText = `AI: ${sysRes.data.AI_MODE || 'UNKNOWN'}`;
        }

        // Start the engine
        this.refreshData(); 
        this.pollingInterval = setInterval(() => this.refreshData(), 2000); 
    }

    async refreshData() {
        // 1. Fetch only Delta data
        const sensorRes = await this.model.fetchSensorData(this.timeWindowMinutes, this.lastTimestamp);
        
        if (sensorRes.status === 'success' && sensorRes.data.length > 0) {
            // Update our tracker to the newest timestamp received
            this.lastTimestamp = sensorRes.data[sensorRes.data.length - 1].timestamp;
            
            // Append the new rows to our local memory buffer
            this.sensorBuffer.push(...sensorRes.data);
            
            // 2. Prune old data (Slide the window)
            // Calculate what the cutoff time is right now
            const cutoffTime = new Date(Date.now() - (this.timeWindowMinutes * 60 * 1000));
            
            // Keep only rows newer than the cutoff
            // (Note: SQLite timestamps are UTC, we append 'Z' to parse safely in JS)
            this.sensorBuffer = this.sensorBuffer.filter(d => new Date(d.timestamp + 'Z') >= cutoffTime);
            
            // 3. Send the clean buffer to the View
            this.view.updateChart(this.sensorBuffer);
        }

        // Fetch logs normally
        const logRes = await this.model.fetchLogs();
        if (logRes.status === 'success') {
            this.view.updateLogs(logRes.logs);
        }
    }

}