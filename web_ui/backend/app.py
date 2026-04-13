from flask import Flask, jsonify, request, send_from_directory
import sqlite3
import os

# Point Flask to serve the static frontend folder
FRONTEND_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'frontend'))
app = Flask(__name__, static_folder=FRONTEND_DIR, static_url_path='')

# The SQLite database sits at the root of the edge-sentinel-os project
DB_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'edge_data.sqlite'))

def get_db_connection():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn

@app.after_request
def after_request(response):
    response.headers.add('Access-Control-Allow-Origin', '*')
    return response

@app.route('/')
def index():
    return send_from_directory(app.static_folder, 'index.html')

@app.route('/api/data')
def api_data():
    minutes = request.args.get('minutes', 5, type=int)
    since = request.args.get('since') # e.g., "2026-04-13 10:00:00"
    
    try:
        conn = get_db_connection()
        
        # Base query: Get data within the time window
        query = "SELECT timestamp, temperature, humidity, pressure, anomaly_score FROM sensor_logs WHERE timestamp >= datetime('now', ?)"
        params = [f'-{minutes} minutes']
        
        # Delta query: If the frontend gave us a timestamp, ONLY get newer rows
        if since:
            query += " AND timestamp > ?"
            params.append(since)
            
        # Ensure chronological order for the chart (oldest to newest)
        query += " ORDER BY timestamp ASC"
        
        rows = conn.execute(query, params).fetchall()
        conn.close()
        
        return jsonify({"status": "success", "data": [dict(row) for row in rows]})
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

@app.route('/api/logs')
def api_logs():
    try:
        conn = get_db_connection()
        # 1. Fetch the latest 50 system logs (INFO, WARN, CRITICAL, etc.)
        rows = conn.execute(
            'SELECT timestamp, level, tag, message FROM system_logs ORDER BY timestamp DESC LIMIT 50'
        ).fetchall()
        conn.close()
        
        # 2. Map the DB columns to the format the Frontend expects
        logs = []
        for row in rows:
            # We use the 'tag' field to show which file/line the log came from
            logs.append({
                "time": row["timestamp"],
                "msg": f"[{row['level']}] {row['message']} ({row['tag']})"
            })

        if not logs:
            logs = [{"time": "Now", "msg": "System log is empty. Waiting for C++ data..."}]
            
        return jsonify({"status": "success", "logs": logs})
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

@app.route('/api/sysinfo')
def api_sysinfo():
    # Provide defaults
    sysinfo = {
        "FW_VERSION": "1.0.0", 
        "AI_MODE": "UNKNOWN"
    }
    try:
        # Read the config.env file from the project root
        config_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'config.env'))
        with open(config_path, 'r') as f:
            for line in f:
                if '=' in line and not line.startswith('#'):
                    key, val = line.strip().split('=', 1)
                    sysinfo[key] = val
    except Exception as e:
        pass
    
    return jsonify({"status": "success", "data": sysinfo})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)