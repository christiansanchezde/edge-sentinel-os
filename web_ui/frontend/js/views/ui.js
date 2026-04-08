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
        document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
        document.querySelector(`[data-target="${targetId}"]`).classList.add('active');
    }
}