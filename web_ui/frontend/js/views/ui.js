class UI {
    constructor() {
        this.logContainer = document.getElementById('logOutput');
        this.chartInstance = null;
        this.initChart();
    }

    initChart() {
        const ctx = document.getElementById('sensorChart').getContext('2d');
        this.chartInstance = new Chart(ctx, {
            type: 'line',
            data: { labels: [], datasets: [
                { label: 'Temperature (°C)', borderColor: '#ff6384', backgroundColor: 'rgba(255, 99, 132, 0.1)', data: [], yAxisID: 'y' },
                { label: 'Humidity (%)', borderColor: '#36a2eb', backgroundColor: 'rgba(54, 162, 235, 0.1)', data: [], yAxisID: 'y' },
                { label: 'Pressure (hPa)', borderColor: '#ffce56', backgroundColor: 'rgba(255, 206, 86, 0.1)', data: [], yAxisID: 'y1' }
            ]},
            options: {
                responsive: true, maintainAspectRatio: false,
                scales: {
                    x: { ticks: { color: '#aaa' }, grid: { color: '#333' } },
                    y: { type: 'linear', display: true, position: 'left', ticks: { color: '#aaa' }, grid: { color: '#333' } },
                    y1: { type: 'linear', display: true, position: 'right', ticks: { color: '#aaa' }, grid: { drawOnChartArea: false } }
                },
                plugins: { legend: { labels: { color: '#fff', font: { size: 14 } } }, animation: { duration: 0 } }
            }
        });
    }

    updateChart(data) {
        const labels = data.map(d => new Date(d.timestamp).toLocaleTimeString());
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

    toggleSidebar() {
        document.getElementById('configSidebar').classList.toggle('collapsed');
    }

    switchView(targetId) {
        document.querySelectorAll('.view-panel').forEach(p => p.classList.remove('active'));
        document.getElementById(targetId).classList.add('active');
        
        document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
        document.querySelector(`[data-target="${targetId}"]`).classList.add('active');
    }
}