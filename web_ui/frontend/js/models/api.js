class DataModel {
    async fetchSensorData(minutes, since = null) {
        try {
            let url = `/api/data?minutes=${minutes}`;
            if (since) {
                // Encode the timestamp so spaces don't break the URL
                url += `&since=${encodeURIComponent(since)}`; 
            }
            const response = await fetch(url);
            if (!response.ok) throw new Error("API Offline");
            return await response.json();
        } catch (e) {
            console.warn("API not reachable.");
            return { status: 'error' };
        }
    }
    
    async fetchLogs() {
        try {
            const response = await fetch('/api/logs');
            if (!response.ok) throw new Error("API Offline");
            return await response.json();
        } catch (e) {
            return { status: 'success', logs: [{ time: new Date().toISOString(), msg: "Waiting for Flask server..." }] };
        }
    }

    async fetchSysInfo() {
        try {
            const response = await fetch('/api/sysinfo');
            return await response.json();
        } catch (e) {
            return { status: 'error' };
        }
    }
}