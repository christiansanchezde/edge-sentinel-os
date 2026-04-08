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
        // 1. Bind the circular top-right config toggle
        document.getElementById('btnToggleConfig').addEventListener('click', () => this.view.toggleSidebar());

        // 2. Automatically bind ALL navigation containers to their targets
        document.querySelectorAll('.nav-page-container').forEach(container => {
            container.addEventListener('click', (e) => {
                // Find the closest container in case they clicked the text/icon inside it
                const targetContainer = e.target.closest('.nav-page-container');
                const targetViewId = targetContainer.getAttribute('data-target');
                this.view.switchView(targetViewId);
            });
        });

        // 3. Bind the Time Window dropdown
        document.getElementById('timeWindowSelect').addEventListener('change', (e) => {
            this.dataLimit = parseInt(e.target.value);
            this.refreshData(); 
        });
        
        // 4. Bind the Theme dropdown
        document.getElementById('themeSelect').addEventListener('change', (e) => {
            this.view.setTheme(e.target.value);
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