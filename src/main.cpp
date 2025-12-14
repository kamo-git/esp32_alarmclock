#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TM1637Display.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// Pin definitions
#define CLK_PIN 22  // TM1637 CLK pin
#define DIO_PIN 21  // TM1637 DIO pin
#define BUZZER_PIN 25  // Buzzer pin

// WiFi credentials (change these for your network)
// SECURITY NOTE: For production use, consider using WiFiManager library
// or storing credentials in a separate configuration file not committed to version control
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

// NTP Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 32400, 60000); // JST (UTC+9)

// TM1637 Display
TM1637Display display(CLK_PIN, DIO_PIN);

// Web Server
WebServer server(80);

// Preferences for storing alarm settings
Preferences preferences;

// Alarm structure
struct Alarm {
  bool enabled;
  int hour;
  int minute;
  String melody;
};

Alarm alarms[5]; // Support up to 5 alarms
int alarmCount = 0;

// Melody notes (frequencies in Hz)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

// Melody definitions
int melody1[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, 0};
int melody1Duration[] = {200, 200, 200, 400, 0};

int melody2[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, 0};
int melody2Duration[] = {150, 150, 150, 150, 150, 150, 150, 300, 0};

bool alarmTriggered = false;
unsigned long alarmStartTime = 0;
int triggeredAlarmIndex = -1; // Track which alarm was triggered
const unsigned long ALARM_DURATION = 60000; // Alarm duration 60 seconds
const unsigned long MELODY_REPEAT_INTERVAL = 3000; // Replay melody every 3 seconds
const unsigned long TIMING_TOLERANCE = 100; // Timing tolerance in milliseconds

void setupWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed");
  }
}

void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Alarm Clock</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 600px;
            margin: 0 auto;
            padding: 20px;
            background-color: #f0f0f0;
        }
        h1 {
            color: #333;
            text-align: center;
        }
        .alarm-container {
            background: white;
            border-radius: 10px;
            padding: 20px;
            margin: 20px 0;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        .alarm-form {
            display: flex;
            flex-direction: column;
            gap: 10px;
        }
        input, select, button {
            padding: 10px;
            font-size: 16px;
            border: 1px solid #ddd;
            border-radius: 5px;
        }
        button {
            background-color: #4CAF50;
            color: white;
            border: none;
            cursor: pointer;
            font-weight: bold;
        }
        button:hover {
            background-color: #45a049;
        }
        .delete-btn {
            background-color: #f44336;
        }
        .delete-btn:hover {
            background-color: #da190b;
        }
        .alarm-list {
            margin-top: 20px;
        }
        .alarm-item {
            background: #f9f9f9;
            padding: 15px;
            margin: 10px 0;
            border-radius: 5px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .time-input {
            display: flex;
            gap: 10px;
        }
        .current-time {
            text-align: center;
            font-size: 24px;
            font-weight: bold;
            color: #4CAF50;
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <h1>🕐 ESP32 Alarm Clock</h1>
    <div class="current-time" id="currentTime">--:--</div>
    
    <div class="alarm-container">
        <h2>Set New Alarm</h2>
        <form class="alarm-form" onsubmit="setAlarm(event)">
            <div class="time-input">
                <input type="number" id="hour" min="0" max="23" placeholder="Hour (0-23)" required>
                <input type="number" id="minute" min="0" max="59" placeholder="Minute (0-59)" required>
            </div>
            <select id="melody">
                <option value="1">Melody 1 (Simple)</option>
                <option value="2">Melody 2 (Scale)</option>
            </select>
            <button type="submit">Add Alarm</button>
        </form>
    </div>
    
    <div class="alarm-container">
        <h2>Active Alarms</h2>
        <div class="alarm-list" id="alarmList">
            Loading...
        </div>
    </div>
    
    <script>
        function updateCurrentTime() {
            fetch('/time')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('currentTime').textContent = data.time;
                })
                .catch(error => console.error('Error:', error));
        }
        
        function loadAlarms() {
            fetch('/alarms')
                .then(response => response.json())
                .then(data => {
                    const alarmList = document.getElementById('alarmList');
                    if (data.alarms.length === 0) {
                        alarmList.innerHTML = '<p>No alarms set</p>';
                    } else {
                        alarmList.innerHTML = data.alarms.map((alarm, index) => `
                            <div class="alarm-item">
                                <span>
                                    ${String(alarm.hour).padStart(2, '0')}:${String(alarm.minute).padStart(2, '0')} 
                                    - Melody ${alarm.melody}
                                    ${alarm.enabled ? '✓' : '✗'}
                                </span>
                                <button class="delete-btn" onclick="deleteAlarm(${index})">Delete</button>
                            </div>
                        `).join('');
                    }
                })
                .catch(error => console.error('Error:', error));
        }
        
        function setAlarm(event) {
            event.preventDefault();
            const hour = document.getElementById('hour').value;
            const minute = document.getElementById('minute').value;
            const melody = document.getElementById('melody').value;
            
            fetch('/setalarm', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({hour: parseInt(hour), minute: parseInt(minute), melody: melody})
            })
            .then(response => response.json())
            .then(data => {
                alert(data.message);
                document.getElementById('hour').value = '';
                document.getElementById('minute').value = '';
                loadAlarms();
            })
            .catch(error => {
                alert('Error setting alarm');
                console.error('Error:', error);
            });
        }
        
        function deleteAlarm(index) {
            fetch('/deletealarm', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({index: index})
            })
            .then(response => response.json())
            .then(data => {
                alert(data.message);
                loadAlarms();
            })
            .catch(error => {
                alert('Error deleting alarm');
                console.error('Error:', error);
            });
        }
        
        // Update time every second
        updateCurrentTime();
        setInterval(updateCurrentTime, 1000);
        
        // Load alarms on page load
        loadAlarms();
    </script>
</body>
</html>
)";
  server.send(200, "text/html", html);
}

void handleGetTime() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  String timeStr = String(currentHour) + ":" + (currentMinute < 10 ? "0" : "") + String(currentMinute);
  
  StaticJsonDocument<200> doc;
  doc["time"] = timeStr;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleGetAlarms() {
  StaticJsonDocument<1024> doc;
  JsonArray alarmsArray = doc.createNestedArray("alarms");
  
  for (int i = 0; i < alarmCount; i++) {
    JsonObject alarmObj = alarmsArray.createNestedObject();
    alarmObj["enabled"] = alarms[i].enabled;
    alarmObj["hour"] = alarms[i].hour;
    alarmObj["minute"] = alarms[i].minute;
    alarmObj["melody"] = alarms[i].melody;
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSetAlarm() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    if (error) {
      server.send(400, "application/json", "{\"message\":\"Invalid JSON\"}");
      return;
    }
    
    if (alarmCount >= 5) {
      server.send(400, "application/json", "{\"message\":\"Maximum 5 alarms\"}");
      return;
    }
    
    alarms[alarmCount].enabled = true;
    alarms[alarmCount].hour = doc["hour"];
    alarms[alarmCount].minute = doc["minute"];
    alarms[alarmCount].melody = doc["melody"].as<String>();
    alarmCount++;
    
    saveAlarms();
    
    server.send(200, "application/json", "{\"message\":\"Alarm set successfully\"}");
  } else {
    server.send(400, "application/json", "{\"message\":\"No data received\"}");
  }
}

void handleDeleteAlarm() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    if (error) {
      server.send(400, "application/json", "{\"message\":\"Invalid JSON\"}");
      return;
    }
    
    int index = doc["index"];
    
    if (index < 0 || index >= alarmCount) {
      server.send(400, "application/json", "{\"message\":\"Invalid alarm index\"}");
      return;
    }
    
    // Shift alarms down
    for (int i = index; i < alarmCount - 1; i++) {
      alarms[i] = alarms[i + 1];
    }
    alarmCount--;
    
    saveAlarms();
    
    server.send(200, "application/json", "{\"message\":\"Alarm deleted\"}");
  } else {
    server.send(400, "application/json", "{\"message\":\"No data received\"}");
  }
}

void saveAlarms() {
  preferences.begin("alarms", false);
  preferences.putInt("count", alarmCount);
  
  for (int i = 0; i < alarmCount; i++) {
    String key = "alarm" + String(i);
    String value = String(alarms[i].enabled) + "," + 
                   String(alarms[i].hour) + "," + 
                   String(alarms[i].minute) + "," + 
                   alarms[i].melody;
    preferences.putString(key.c_str(), value);
  }
  
  preferences.end();
}

void loadAlarms() {
  preferences.begin("alarms", true);
  alarmCount = preferences.getInt("count", 0);
  
  for (int i = 0; i < alarmCount; i++) {
    String key = "alarm" + String(i);
    String value = preferences.getString(key.c_str(), "");
    
    if (value.length() > 0) {
      int firstComma = value.indexOf(',');
      int secondComma = value.indexOf(',', firstComma + 1);
      int thirdComma = value.indexOf(',', secondComma + 1);
      
      alarms[i].enabled = value.substring(0, firstComma).toInt();
      alarms[i].hour = value.substring(firstComma + 1, secondComma).toInt();
      alarms[i].minute = value.substring(secondComma + 1, thirdComma).toInt();
      alarms[i].melody = value.substring(thirdComma + 1);
    }
  }
  
  preferences.end();
}

void playMelody(int melodyNumber) {
  int* notes;
  int* durations;
  
  if (melodyNumber == 1) {
    notes = melody1;
    durations = melody1Duration;
  } else {
    notes = melody2;
    durations = melody2Duration;
  }
  
  for (int i = 0; notes[i] != 0; i++) {
    tone(BUZZER_PIN, notes[i], durations[i]);
    delay(durations[i] * 1.3);
    noTone(BUZZER_PIN);
  }
}

void checkAlarms() {
  if (alarmTriggered) {
    // Check if alarm duration has passed
    if (millis() - alarmStartTime > ALARM_DURATION) {
      alarmTriggered = false;
      triggeredAlarmIndex = -1;
      noTone(BUZZER_PIN);
    }
    return;
  }
  
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  
  // Only check at the start of the minute
  if (currentSecond != 0) {
    return;
  }
  
  for (int i = 0; i < alarmCount; i++) {
    if (alarms[i].enabled && 
        alarms[i].hour == currentHour && 
        alarms[i].minute == currentMinute) {
      Serial.println("Alarm triggered!");
      alarmTriggered = true;
      alarmStartTime = millis();
      triggeredAlarmIndex = i; // Remember which alarm triggered
      
      // Play melody
      int melodyNum = alarms[i].melody.toInt();
      playMelody(melodyNum);
      break;
    }
  }
}

void updateDisplay() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  // Display time in HH:MM format
  int displayValue = currentHour * 100 + currentMinute;
  display.showNumberDecEx(displayValue, 0b01000000, true); // Show with colon
}

void setup() {
  Serial.begin(115200);
  
  // Initialize buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize display
  display.setBrightness(0x0f);
  display.clear();
  
  // Display "----" while connecting
  uint8_t segto[] = { 0x40, 0x40, 0x40, 0x40 };
  display.setSegments(segto);
  
  // Connect to WiFi
  setupWiFi();
  
  // Initialize NTP Client
  timeClient.begin();
  timeClient.update();
  
  // Load saved alarms
  loadAlarms();
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/time", handleGetTime);
  server.on("/alarms", handleGetAlarms);
  server.on("/setalarm", HTTP_POST, handleSetAlarm);
  server.on("/deletealarm", HTTP_POST, handleDeleteAlarm);
  
  server.begin();
  Serial.println("HTTP server started");
  
  // Display initial time
  updateDisplay();
}

void loop() {
  server.handleClient();
  
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    updateDisplay();
    checkAlarms();
    lastUpdate = millis();
  }
  
  // Continue playing melody if alarm is triggered
  if (alarmTriggered && triggeredAlarmIndex >= 0 && 
      (millis() - alarmStartTime) % MELODY_REPEAT_INTERVAL < TIMING_TOLERANCE) {
    // Replay melody every 3 seconds during alarm
    int melodyNum = alarms[triggeredAlarmIndex].melody.toInt();
    playMelody(melodyNum);
  }
}
