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
const char* ssid = "elecom-4e1ba6";
const char* password = "w6v42nc58d77";

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
  uint8_t days; // Bitmask: bit0=日, bit1=月, bit2=火, bit3=水, bit4=木, bit5=金, bit6=土
};

Alarm alarms[10]; // Support up to 10 alarms
int alarmCount = 0;

void saveAlarms();
void loadAlarms();

bool alarmTriggered = false;
unsigned long alarmStartTime = 0;
const unsigned long ALARM_DURATION = 60000; // Alarm duration 60 seconds

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
  String html = R"ESP32HTML(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 目覚まし時計</title>
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
        .days-select {
            display: flex;
            flex-wrap: wrap;
            gap: 5px;
        }
        .days-select label {
            display: flex;
            align-items: center;
            gap: 3px;
            padding: 5px 10px;
            background: #e0e0e0;
            border-radius: 5px;
            cursor: pointer;
        }
        .days-select input:checked + span {
            font-weight: bold;
            color: #4CAF50;
        }
        .days-display {
            font-size: 12px;
            color: #666;
        }
    </style>
</head>
<body>
    <h1>🕐 ESP32 目覚まし時計</h1>
    <div class="current-time" id="currentTime">--:--</div>
    
    <div class="alarm-container">
        <h2>アラーム設定</h2>
        <form class="alarm-form" onsubmit="setAlarm(event)">
            <div class="time-input">
                <input type="number" id="hour" min="0" max="23" placeholder="時 (0-23)" required>
                <input type="number" id="minute" min="0" max="59" placeholder="分 (0-59)" required>
            </div>
            <div class="days-select">
                <label><input type="checkbox" name="day" value="0"><span>日</span></label>
                <label><input type="checkbox" name="day" value="1" checked><span>月</span></label>
                <label><input type="checkbox" name="day" value="2" checked><span>火</span></label>
                <label><input type="checkbox" name="day" value="3" checked><span>水</span></label>
                <label><input type="checkbox" name="day" value="4" checked><span>木</span></label>
                <label><input type="checkbox" name="day" value="5" checked><span>金</span></label>
                <label><input type="checkbox" name="day" value="6"><span>土</span></label>
            </div>
            <button type="submit">アラーム追加</button>
        </form>
    </div>
    
    <div class="alarm-container">
        <h2>設定済みアラーム</h2>
        <div class="alarm-list" id="alarmList">
            読み込み中...
        </div>
    </div>
    
    <script>
        const dayNames = ['日', '月', '火', '水', '木', '金', '土'];
        
        function updateCurrentTime() {
            fetch('/time')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('currentTime').textContent = data.time + ' (' + dayNames[data.day] + ')';
                })
                .catch(error => console.error('Error:', error));
        }
        
        function getDaysString(days) {
            let result = [];
            for (let i = 0; i < 7; i++) {
                if (days & (1 << i)) {
                    result.push(dayNames[i]);
                }
            }
            return result.join(' ');
        }
        
        function loadAlarms() {
            fetch('/alarms')
                .then(response => response.json())
                .then(data => {
                    const alarmList = document.getElementById('alarmList');
                    if (data.alarms.length === 0) {
                        alarmList.innerHTML = '<p>アラームが設定されていません</p>';
                    } else {
                        alarmList.innerHTML = data.alarms.map((alarm, index) => {
                            return `<div class="alarm-item">
                                <div>
                                    <div>${String(alarm.hour).padStart(2, '0')}:${String(alarm.minute).padStart(2, '0')} 
                                    ${alarm.enabled ? '✓' : '✗'}</div>
                                    <div class="days-display">${getDaysString(alarm.days)}</div>
                                </div>
                                <button class="delete-btn" onclick="deleteAlarm(${index})">削除</button>
                            </div>`;
                        }).join('');
                    }
                })
                .catch(error => console.error('Error:', error));
        }
        
        function setAlarm(event) {
            event.preventDefault();
            const hour = document.getElementById('hour').value;
            const minute = document.getElementById('minute').value;
            
            let days = 0;
            document.querySelectorAll('input[name="day"]:checked').forEach(cb => {
                days |= (1 << parseInt(cb.value));
            });
            
            if (days === 0) {
                alert('曜日を1つ以上選択してください');
                return;
            }
            
            fetch('/setalarm', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({hour: parseInt(hour), minute: parseInt(minute), days: days})
            })
            .then(response => response.json())
            .then(data => {
                alert(data.message);
                document.getElementById('hour').value = '';
                document.getElementById('minute').value = '';
                loadAlarms();
            })
            .catch(error => {
                alert('アラーム設定エラー');
                console.error('Error:', error);
            });
        }
        
        function deleteAlarm(index) {
            if (!confirm('このアラームを削除しますか？')) return;
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
                alert('アラーム削除エラー');
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
)ESP32HTML";
  server.send(200, "text/html", html);
}

void handleGetTime() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentDay = timeClient.getDay(); // 0=Sunday, 1=Monday, ...
  
  String timeStr = String(currentHour) + ":" + (currentMinute < 10 ? "0" : "") + String(currentMinute);
  
  StaticJsonDocument<200> doc;
  doc["time"] = timeStr;
  doc["day"] = currentDay;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleGetAlarms() {
  StaticJsonDocument<2048> doc;
  JsonArray alarmsArray = doc.createNestedArray("alarms");
  
  for (int i = 0; i < alarmCount; i++) {
    JsonObject alarmObj = alarmsArray.createNestedObject();
    alarmObj["enabled"] = alarms[i].enabled;
    alarmObj["hour"] = alarms[i].hour;
    alarmObj["minute"] = alarms[i].minute;
    alarmObj["days"] = alarms[i].days;
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
      server.send(400, "application/json", "{\"message\":\"JSONエラー\"}");
      return;
    }
    
    if (alarmCount >= 10) {
      server.send(400, "application/json", "{\"message\":\"アラームは最大10個までです\"}");
      return;
    }
    
    alarms[alarmCount].enabled = true;
    alarms[alarmCount].hour = doc["hour"];
    alarms[alarmCount].minute = doc["minute"];
    alarms[alarmCount].days = doc["days"];
    alarmCount++;
    
    saveAlarms();
    
    server.send(200, "application/json", "{\"message\":\"アラームを設定しました\"}");
  } else {
    server.send(400, "application/json", "{\"message\":\"データがありません\"}");
  }
}

void handleDeleteAlarm() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    if (error) {
      server.send(400, "application/json", "{\"message\":\"JSONエラー\"}");
      return;
    }
    
    int index = doc["index"];
    
    if (index < 0 || index >= alarmCount) {
      server.send(400, "application/json", "{\"message\":\"無効なアラームです\"}");
      return;
    }
    
    // Shift alarms down
    for (int i = index; i < alarmCount - 1; i++) {
      alarms[i] = alarms[i + 1];
    }
    alarmCount--;
    
    saveAlarms();
    
    server.send(200, "application/json", "{\"message\":\"アラームを削除しました\"}");
  } else {
    server.send(400, "application/json", "{\"message\":\"データがありません\"}");
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
                   String(alarms[i].days);
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
      alarms[i].days = value.substring(thirdComma + 1).toInt();
    }
  }
  
  preferences.end();
}



void checkAlarms() {
  if (alarmTriggered) {
    // Check if alarm duration has passed
    if (millis() - alarmStartTime > ALARM_DURATION) {
      alarmTriggered = false;
      digitalWrite(BUZZER_PIN, LOW);
    }
    return;
  }
  
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  int currentDay = timeClient.getDay(); // 0=Sunday, 1=Monday, ...
  
  // Only check at the start of the minute
  if (currentSecond != 0) {
    return;
  }
  
  for (int i = 0; i < alarmCount; i++) {
    if (alarms[i].enabled && 
        alarms[i].hour == currentHour && 
        alarms[i].minute == currentMinute &&
        (alarms[i].days & (1 << currentDay))) {
      Serial.println("Alarm triggered!");
      alarmTriggered = true;
      alarmStartTime = millis();
      
      // Turn on melody IC
      digitalWrite(BUZZER_PIN, HIGH);
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
  
  // Display IP address host part for 5 seconds
  if (WiFi.status() == WL_CONNECTED) {
    IPAddress ip = WiFi.localIP();
    int hostPart = ip[3]; // Get the last octet (host part)
    display.showNumberDec(hostPart, false);
    delay(5000);
  }
  
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
}
