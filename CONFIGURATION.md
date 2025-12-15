# Configuration Guide / 設定ガイド

## WiFi Configuration / WiFi設定

Before uploading the code to your ESP32, you need to configure your WiFi credentials.

### Method 1: Edit main.cpp directly (recommended for beginners)

1. Open `src/main.cpp`
2. Find these lines (around line 17-18):

```cpp
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
```

3. Replace with your actual WiFi credentials:

```cpp
const char* ssid = "MyHomeWiFi";      // Your WiFi network name
const char* password = "mypassword123"; // Your WiFi password
```

4. Save the file and upload to ESP32

### Example Configurations / 設定例

#### Example 1: Home WiFi
```cpp
const char* ssid = "HOME-WIFI-5G";
const char* password = "SecurePassword2024";
```

#### Example 2: Mobile Hotspot
```cpp
const char* ssid = "iPhone-Hotspot";
const char* password = "12345678";
```

#### Example 3: Japanese WiFi
```cpp
const char* ssid = "バッファロー-A-1234";  // UTF-8 SSID supported
const char* password = "password123";
```

---

## Timezone Configuration / タイムゾーン設定

The default timezone is JST (Japan Standard Time, UTC+9).

### Change Timezone

In `src/main.cpp`, find this line (around line 24):

```cpp
NTPClient timeClient(ntpUDP, "pool.ntp.org", 32400, 60000);
```

The `32400` is the timezone offset in seconds (32400 = 9 hours * 3600 seconds).

### Common Timezones / よく使うタイムゾーン

| Timezone | Offset (seconds) | Example |
|----------|------------------|---------|
| UTC+0 (GMT) | 0 | London |
| UTC+1 (CET) | 3600 | Paris, Berlin |
| UTC+8 | 28800 | Singapore, Beijing |
| UTC+9 (JST) | 32400 | Tokyo, Seoul |
| UTC+10 | 36000 | Sydney |
| UTC-5 (EST) | -18000 | New York |
| UTC-8 (PST) | -28800 | Los Angeles |

### Example: Change to UTC+8 (Singapore)

```cpp
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800, 60000);
```

---

## Pin Configuration / ピン設定

You can customize the GPIO pins used for the display and buzzer.

### Default Pin Assignment

In `src/main.cpp` (around line 12-14):

```cpp
#define CLK_PIN 22    // TM1637 CLK pin
#define DIO_PIN 21    // TM1637 DIO pin
#define BUZZER_PIN 25 // Buzzer pin
```

### Custom Pin Assignment Example

If you want to use different pins:

```cpp
#define CLK_PIN 18    // TM1637 CLK pin
#define DIO_PIN 19    // TM1637 DIO pin
#define BUZZER_PIN 26 // Buzzer pin
```

**Note**: Make sure the pins you choose support the required functions:
- CLK and DIO: Any GPIO pin
- BUZZER: Any GPIO pin that supports PWM output

---

## Display Brightness / ディスプレイ輝度

Adjust the TM1637 display brightness in `src/main.cpp` (around line 342):

```cpp
display.setBrightness(0x0f);  // 0x00 (dim) to 0x0f (bright)
```

### Brightness Levels

- `0x00`: Minimum (very dim)
- `0x07`: Medium
- `0x0f`: Maximum (very bright) - **Default**

Example for medium brightness:
```cpp
display.setBrightness(0x07);
```

---

## Alarm Configuration / アラーム設定

### Maximum Number of Alarms

By default, the system supports up to 5 alarms. To change this, modify in `src/main.cpp` (around line 37):

```cpp
Alarm alarms[5]; // Change 5 to your desired number
```

Example for 10 alarms:
```cpp
Alarm alarms[10];
int alarmCount = 0;
```

Also update the check in `handleSetAlarm()` function:
```cpp
if (alarmCount >= 10) {  // Change to match your alarm array size
    server.send(400, "application/json", "{\"message\":\"Maximum 10 alarms\"}");
    return;
}
```

### Alarm Duration

The default alarm duration is 60 seconds. To change it, modify in `src/main.cpp` (around line 62):

```cpp
const unsigned long ALARM_DURATION = 60000; // in milliseconds
```

Example for 2 minutes (120 seconds):
```cpp
const unsigned long ALARM_DURATION = 120000; // 120 seconds
```

---

## NTP Server Configuration / NTPサーバー設定

The default NTP server is `pool.ntp.org`. You can change it to a local or preferred NTP server.

In `src/main.cpp` (around line 24):

```cpp
NTPClient timeClient(ntpUDP, "pool.ntp.org", 32400, 60000);
```

### Alternative NTP Servers

#### Japan
```cpp
NTPClient timeClient(ntpUDP, "ntp.nict.jp", 32400, 60000);
```

#### Google Public NTP
```cpp
NTPClient timeClient(ntpUDP, "time.google.com", 32400, 60000);
```

#### Cloudflare NTP
```cpp
NTPClient timeClient(ntpUDP, "time.cloudflare.com", 32400, 60000);
```

---

## Web Server Port / Webサーバーポート

The default web server port is 80. To change it, modify in `src/main.cpp` (around line 27):

```cpp
WebServer server(80);  // Default HTTP port
```

Example for port 8080:
```cpp
WebServer server(8080);
```

**Note**: If you change the port, you'll need to access the web interface at `http://[IP-ADDRESS]:8080`

---

## Troubleshooting Configuration Issues / 設定のトラブルシューティング

### WiFi Not Connecting

1. Check SSID and password are correct
2. Ensure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
3. Check serial monitor for error messages
4. Try increasing connection timeout (modify `attempts < 20` to higher value)

### Time Not Syncing

1. Verify internet connection is working
2. Try a different NTP server
3. Check timezone offset is correct
4. Ensure firewall allows NTP (UDP port 123)

### Display Not Working

1. Verify pin numbers in code match physical wiring
2. Check display brightness setting
3. Try swapping CLK and DIO pins (both in code and wiring)
4. Test with different GPIO pins

### Buzzer Not Playing

1. Confirm you're using a passive buzzer, not active
2. Check BUZZER_PIN matches your wiring
3. Try a different GPIO pin with PWM support
4. Increase volume by using a transistor amplifier circuit
