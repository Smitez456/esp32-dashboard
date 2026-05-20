const API_BASE = "/api";
const alertList = document.getElementById("alert-list");
const alertCount = document.getElementById("alert-count");
const vehicleCount = document.getElementById("vehicle-count");
const clearCount = document.getElementById("clear-count");
const currentLat = document.getElementById("current-lat");
const currentLng = document.getElementById("current-lng");
const themeToggle = document.getElementById("theme-toggle");
const clearButton = document.getElementById("clear-button");
const simulateForm = document.getElementById("simulate-form");

let clearedAlarmCount = 0;

function setTheme(theme) {
  document.documentElement.setAttribute("data-theme", theme);
  localStorage.setItem("dashboardTheme", theme);
  themeToggle.textContent = theme === "dark" ? "Light Mode" : "Dark Mode";
}

function loadTheme() {
  const savedTheme = localStorage.getItem("dashboardTheme") || "light";
  setTheme(savedTheme);
}

function formatAlert(alert) {
  return `
    <article class="alert-item">
      <strong>${alert.vehicle_type} • ${alert.vehicle_number}</strong>
      <div>${alert.description}</div>
      <div class="alert-meta">
        <span>${alert.date} ${alert.time}</span>
        <span>Location: ${alert.latitude}, ${alert.longitude}</span>
        <span>Status: ${alert.status}</span>
      </div>
    </article>
  `;
}

async function loadAlerts() {
  try {
    const response = await fetch(`${API_BASE}/alerts`);
    const data = await response.json();
    const alerts = data.alerts || [];
    alertCount.textContent = alerts.length;
    vehicleCount.textContent = Math.max(1, alerts.length);

    if (alerts.length === 0) {
      alertList.innerHTML = '<div class="empty-state">No active alerts yet. Submit a test alert or wait for ESP32 data.</div>';
      currentLat.textContent = "0.000000";
      currentLng.textContent = "0.000000";
      return;
    }

    alertList.innerHTML = alerts.map(formatAlert).join("");
    currentLat.textContent = alerts[0].latitude;
    currentLng.textContent = alerts[0].longitude;
  } catch (error) {
    alertList.innerHTML = '<div class="empty-state">Unable to load alerts. Check the server connection.</div>';
    console.error(error);
  }
}

async function clearAlerts() {
  try {
    const response = await fetch(`${API_BASE}/clear`, { method: "POST" });
    const result = await response.json();
    if (result.success) {
      clearedAlarmCount += 1;
      clearCount.textContent = clearedAlarmCount;
      await loadAlerts();
    }
  } catch (error) {
    console.error(error);
  }
}

async function sendSimulatedAlert(event) {
  event.preventDefault();
  const payload = {
    vehicle_type: document.getElementById("vehicle-type").value,
    vehicle_number: document.getElementById("vehicle-number").value,
    latitude: document.getElementById("latitude").value,
    longitude: document.getElementById("longitude").value,
    status: document.getElementById("status").value,
    description: "Manual simulation from dashboard UI",
  };

  try {
    const response = await fetch(`${API_BASE}/submit`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload),
    });
    const result = await response.json();
    if (result.success) {
      await loadAlerts();
      simulateForm.reset();
    }
  } catch (error) {
    console.error(error);
  }
}

function attachModuleActions() {
  document.querySelectorAll(".module-action").forEach((button) => {
    button.addEventListener("click", () => {
      const targetId = button.dataset.target;
      const target = document.getElementById(targetId);
      target.classList.toggle("expanded");
    });
  });
}

themeToggle.addEventListener("click", () => {
  setTheme(document.documentElement.getAttribute("data-theme") === "dark" ? "light" : "dark");
});

clearButton.addEventListener("click", clearAlerts);
simulateForm.addEventListener("submit", sendSimulatedAlert);

loadTheme();
attachModuleActions();
loadAlerts();
setInterval(loadAlerts, 6000);
