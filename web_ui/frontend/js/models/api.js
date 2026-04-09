class DataModel {
    async fetchSensorData(limit) {
        try {
            // Using relative path so it dynamically uses the Pi's IP
            const response = await fetch(`/api/data?limit=${limit}`);
            if (!response.ok) throw new Error("API Offline");
            return await response.json();
        } catch (e) {
            console.warn("API not reachable, using mock data for UI preview.");
            return { 
                status: 'success', 
                data: [
                    { timestamp: new Date().toISOString(), temperature: 0, humidity: 0, pressure: 0 }
                ] 
            };
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
}