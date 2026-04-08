class UI {
    constructor() {
        this.logContainer = document.getElementById('logOutput');
        
        // Grab the KPI DOM elements
        this.valTemp = document.getElementById('val-temp');
        this.valHum = document.getElementById('val-hum');
        this.valPres = document.getElementById('val-pres');

        // Extract CSS Variables for Chart Colors
        const styles = getComputedStyle(document.documentElement);
        this.colors = {
            plot1: styles.getPropertyValue('--plot-line-1').trim(),
            plot2: styles.getPropertyValue('--plot-line-2').trim(),
            plot3: styles.getPropertyValue('--plot-line-3').trim(),
            grid: styles.getPropertyValue('--hmi-border-muted').trim(),
            text: styles.getPropertyValue('--hmi-text-main').trim()
        };

        this.chartInstance = null;
        this.initChart();
    }

    initChart() {
        const ctx = document.getElementById('sensorChart').getContext('2d');
        this.chartInstance = new Chart(ctx, {
            type: 'line',
            data: { labels: [], datasets: [
                { label: 'Temp (°C)', borderColor: this.colors.plot1, backgroundColor: 'transparent', data: [], yAxisID: 'y', borderWidth: 3, pointRadius: 0 },
                { label: 'Humidity (%)', borderColor: this.colors.plot2, backgroundColor: 'transparent', data: [], yAxisID: 'y', borderWidth: 3, pointRadius: 0 },
                { label: 'Pressure (hPa)', borderColor: this.colors.plot3, backgroundColor: 'transparent', data: [], yAxisID: 'y1', borderWidth: 3, pointRadius: 0 }
            ]},
            options: {
                responsive: true, maintainAspectRatio: false,
                scales: {
                    x: { ticks: { color: this.colors.text }, grid: { color: this.colors.grid } },
                    y: { type: 'linear', display: true, position: 'left', ticks: { color: this.colors.text }, grid: { color: this.colors.grid } },
                    y1: { type: 'linear', display: true, position: 'right', ticks: { color: this.colors.text }, grid: { drawOnChartArea: false } }
                },
                plugins: { 
                    legend: { labels: { color: this.colors.text, font: { size: 14, weight: 'bold' } } },
                    animation: { duration: 0 } 
                }
            }
        });
    }

    updateChart(data) {
        if (!data || data.length === 0) return;

        // 1. Update the Bento Grid KPIs with the latest reading (last item in array)
        const latest = data[data.length - 1];
        this.valTemp.innerText = `${latest.temperature.toFixed(1)} °C`;
        this.valHum.innerText = `${latest.humidity.toFixed(1)} %`;
        this.valPres.innerText = `${latest.pressure.toFixed(0)} hPa`;

        // 2. Update the Chart
        const labels = data.map(d => new Date(d.timestamp).toLocaleTimeString([], {hour: '2-digit', minute:'2-digit', second:'2-digit'}));
        this.chartInstance.data.labels = labels;
        this.chartInstance.data.datasets[0].data = data.map(d => d.temperature);
        this.chartInstance.data.datasets[1].data = data.map(d => d.humidity);
        this.chartInstance.data.datasets[2].data = data.map(d => d.pressure);
        this.chartInstance.update();
    }

    updateLogs(logs) {
        this.logContainer.innerHTML = logs.map(log => 
            `<div class="log-entry">
                <span class="log-time">[${new Date(log.time).toLocaleTimeString()}]</span>
                <span class="log-msg ${log.msg.includes('CRITICAL') ? 'critical' : ''}">${log.msg}</span>
             </div>`
        ).join('');
    }

    toggleSidebar() { document.getElementById('configSidebar').classList.toggle('collapsed'); }
    
    switchView(targetId) {
        document.querySelectorAll('.view-panel').forEach(p => p.classList.remove('active'));
        document.getElementById(targetId).classList.add('active');
        
        // Update selection state for the new interactive containers
        document.querySelectorAll('.nav-page-container').forEach(b => b.classList.remove('active'));
        document.querySelector(`[data-target="${targetId}"]`).classList.add('active');
    }

    setTheme(themeName) {
        // 1. Swap the CSS variables on the root document
        document.documentElement.setAttribute('data-theme', themeName);

        // 2. Re-extract the new CSS variables
        const styles = getComputedStyle(document.documentElement);
        this.colors = {
            plot1: styles.getPropertyValue('--plot-line-1').trim(),
            plot2: styles.getPropertyValue('--plot-line-2').trim(),
            plot3: styles.getPropertyValue('--plot-line-3').trim(),
            grid: styles.getPropertyValue('--hmi-border-muted').trim(),
            text: styles.getPropertyValue('--hmi-text-main').trim()
        };

        // 3. Force Chart.js to update its colors
        if (this.chartInstance) {
            // Update line colors
            this.chartInstance.data.datasets[0].borderColor = this.colors.plot1;
            this.chartInstance.data.datasets[1].borderColor = this.colors.plot2;
            this.chartInstance.data.datasets[2].borderColor = this.colors.plot3;
            
            // Update grid and text colors
            this.chartInstance.options.scales.x.ticks.color = this.colors.text;
            this.chartInstance.options.scales.x.grid.color = this.colors.grid;
            this.chartInstance.options.scales.y.ticks.color = this.colors.text;
            this.chartInstance.options.scales.y.grid.color = this.colors.grid;
            this.chartInstance.options.scales.y1.ticks.color = this.colors.text;
            this.chartInstance.options.plugins.legend.labels.color = this.colors.text;

            this.chartInstance.update();
        }
    }
}