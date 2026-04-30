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

        // Force set the initial theme based on the HTML selection
        const initialTheme = document.getElementById('themeSelect').value;
        this.setTheme(initialTheme);

    }

    initChart() {
        const ctx = document.getElementById('sensorChart').getContext('2d');
        this.chartInstance = new Chart(ctx, {
            type: 'line',
            data: { labels: [], datasets: [
                { label: 'Temp (°C)', borderColor: this.colors.plot1, tension: 0.4, data: [], yAxisID: 'y', borderWidth: 3, pointRadius: 0 },
                { label: 'Humidity (%)', borderColor: this.colors.plot2, tension: 0.4, data: [], yAxisID: 'y', borderWidth: 3, pointRadius: 0 },
                { label: 'Press (hPa)', borderColor: this.colors.plot3, tension: 0.4, data: [], yAxisID: 'y1', borderWidth: 3, pointRadius: 0 }
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
        if (!this.logContainer) return;

        // 1. Check if the user is currently at the bottom BEFORE adding new content
        // We add a 10px "buffer" to account for sub-pixel rounding in different browsers
        const isAtBottom = (this.logContainer.scrollHeight - this.logContainer.scrollTop) <= (this.logContainer.clientHeight + 10);

        if (!logs || logs.length === 0) {
            this.logContainer.innerHTML = '<div class="log-entry" style="opacity: 0.5;">No system events recorded.</div>';
            return;
        }

        // 2. Render the logs
        this.logContainer.innerHTML = logs.map(log => {
            const timeStr = log.time || log.timestamp || new Date().toISOString();
            const msgStr = log.msg || log.message || "Unknown event";
            const isCritical = msgStr.includes('CRITICAL') || msgStr.includes('ERROR');
            
            return `
                <div class="log-entry ${isCritical ? 'critical' : ''}">
                    <span class="log-time">[${new Date(timeStr).toLocaleTimeString()}]</span>
                    <span class="log-msg">${msgStr}</span>
                </div>`;
        }).join('');

        // 3. ONLY auto-scroll if the user was already at the bottom
        if (isAtBottom) {
            this.logContainer.scrollTop = this.logContainer.scrollHeight;
        }
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