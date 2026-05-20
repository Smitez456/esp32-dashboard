#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* geolocationKey = "YOUR_GOOGLE_API_KEY";
const char* dashboardHost = "192.168.1.100"; // Replace with your dashboard server IP
const int dashboardPort = 5000;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32 Accident");
  lcd.setCursor(0, 1);
  lcd.print("Dashboard Ready");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  lcd.clear();
  lcd.print("WiFi connected");

  if (!accel.begin()) {
    Serial.println("ADXL345 not found");
    lcd.clear();
    lcd.print("ADXL345 error");
    while (true) {
      delay(1000);
    }
  }
  accel.setRange(ADXL345_RANGE_16_G);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    sensors_event_t event;
    accel.getEvent(&event);
    float ax = event.acceleration.x;
    float ay = event.acceleration.y;
    float az = event.acceleration.z;

    Serial.printf("Accel: %.2f %.2f %.2f\n", ax, ay, az);

    if (abs(ax) > 18 || abs(ay) > 18 || abs(az) > 18) {
      lcd.clear();
      lcd.print("Accident Detected");
      if (sendAccidentReport()) {
        lcd.setCursor(0, 1);
        lcd.print("Report sent");
      } else {
        lcd.setCursor(0, 1);
        lcd.print("Send failed");
      }
      delay(30000);
    }
  } else {
    lcd.clear();
    lcd.print("WiFi reconnecting");
  }
  delay(1000);
}

bool sendAccidentReport() {
  float latitude = 0;
  float longitude = 0;
  if (!fetchLocation(latitude, longitude)) {
    Serial.println("Location fetch failed");
  }

  StaticJsonDocument<512> doc;
  doc["vehicle_type"] = "Bus";
  doc["vehicle_number"] = "XYZ-9876";
  doc["latitude"] = String(latitude, 6);
  doc["longitude"] = String(longitude, 6);
  doc["status"] = "Severe collision";
  doc["description"] = "Wi-Fi geolocation accident alert";

  String payload;
  serializeJson(doc, payload);

  HTTPClient http;
  String url = String("http://") + dashboardHost + ":" + dashboardPort + "/api/submit";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.printf("POST failed: %d\n", httpResponseCode);
  }

  http.end();
  return httpResponseCode == 200 || httpResponseCode == 201;
}

bool fetchLocation(float &latitude, float &longitude) {
  int n = WiFi.scanNetworks();
  StaticJsonDocument<1024> requestDoc;
  JsonArray wifiArray = requestDoc.createNestedArray("wifiAccessPoints");
  for (int i = 0; i < n && i < 8; i++) {
    JsonObject ap = wifiArray.createNestedObject();
    ap["macAddress"] = WiFi.BSSIDstr(i);
    ap["signalStrength"] = WiFi.RSSI(i);
  }

  String jsonPayload;
  serializeJson(requestDoc, jsonPayload);

  HTTPClient http;
  String url = String("https://www.googleapis.com/geolocation/v1/geolocate?key=") + geolocationKey;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(jsonPayload);
  if (code != HTTP_CODE_OK) {
    Serial.printf("Geolocation request failed: %d\n", code);
    http.end();
    return false;
  }

  String body = http.getString();
  http.end();

  StaticJsonDocument<512> resultDoc;
  DeserializationError error = deserializeJson(resultDoc, body);
  if (error) {
    Serial.println("Geolocation parse failed");
    return false;
  }

  latitude = resultDoc["location"]["lat"].as<float>();
  longitude = resultDoc["location"]["lng"].as<float>();
  Serial.printf("Location: %.6f, %.6f\n", latitude, longitude);
  return true;
}
