class DashboardViewModel {
    constructor() {
        this.model = new DataModel();
        this.view = new UI();
        this.dataLimit = 50;
        this.pollingInterval = null;

        this.bindEvents();
        this.startPolling();
    }

    bindEvents() {
        document.getElementById('btnReadouts').addEventListener('click', () => this.view.switchView('view-readouts'));
        document.getElementById('btnStatus').addEventListener('click', () => this.view.switchView('view-status'));
        document.getElementById('btnToggleSidebar').addEventListener('click', () => this.view.toggleSidebar());

        document.getElementById('timeWindowSelect').addEventListener('change', (e) => {
            this.dataLimit = parseInt(e.target.value);
            this.refreshData(); 
        });
    }

    async refreshData() {
        const sensorRes = await this.model.fetchSensorData(this.dataLimit);
        if (sensorRes.status === 'success') {
            this.view.updateChart(sensorRes.data);
        }

        const logRes = await this.model.fetchLogs();
        if (logRes.status === 'success') {
            this.view.updateLogs(logRes.logs);
        }
    }

    startPolling() {
        this.refreshData(); 
        this.pollingInterval = setInterval(() => this.refreshData(), 2000); 
    }
}