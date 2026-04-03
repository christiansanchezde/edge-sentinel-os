class DataModel {
    async fetchSensorData(limit) {
        try {
            // Force 127.0.0.1 so VS Code Live Preview can hit the local Flask server
            const response = await fetch(`http://127.0.0.1:5000/api/data?limit=${limit}`);
            if (!response.ok) throw new Error("API Offline");
            return await response.json();
        } catch (e) {
            console.warn("API not reachable, using mock data for UI preview.");
            return { 
                status: 'success', 
                data: [
                    { timestamp: new Date(Date.now() - 4000).toISOString(), temperature: 24.1, humidity: 45, pressure: 1010 },
                    { timestamp: new Date(Date.now() - 2000).toISOString(), temperature: 24.5, humidity: 46, pressure: 1011 },
                    { timestamp: new Date().toISOString(), temperature: 25.0, humidity: 48, pressure: 1013 }
                ] 
            };
        }
    }
    
    async fetchLogs() {
        try {
            const response = await fetch('http://127.0.0.1:5000/api/logs');
            if (!response.ok) throw new Error("API Offline");
            return await response.json();
        } catch (e) {
            return { status: 'success', logs: [{ time: new Date().toISOString(), msg: "UI Preview Mode Active. Waiting for Flask server..." }] };
        }
    }
}