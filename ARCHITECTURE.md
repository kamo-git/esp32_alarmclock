# System Architecture / システム構成

## Block Diagram / ブロック図

```
┌─────────────────────────────────────────────────────────────┐
│                         ESP32 Board                          │
│                                                               │
│  ┌──────────────┐        ┌──────────────┐                   │
│  │   WiFi       │        │     NTP      │                   │
│  │   Module     │◄──────►│   Client     │                   │
│  └──────────────┘        └──────────────┘                   │
│         │                        │                           │
│         │                        ▼                           │
│         │                ┌──────────────┐                   │
│         │                │  Time Sync   │                   │
│         │                └──────────────┘                   │
│         │                        │                           │
│         ▼                        ▼                           │
│  ┌──────────────┐        ┌──────────────┐                   │
│  │ Web Server   │        │    Alarm     │                   │
│  │  (Port 80)   │        │   Manager    │                   │
│  └──────────────┘        └──────────────┘                   │
│         │                        │                           │
│         │                        │                           │
│  GPIO22 ├────────────┐          │                           │
│  GPIO21 ├──────┐     │          │                           │
│  GPIO25 ├──┐   │     │          │                           │
│         │  │   │     │          │                           │
└─────────┼──┼───┼─────┼──────────┼───────────────────────────┘
          │  │   │     │          │
          │  │   │     │          ▼
          │  │   │     │    ┌──────────────┐
          │  │   │     │    │  Preferences │
          │  │   │     │    │   Storage    │
          │  │   │     │    └──────────────┘
          │  │   │     │
          │  │   │     └──►┌──────────────┐
          │  │   │         │   TM1637     │
          │  │   └────────►│   Display    │
          │  │             │   (4-digit)  │
          │  │             └──────────────┘
          │  │
          │  └─────────────►┌──────────────┐
          │                 │   Passive    │
          └────────────────►│   Buzzer     │
                            └──────────────┘
```

## Component Interaction / コンポーネント間の連携

### 1. WiFi & NTP Time Synchronization
- ESP32 connects to WiFi network
- NTP client fetches current time from internet time servers
- Time automatically syncs every 60 seconds
- Timezone offset applied (default: JST +9)

### 2. Web Server
- Hosts responsive web interface on port 80
- Accessible from any device on the same WiFi network
- Provides REST API endpoints:
  - `GET /` - Main web interface
  - `GET /time` - Current time (JSON)
  - `GET /alarms` - List of alarms (JSON)
  - `POST /setalarm` - Add new alarm
  - `POST /deletealarm` - Delete alarm

### 3. Alarm Manager
- Stores up to 5 alarms in memory
- Persists alarms to ESP32 flash memory (Preferences)
- Checks alarms every second
- Triggers buzzer when alarm matches current time

### 4. Display Controller (TM1637)
- Displays current time in HH:MM format
- Updates every second
- Communicates via 2-wire interface (CLK, DIO)
- Adjustable brightness

### 5. Sound Output (Buzzer)
- Passive buzzer for playing melodies
- PWM signal generates different frequencies
- Two built-in melodies
- Plays alarm sound for 60 seconds

## Data Flow / データフロー

### Setting an Alarm
```
User Device          Web Server        Alarm Manager       Preferences
    │                    │                    │                  │
    ├─POST /setalarm────►│                    │                  │
    │                    ├─Parse JSON────────►│                  │
    │                    │                    ├─Validate         │
    │                    │                    ├─Add to array     │
    │                    │                    ├─Save────────────►│
    │                    │                    │                  ├─Write Flash
    │                    │◄───Success─────────┤                  │
    │◄────200 OK─────────┤                    │                  │
```

### Alarm Trigger Flow
```
NTP Client      Alarm Manager      Buzzer         Display
    │                │               │              │
    ├─Update Time───►│               │              │
    │                ├─Check alarms  │              │
    │                ├─Match found!  │              │
    │                ├─Play Melody──►│              │
    │                │               ├─Beep! Beep!  │
    │                │               │              │
    │                └─Update────────┴──────────────►│
    │                                                ├─Show Time
```

## Memory Usage / メモリ使用量

### Flash Memory
- Program code: ~300 KB
- Alarm storage: < 1 KB
- Total: < 400 KB (ESP32 has 4 MB flash)

### RAM
- WiFi/Network: ~50 KB
- Web Server: ~20 KB
- Display buffer: < 1 KB
- Alarm array: < 1 KB
- Total: < 100 KB (ESP32 has 520 KB RAM)

## Power Consumption / 消費電力

Typical operating conditions:
- ESP32 (WiFi active): ~160 mA
- TM1637 Display: ~20 mA
- Buzzer (idle): 0 mA
- Buzzer (active): ~30 mA
- **Total (normal): ~180 mA @ 3.3V ≈ 0.6W**
- **Total (alarm): ~210 mA @ 3.3V ≈ 0.7W**

Power supply recommendations:
- USB power: 5V, 500 mA minimum
- Battery: 3.7V LiPo, 1000+ mAh recommended
- Expected battery life: 5-6 hours (with 1000 mAh battery)

## Network Architecture / ネットワーク構成

```
Internet                WiFi Router              ESP32
   │                         │                     │
   │◄─NTP Request───────────┼────────────────────►│
   │  (time.google.com)      │                     │
   │                         │                     │
   │                    ┌────┴────┐                │
   │                    │ DHCP    │                │
   │                    │ Server  │                │
   │                    └────┬────┘                │
   │                         ├─Assign IP──────────►│
   │                         │                     │
   │                         │                     │
User Device                  │                     │
   │                         │                     │
   ├──HTTP Request──────────┼────────────────────►│
   │  (192.168.x.x:80)       │                     │
   │◄──HTML Response─────────┼─────────────────────┤
   │                         │                     │
```

### Network Ports
- **80**: HTTP Web Server (incoming)
- **123**: NTP Client (outgoing, UDP)

### IP Address Assignment
- ESP32 uses DHCP to get IP address automatically
- IP address displayed in serial monitor on startup
- Static IP can be configured if needed

## File System / ファイルシステム

```
esp32_alarmclock/
├── platformio.ini          # PlatformIO configuration
├── src/
│   └── main.cpp           # Main program (ESP32 firmware)
├── data/                  # (unused, for future SPIFFS data)
├── README.md              # Main documentation
├── WIRING.md              # Wiring instructions
├── CONFIGURATION.md       # Configuration guide
├── EXAMPLES.md            # Usage examples
└── .gitignore            # Git ignore file
```

## Security Considerations / セキュリティ考慮事項

⚠️ **Important Security Notes:**

1. **WiFi Credentials**: Hardcoded in source code
   - ✓ Suitable for personal/home use
   - ✗ Not recommended for public/shared devices
   - Consider using WiFiManager library for production

2. **Web Interface**: No authentication
   - ✓ Simple to use
   - ✗ Anyone on WiFi can modify alarms
   - Consider adding basic authentication for shared networks

3. **Data Storage**: Preferences stored in plaintext
   - Alarm data is not encrypted
   - Not critical for alarm clock application

4. **Network Security**: HTTP (not HTTPS)
   - ✓ Adequate for local network
   - ✗ Not secure over internet
   - Keep device on private WiFi network

### Recommendations for Enhanced Security

1. **WiFi Manager**: Use WiFiManager library for credential storage
2. **Web Authentication**: Add HTTP basic authentication
3. **HTTPS**: Use AsyncWebServer with SSL for encrypted connection
4. **Firewall**: Configure router to isolate IoT devices
5. **Regular Updates**: Keep PlatformIO and libraries updated
