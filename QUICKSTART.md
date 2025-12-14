# Quick Start Guide / クイックスタートガイド

Get your ESP32 Alarm Clock running in 5 steps!

## 🚀 Quick Setup (5 Steps)

### Step 1: Gather Components (5 minutes)

You need:
- ✅ ESP32 development board
- ✅ TM1637 4-digit 7-segment display
- ✅ **Passive buzzer** (important: not active buzzer!)
- ✅ Breadboard + jumper wires
- ✅ USB cable

### Step 2: Wire Everything (10 minutes)

```
ESP32 GPIO22 → TM1637 CLK
ESP32 GPIO21 → TM1637 DIO
ESP32 3.3V   → TM1637 VCC
ESP32 GND    → TM1637 GND + Buzzer -

ESP32 GPIO25 → Buzzer +
```

**Visual Check:**
- 5 wires total
- No loose connections
- Buzzer polarity correct

### Step 3: Install Software (10 minutes)

#### Option A: Using VS Code (Recommended)
1. Install [VS Code](https://code.visualstudio.com/)
2. Install "PlatformIO IDE" extension
3. Clone this repository
4. Open folder in VS Code

#### Option B: Using Command Line
```bash
pip install platformio
git clone https://github.com/kamo-git/esp32_alarmclock.git
cd esp32_alarmclock
```

### Step 4: Configure WiFi (2 minutes)

Edit `src/main.cpp`, change these lines:

```cpp
const char* ssid = "YourWiFiName";      // ← Your WiFi name
const char* password = "YourPassword";   // ← Your WiFi password
```

**Save the file!**

### Step 5: Upload to ESP32 (5 minutes)

#### VS Code:
1. Click PlatformIO icon (alien head) in sidebar
2. Click "Upload" under PROJECT TASKS

#### Command Line:
```bash
pio run --target upload
```

**Watch for success message!**

---

## ✅ Verify It Works

### 1. Check Serial Monitor
```
WiFi connected
IP address: 192.168.1.100  ← Note this!
HTTP server started
```

### 2. Check Display
- Should show current time: `12:34`
- Updates every second

### 3. Access Web Interface
- Open browser on phone
- Go to: `http://192.168.1.100` (use your IP)
- You should see the alarm clock interface!

---

## 🎵 Set Your First Alarm

1. Open web interface
2. Enter time (24-hour format):
   - Hour: `7` (for 7 AM)
   - Minute: `30`
3. Choose Melody 1 or 2
4. Click "Add Alarm"
5. Done! 🎉

**Test it:** Set alarm for 1 minute in future!

---

## 📱 Use From Your Phone

1. Connect phone to same WiFi
2. Open browser
3. Type ESP32 IP address
4. Bookmark for easy access!

**Pro tip:** Add to home screen:
- **iPhone:** Share → Add to Home Screen
- **Android:** Menu → Add to Home Screen

---

## 🔧 Common Issues (Quick Fix)

### Display shows "----"
- ✅ WiFi is connecting, wait 10 seconds

### Display blank
- ❌ Check wiring, especially VCC and GND

### No sound from buzzer
- ❌ Wrong buzzer type! Use **passive** buzzer
- ❌ Check GPIO25 connection

### Can't access web page
- ❌ Check IP address in serial monitor
- ❌ Ensure phone on same WiFi network

### Wrong time displayed
- ✅ Wait 1-2 minutes for NTP sync
- ❌ Check timezone in code (default: JST +9)

---

## 🎯 What You Can Do Now

✨ **Set multiple alarms** (up to 5)
✨ **Choose different melodies**
✨ **Control from any WiFi device**
✨ **Alarms persist after power off**

---

## 📚 Learn More

- **Full Documentation:** See [README.md](README.md)
- **Wiring Details:** See [WIRING.md](WIRING.md)
- **Configuration:** See [CONFIGURATION.md](CONFIGURATION.md)
- **Examples:** See [EXAMPLES.md](EXAMPLES.md)
- **Testing:** See [TESTING.md](TESTING.md)

---

## 🆘 Need Help?

### Check Serial Monitor First
```bash
pio device monitor
```
Look for error messages!

### Common Messages

| Message | Meaning | Action |
|---------|---------|--------|
| "WiFi connected" | ✅ Good! | Note the IP address |
| "WiFi connection failed" | ❌ Problem | Check SSID/password |
| "HTTP server started" | ✅ Good! | Web interface ready |
| Garbled text | ❌ Problem | Check baud rate (115200) |

---

## 🎨 Customization Ideas

Once basic setup works, try:

### Change Timezone
```cpp
// For UTC+8 (Singapore): use 28800
// For UTC+0 (London): use 0
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800, 60000);
```

### Adjust Brightness
```cpp
display.setBrightness(0x07); // 0x00 (dim) to 0x0f (bright)
```

### Longer Alarm
```cpp
const unsigned long ALARM_DURATION = 120000; // 2 minutes
```

---

## ⏱️ Time Estimate

| Task | Time |
|------|------|
| Gather components | 5 min |
| Wiring | 10 min |
| Software install | 10 min |
| Configuration | 2 min |
| Upload | 5 min |
| Testing | 5 min |
| **Total** | **~35 minutes** |

---

## ✅ Success Checklist

- [ ] All components gathered
- [ ] Wiring completed and verified
- [ ] PlatformIO installed
- [ ] WiFi credentials configured
- [ ] Code uploaded successfully
- [ ] Serial monitor shows IP address
- [ ] Display shows correct time
- [ ] Web interface accessible
- [ ] Test alarm works

**All checked?** Congratulations! 🎉 Your ESP32 Alarm Clock is ready!

---

## 🔗 Useful Links

- **PlatformIO:** https://platformio.org/
- **ESP32 Docs:** https://docs.espressif.com/
- **TM1637 Library:** https://github.com/avishorp/TM1637
- **Issue Tracker:** https://github.com/kamo-git/esp32_alarmclock/issues

---

## 📸 Share Your Build!

Built your ESP32 Alarm Clock?
- Take a photo!
- Share on social media
- Tag: #ESP32AlarmClock
- Open a show & tell issue on GitHub!

---

## 🚀 Next Level

Ready for more?

1. **Add more melodies** - Customize your wake-up sound
2. **3D print enclosure** - Make it look professional
3. **Add buttons** - Snooze and quick controls
4. **Battery power** - Make it portable
5. **RTC module** - Keep time without WiFi

See [EXAMPLES.md](EXAMPLES.md) for detailed guides!

---

**Happy Building! 🎉**
