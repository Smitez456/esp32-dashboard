# ESP32 Wi-Fi Location Tracking Dashboard

This demo contains a working dashboard and a matching ESP32 sketch for Wi-Fi-based accident tracking.

## What is included

- `dashboard.py`: Flask server exposing a dashboard UI and REST API endpoints
- `templates/index.html`: Interactive dashboard with light/dark mode and smart alert modules
- `static/styles.css`: Dashboard styling for both themes
- `static/app.js`: Dashboard logic, alert management, and API integration
- `esp32_location_accident.ino`: ESP32 sketch for Wi-Fi scan, Google Geolocation API, accident detection, and dashboard upload
- `requirements.txt`: Python dependencies for the dashboard server

## Run the dashboard

1. Create a local virtual environment and install dependencies:

```bash
cd esp32_dashboard
python -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r requirements.txt
```

2. Start the server without activating the venv:

```bash
cd esp32_dashboard
.\.venv\Scripts\python.exe dashboard.py
```

3. Open the dashboard in your browser:

```
http://localhost:5000
```

> If PowerShell rejects `activate` because running scripts is disabled, use the direct venv interpreter path shown above.

## ESP32 setup

1. Open `esp32_dashboard/esp32_location_accident.ino` in the Arduino IDE.
2. Replace `YOUR_WIFI_SSID`, `YOUR_WIFI_PASSWORD`, `YOUR_GOOGLE_API_KEY`, and `dashboardHost` with your values.
3. Upload to an ESP32 board.

## Demo test mode

Run the demo sender directly with the virtual environment interpreter:

```bash
cd esp32_dashboard
.\.venv\Scripts\python.exe demo_send_alert.py
```

This script submits sample accident alerts to the dashboard server so you can verify UI behavior and alert handling.

## Public Internet Access (Port Forwarding)

To allow users from **any WiFi network** to access your dashboard without third-party services:

## Public Deployment

This project is now ready to deploy on a public server or container host.

### Deploy with a cloud platform

Use any platform that supports Python Flask apps or Docker containers.

- If your host supports `Procfile` (Heroku, Render, Railway): the app will run via `gunicorn`
- If you prefer containers: use the provided `Dockerfile`

### Deploy using Docker

```bash
cd esp32_dashboard
docker build -t esp32-dashboard .
docker run -p 5000:5000 esp32-dashboard
```

Then the public host should expose port `5000` to the internet.

### Deploy using a Python web host

The app uses `dashboard.py` as the Flask entrypoint. Your host should start it with either:

```bash
gunicorn dashboard:app --bind 0.0.0.0:$PORT --workers 2
```

or via the included `Procfile`.

### What to share

Once deployed, share the public URL from your host provider, for example:

```bash
https://yourapp.example.com
```

## Notes

- The dashboard supports dark/light theme and interactive alert modules.
- The ESP32 sketch sends accident data to the dashboard via HTTP POST.
- For security, prefer a host with HTTPS and authentication.
