<style>
    * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
    }

    body {
        font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        min-height: 100vh;
        padding: 20px;
    }

    .container {
        max-width: 1200px;
        margin: 0 auto;
        background: white;
        border-radius: 20px;
        box-shadow: 0 20px 40px rgba(0,0,0,0.1);
        overflow: hidden;
    }

    .header {
        background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%);
        color: white;
        padding: 30px;
        text-align: center;
    }

    .header h1 {
        font-size: 2.5rem;
        margin-bottom: 10px;
        font-weight: 700;
    }

    .header p {
        opacity: 0.9;
        font-size: 1.1rem;
    }

    .system-status {
        background: #f8fafc;
        padding: 20px 30px;
        border-bottom: 1px solid #e2e8f0;
        display: flex;
        justify-content: space-between;
        align-items: center;
        flex-wrap: wrap;
        gap: 15px;
    }

    .status-item {
        display: flex;
        align-items: center;
        gap: 8px;
    }

    .status-indicator {
        width: 10px;
        height: 10px;
        border-radius: 50%;
    }

    .status-indicator.healthy {
        background: #48bb78;
        box-shadow: 0 0 8px rgba(72, 187, 120, 0.5);
    }

    .status-indicator.unhealthy {
        background: #e53e3e;
        box-shadow: 0 0 8px rgba(229, 62, 62, 0.5);
    }

    .controls {
        padding: 30px;
    }

    .valve-grid {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
        gap: 20px;
        margin-bottom: 30px;
    }

    .valve-card {
        background: #f8fafc;
        border-radius: 15px;
        padding: 25px;
        border: 2px solid #e2e8f0;
        transition: all 0.3s ease;
    }

    .valve-card:hover {
        transform: translateY(-2px);
        box-shadow: 0 10px 25px rgba(0,0,0,0.1);
    }

    .valve-card.error {
        border-color: #e53e3e;
        background: #fef5f5;
    }

    .valve-header {
        display: flex;
        justify-content: space-between;
        align-items: center;
        margin-bottom: 15px;
    }

    .valve-name {
        font-size: 1.3rem;
        font-weight: 600;
        color: #2d3748;
    }

    .valve-id {
        font-size: 0.9rem;
        color: #718096;
        background: #e2e8f0;
        padding: 4px 8px;
        border-radius: 6px;
    }

    .valve-info {
        display: grid;
        grid-template-columns: 1fr 1fr;
        gap: 10px;
        margin-bottom: 20px;
        font-size: 0.9rem;
        color: #4a5568;
    }

    .valve-controls {
        display: flex;
        gap: 10px;
        margin-bottom: 15px;
    }

    .btn {
        flex: 1;
        padding: 12px 20px;
        border: none;
        border-radius: 10px;
        font-size: 1rem;
        font-weight: 600;
        cursor: pointer;
        transition: all 0.3s ease;
        text-transform: uppercase;
        letter-spacing: 0.5px;
    }

    .btn-on {
        background: linear-gradient(135deg, #48bb78, #38a169);
        color: white;
    }

    .btn-off {
        background: linear-gradient(135deg, #e53e3e, #c53030);
        color: white;
    }

    .btn-toggle {
        background: linear-gradient(135deg, #4299e1, #3182ce);
        color: white;
    }

    .btn:hover {
        transform: translateY(-1px);
        box-shadow: 0 5px 15px rgba(0,0,0,0.2);
    }

    .btn:disabled {
        opacity: 0.6;
        cursor: not-allowed;
        transform: none !important;
        box-shadow: none !important;
    }

    .valve-status {
        display: flex;
        align-items: center;
        gap: 8px;
        font-size: 0.9rem;
        font-weight: 500;
    }

    .valve-status.on {
        color: #48bb78;
    }

    .valve-status.off {
        color: #718096;
    }

    .valve-status.error {
        color: #e53e3e;
    }

    .master-controls {
        background: #2d3748;
        border-radius: 15px;
        padding: 25px;
        text-align: center;
        margin-bottom: 20px;
    }

    .master-controls h3 {
        color: white;
        margin-bottom: 20px;
        font-size: 1.2rem;
    }

    .master-buttons {
        display: flex;
        gap: 15px;
        justify-content: center;
        flex-wrap: wrap;
    }

    .btn-master {
        padding: 15px 30px;
        border: none;
        border-radius: 10px;
        font-size: 1.1rem;
        font-weight: 600;
        cursor: pointer;
        transition: all 0.3s ease;
        text-transform: uppercase;
        letter-spacing: 0.5px;
        min-width: 120px;
    }

    .btn-all-on {
        background: linear-gradient(135deg, #48bb78, #38a169);
        color: white;
    }

    .btn-all-off {
        background: linear-gradient(135deg, #e53e3e, #c53030);
        color: white;
    }

    .btn-emergency {
        background: linear-gradient(135deg, #d69e2e, #b7791f);
        color: white;
    }

    .btn-master:hover {
        transform: translateY(-2px);
        box-shadow: 0 8px 20px rgba(0,0,0,0.2);
    }

    .loading {
        opacity: 0.7;
        pointer-events: none;
    }

    .connection-status {
        position: fixed;
        top: 20px;
        right: 20px;
        padding: 10px 15px;
        border-radius: 20px;
        font-size: 0.9rem;
        font-weight: 600;
        z-index: 1000;
    }

    .connection-status.connected {
        background: #48bb78;
        color: white;
    }


    @media (max-width: 768px) {
        .valve-grid {
            grid-template-columns: 1fr;
        }
        
        .master-buttons {
            flex-direction: column;
        }
        
        .header h1 {
            font-size: 2rem;
        }

        .system-status {
            flex-direction: column;
            align-items: flex-start;
        }
    }
</style>

<script>
    let systemData = null;
    let isConnected = true;

    async function loadSystemStatus() {
        try {
            const response = await fetch('/api/status');
            if (response.ok) {
                systemData = await response.json();
                updateSystemDisplay();
                updateValveGrid();
                updateConnectionStatus(true);
            } else {
                throw new Error('Failed to load system status');
            }
        } catch (error) {
            console.error('Error loading system status:', error);
            updateConnectionStatus(false);
        }
    }

    function updateSystemDisplay() {
        if (!systemData) return;

        const healthIndicator = document.getElementById('healthIndicator');
        const healthText = document.getElementById('healthText');
        const activeCount = document.getElementById('activeCount');
        const totalCount = document.getElementById('totalCount');
        const systemEnabled = document.getElementById('systemEnabled');

        // Update health status
        if (systemData.healthy) {
            healthIndicator.className = 'status-indicator healthy';
            healthText.textContent = 'Healthy';
        } else {
            healthIndicator.className = 'status-indicator unhealthy';
            healthText.textContent = 'Unhealthy';
        }

        // Update counts
        activeCount.textContent = systemData.active_valves || 0;
        totalCount.textContent = systemData.valve_count || 0;
        systemEnabled.textContent = systemData.system_enabled ? 'Enabled' : 'Disabled';
    }

    function updateValveGrid() {
        if (!systemData || !systemData.valves) return;

        const valveGrid = document.getElementById('valveGrid');
        valveGrid.innerHTML = '';

        systemData.valves.forEach(valve => {
            const valveCard = createValveCard(valve);
            valveGrid.appendChild(valveCard);
        });
    }

    function createValveCard(valve) {
        const card = document.createElement('div');
        card.className = `valve-card ${valve.state === 'error' ? 'error' : ''}`;
        
        card.innerHTML = `
            <div class="valve-header">
                <div class="valve-name">${valve.name}</div>
                <div class="valve-id">ID: ${valve.id}</div>
            </div>
            <div class="valve-info">
                <div>GPIO: ${valve.gpio_pin}</div>
                <div>Mode: ${valve.mode}</div>
                <div>Runtime: ${formatRuntime(valve.total_runtime_seconds)}</div>
                <div>Enabled: ${valve.enabled ? 'Yes' : 'No'}</div>
            </div>
            <div class="valve-controls">
                <button class="btn btn-on" on:click=${() => controlValve(valve.id, 'on')}"
                        disabled=${!valve.enabled}>
                    Turn On
                </button>
                <button class="btn btn-off" on:click=${() => controlValve(valve.id, 'off')}">
                    Turn Off
                </button>
                <button class="btn btn-toggle" on:click=${() => controlValve(valve.id, 'toggle')}"
                        disabled=${!valve.enabled}>
                    Toggle
                </button>
            </div>
            <div class="valve-status ${valve.state}" id="status-${valve.id}">
                <div class="status-indicator ${valve.state}"></div>
                Status: ${valve.state.charAt(0).toUpperCase() + valve.state.slice(1)}</div>
        `;

        return card;
    }

    async function controlValve(valveId, action) {
        try {
            document.body.classList.add('loading');
            
            const response = await fetch(`/api/valve/${valveId}/control`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ action: action })
            });
            
            if (response.ok) {
                const result = await response.json();
                console.log(`Valve ${valveId} ${action} successful:`, result);
                
                // Refresh system status
                await loadSystemStatus();
                updateConnectionStatus(true);
            } else {
                const error = await response.json();
                throw new Error(error.error || 'Failed to control valve');
            }
        } catch (error) {
            console.error('Error controlling valve:', error);
            updateConnectionStatus(false);
            alert(`Failed to ${action} Valve ${valveId}: ${error.message}`);
        } finally {
            document.body.classList.remove('loading');
        }
    }

    async function controlAllValves(action) {
        try {
            document.body.classList.add('loading');
            
            const response = await fetch('/api/valves/all', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ action: action })
            });
            
            if (response.ok) {
                const result = await response.json();
                console.log(`All valves ${action} successful:`, result);
                
                // Refresh system status
                await loadSystemStatus();
                updateConnectionStatus(true);
            } else {
                const error = await response.json();
                throw new Error(error.error || 'Failed to control all valves');
            }
        } catch (error) {
            console.error('Error controlling all valves:', error);
            updateConnectionStatus(false);
            alert(`Failed to turn all valves ${action}: ${error.message}`);
        } finally {
            document.body.classList.remove('loading');
        }
    }

    async function emergencyStop() {
        if (!confirm('Are you sure you want to activate EMERGENCY STOP? This will turn off all valves and disable the system.')) {
            return;
        }

        try {
            document.body.classList.add('loading');
            
            const response = await fetch('/api/emergency-stop', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                }
            });
            
            if (response.ok) {
                const result = await response.json();
                console.log('Emergency stop successful:', result);
                alert('EMERGENCY STOP activated - all valves turned off');
                
                // Refresh system status
                await loadSystemStatus();
                updateConnectionStatus(true);
            } else {
                const error = await response.json();
                throw new Error(error.error || 'Emergency stop failed');
            }
        } catch (error) {
            console.error('Error during emergency stop:', error);
            updateConnectionStatus(false);
            alert(`Emergency stop failed: {error.message}`);
        } finally {
            document.body.classList.remove('loading');
        }
    }

    function updateConnectionStatus(connected) {
        const statusElement = document.getElementById('connectionStatus');
        isConnected = connected;
        
        if (connected) {
            statusElement.textContent = 'Connected';
            statusElement.className = 'connection-status connected';
        } else {
            statusElement.textContent = 'Disconnected';
            statusElement.className = 'connection-status disconnected';
        }
    }

    function formatRuntime(seconds) {
        if (seconds < 60) {
            return `${seconds}s`;
        } else if (seconds < 3600) {
            return `${Math.floor(seconds / 60)}m`;
        } else {
            return `${Math.floor(seconds / 3600)}h ${Math.floor((seconds % 3600) / 60)}m`;
        }
    }

    // Initialize the application
    document.addEventListener('DOMContentLoaded', function() {
        loadSystemStatus();
        
        // Refresh status every 3 seconds
        setInterval(loadSystemStatus, 3000);
    });
</script>

<div class="connection-status" id="connectionStatus">Connected</div>
    
<div class="container">
    <div class="header">
        <h1>💧 ESP32 Valve Controller</h1>
        <p>Standalone Water Management System - AP Mode</p>
    </div>
    
    <div class="system-status" id="systemStatus">
        <div class="status-item">
            <div class="status-indicator healthy" id="healthIndicator"></div>
            <span>System Health: <span id="healthText">Healthy</span></span>
        </div>
        <div class="status-item">
            <span>Active Valves: <span id="activeCount">0</span></span>
        </div>
        <div class="status-item">
            <span>Total Valves: <span id="totalCount">0</span></span>
        </div>
        <div class="status-item">
            <span>System: <span id="systemEnabled">Enabled</span></span>
        </div>
        <div class="status-item">
            <span>WiFi: ESP32-Valve-Controller</span>
        </div>
        <div class="status-item">
            <span>IP: 192.168.4.1</span>
        </div>
    </div>
    
    <div class="controls">
        <div class="master-controls">
            <h3>Master Controls</h3>
            <div class="master-buttons">
                <button class="btn-master btn-all-on"  on:click={() => controlAllValves('on')}>
                    All On
                </button>
                <button class="btn-master btn-all-off"  on:click={() => controlAllValves('off')}>
                    All Off
                </button>
                <button class="btn-master btn-emergency"  on:click={emergencyStop}>
                    Emergency Stop
                </button>
            </div>
        </div>
        
        <div class="valve-grid" id="valveGrid">
            <!-- Valves will be generated by JavaScript -->
        </div>
    </div>
</div>
