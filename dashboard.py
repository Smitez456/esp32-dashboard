import os
from datetime import datetime
from flask import Flask, render_template, jsonify, request

app = Flask(__name__)
alerts = []


def format_alert(data):
    now = datetime.now()
    return {
        "id": len(alerts) + 1,
        "date": now.strftime("%Y-%m-%d"),
        "time": now.strftime("%H:%M:%S"),
        "vehicle_type": data.get("vehicle_type", "Unknown"),
        "vehicle_number": data.get("vehicle_number", "Unknown"),
        "latitude": data.get("latitude", "0.000000"),
        "longitude": data.get("longitude", "0.000000"),
        "status": data.get("status", "Accident confirmed"),
        "severity": data.get("severity", "High"),
        "description": data.get("description", "Accident alert received from ESP32"),
    }


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/api/alerts", methods=["GET"])
def get_alerts():
    return jsonify({"alerts": alerts})


@app.route("/api/submit", methods=["POST"])
def submit_alert():
    payload = request.get_json(force=True, silent=True)
    if not payload:
        return jsonify({"error": "Invalid JSON payload"}), 400

    alert = format_alert(payload)
    alerts.insert(0, alert)
    if len(alerts) > 10:
        alerts.pop()
    return jsonify({"success": True, "alert": alert})


@app.route("/api/clear", methods=["POST"])
def clear_alerts():
    alerts.clear()
    return jsonify({"success": True})


if __name__ == "__main__":
    port = int(os.getenv("PORT", "5000"))
    debug_mode = os.getenv("FLASK_DEBUG", "false").lower() == "true"
    app.run(host="0.0.0.0", port=port, debug=debug_mode)
