# Quick Start Examples / クイックスタート例

## Example 1: Basic Home Setup / 基本的な家庭用セットアップ

```cpp
// WiFi Configuration
const char* ssid = "MyHomeWiFi";
const char* password = "mypassword123";

// Timezone: Japan (UTC+9)
NTPClient timeClient(ntpUDP, "pool.ntp.org", 32400, 60000);

// Default pins
#define CLK_PIN 22
#define DIO_PIN 21
#define BUZZER_PIN 25
```

**Usage:**
- Access web interface at `http://192.168.1.xxx`
- Set alarm for 7:00 AM to wake up
- Choose Melody 2 for a pleasant wake-up sound

---

## Example 2: Office/Laboratory Setup / オフィス・研究室用セットアップ

```cpp
// WiFi Configuration (different timezone)
const char* ssid = "OfficeWiFi";
const char* password = "secureofficepass";

// Timezone: Singapore (UTC+8)
NTPClient timeClient(ntpUDP, "time.google.com", 28800, 60000);

// Using different pins (if GPIO22/21 are in use)
#define CLK_PIN 18
#define DIO_PIN 19
#define BUZZER_PIN 26

// Dimmer display for office environment
display.setBrightness(0x07); // Medium brightness
```

**Usage:**
- Set reminder alarms for meetings
- Set break time reminders
- Use Melody 1 for less intrusive notifications

---

## Example 3: Multiple Alarms Setup / 複数アラーム設定例

```cpp
// Support for more alarms
Alarm alarms[10]; // Increased from 5 to 10

// Longer alarm duration
const unsigned long ALARM_DURATION = 120000; // 2 minutes

// Custom NTP server for better reliability
NTPClient timeClient(ntpUDP, "ntp.nict.jp", 32400, 60000);
```

**Usage:**
- Set alarms every 2 hours for medication reminders
- Set multiple alarms for different daily tasks
- Extended alarm duration ensures you don't miss it

---

## Example 4: Portable/Battery Operation / ポータブル・バッテリー動作

```cpp
// Mobile hotspot WiFi
const char* ssid = "MyPhone-Hotspot";
const char* password = "12345678";

// Reduce display brightness to save power
display.setBrightness(0x03); // Low brightness

// Shorter alarm duration to save power
const unsigned long ALARM_DURATION = 30000; // 30 seconds
```

**Usage:**
- Use with power bank
- Connect to smartphone hotspot
- Lower power consumption for longer battery life

---

## Web Interface Usage Examples / Web インターフェース使用例

### Setting Morning Alarm / 朝のアラーム設定

1. Open web browser on your phone
2. Navigate to `http://[ESP32-IP-ADDRESS]`
3. In "Set New Alarm" section:
   - Hour: `7`
   - Minute: `0`
   - Melody: `Melody 2 (Scale)`
4. Click "Add Alarm"

### Setting Multiple Daily Reminders / 複数の日次リマインダー設定

**Breakfast (7:00)**
- Hour: 7, Minute: 0, Melody: 1

**Morning Medicine (8:00)**
- Hour: 8, Minute: 0, Melody: 2

**Lunch Break (12:00)**
- Hour: 12, Minute: 0, Melody: 1

**Afternoon Break (15:00)**
- Hour: 15, Minute: 0, Melody: 2

**Evening Medicine (20:00)**
- Hour: 20, Minute: 0, Melody: 1

---

## Advanced Melody Examples / 高度なメロディー例

### Adding Custom Melody: "Twinkle Twinkle Little Star"

Add to `src/main.cpp`:

```cpp
// Note definitions (add missing notes)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

// Twinkle Twinkle melody
int melody3[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
  0
};

int melody3Duration[] = {
  300, 300, 300, 300, 300, 300, 600,
  300, 300, 300, 300, 300, 300, 600,
  0
};
```

Then update the `playMelody()` function:

```cpp
void playMelody(int melodyNumber) {
  int* notes;
  int* durations;
  
  if (melodyNumber == 1) {
    notes = melody1;
    durations = melody1Duration;
  } else if (melodyNumber == 2) {
    notes = melody2;
    durations = melody2Duration;
  } else if (melodyNumber == 3) {
    notes = melody3;
    durations = melody3Duration;
  } else {
    notes = melody1; // default
    durations = melody1Duration;
  }
  
  for (int i = 0; notes[i] != 0; i++) {
    tone(BUZZER_PIN, notes[i], durations[i]);
    delay(durations[i] * 1.3);
    noTone(BUZZER_PIN);
  }
}
```

And update the HTML to add Melody 3 option:

```html
<select id="melody">
    <option value="1">Melody 1 (Simple)</option>
    <option value="2">Melody 2 (Scale)</option>
    <option value="3">Melody 3 (Twinkle)</option>
</select>
```

---

## Testing Examples / テスト例

### Test 1: Display Test

After uploading, the display should:
1. Show "----" during WiFi connection
2. Show current time in HH:MM format after connection
3. Update every second

### Test 2: Alarm Test

1. Set an alarm for 1 minute in the future
2. Wait for the alarm to trigger
3. Buzzer should play the selected melody
4. Alarm continues for 60 seconds (or your configured duration)

### Test 3: Web Interface Test

1. Access web interface from multiple devices (phone, tablet, PC)
2. All should show the same current time
3. Adding alarm on one device should appear on all devices
4. Deleting alarm should remove it from all devices

---

## Common Modifications / よくあるカスタマイズ

### Change Display Format to 12-Hour

In `updateDisplay()` function:

```cpp
void updateDisplay() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  // Convert to 12-hour format
  int displayHour = currentHour;
  if (displayHour == 0) displayHour = 12;
  if (displayHour > 12) displayHour -= 12;
  
  int displayValue = displayHour * 100 + currentMinute;
  display.showNumberDecEx(displayValue, 0b01000000, true);
}
```

### Add AM/PM Indicator

Use the decimal point segments on the display to indicate AM/PM.

### Add Snooze Function

Add a button and modify the alarm logic to support snooze (e.g., 5-minute delay).

---

## Debugging Examples / デバッグ例

### Enable Verbose Logging

Add more Serial.print statements:

```cpp
void checkAlarms() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  Serial.print("Current time: ");
  Serial.print(currentHour);
  Serial.print(":");
  Serial.println(currentMinute);
  
  for (int i = 0; i < alarmCount; i++) {
    Serial.print("Alarm ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(alarms[i].hour);
    Serial.print(":");
    Serial.print(alarms[i].minute);
    Serial.print(" Enabled: ");
    Serial.println(alarms[i].enabled);
  }
}
```

### Test Alarm Without Waiting

Temporarily modify the alarm check to trigger based on seconds instead of minutes:

```cpp
// For testing only - triggers every minute
if (alarms[i].enabled && 
    alarms[i].hour == currentHour && 
    alarms[i].minute == currentMinute) {
```

Changes to:

```cpp
// Testing mode - triggers when seconds match
if (alarms[i].enabled && 
    currentSecond == 30) { // Triggers at 30 seconds past every minute
```
