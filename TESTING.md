# Testing Guide / テストガイド

## Pre-Upload Testing / アップロード前のテスト

Before uploading to your ESP32, verify the following:

### 1. Hardware Checklist / ハードウェアチェックリスト

- [ ] ESP32 development board
- [ ] TM1637 4-digit 7-segment display
- [ ] Passive buzzer (not active buzzer)
- [ ] Breadboard and jumper wires
- [ ] USB cable for programming
- [ ] 3.3V or 5V power supply (USB is fine)

### 2. Wiring Verification / 配線確認

Using a multimeter (optional but recommended):

1. **Continuity Test**
   - Verify each wire connection
   - Check no short circuits between VCC and GND
   - Confirm GPIO pins are correctly connected

2. **Visual Inspection**
   - Check all wires are firmly inserted
   - Verify no loose connections
   - Ensure correct pin numbers

### 3. Software Configuration / ソフトウェア設定

- [ ] PlatformIO installed
- [ ] Project opens without errors
- [ ] WiFi SSID and password updated in `src/main.cpp`
- [ ] Timezone configured correctly
- [ ] Pin numbers match your wiring

## Post-Upload Testing / アップロード後のテスト

### Test 1: Serial Monitor Output

After uploading, open serial monitor (115200 baud):

**Expected Output:**
```
Connecting to WiFi...
.....
WiFi connected
IP address: 192.168.1.100
HTTP server started
```

**Troubleshooting:**
- ❌ "WiFi connection failed" → Check SSID/password
- ❌ No output → Check serial baud rate (115200)
- ❌ Garbled text → Wrong baud rate

### Test 2: Display Test

**Expected Behavior:**
1. Display shows "----" during WiFi connection
2. After connection, displays current time (HH:MM format)
3. Colon blinks every second
4. Time updates every second

**Troubleshooting:**
- ❌ Display blank → Check VCC and GND connections
- ❌ Wrong segments → Check CLK/DIO connections
- ❌ Dim display → Adjust brightness in code
- ❌ Wrong time → Check timezone offset

### Test 3: Web Interface Test

1. **Access Test**
   - Open browser on phone/PC
   - Navigate to the IP address shown in serial monitor
   - Expected: Web page loads with "ESP32 Alarm Clock" title

2. **Current Time Display**
   - Check if current time is displayed on web page
   - Verify it matches the TM1637 display
   - Confirm it updates every second

3. **Set Alarm Test**
   - Set alarm for 2 minutes in the future
   - Hour: Current hour
   - Minute: Current minute + 2
   - Melody: Select "Melody 1"
   - Click "Add Alarm"
   - Expected: Success message, alarm appears in "Active Alarms"

### Test 4: Alarm Trigger Test

**Setup:**
1. Set an alarm for 1 minute in the future
2. Wait for the alarm time
3. Observe behavior

**Expected Behavior:**
1. At alarm time (XX:XX:00), buzzer starts playing
2. Melody plays completely
3. Melody repeats every 3 seconds
4. Alarm continues for 60 seconds
5. After 60 seconds, buzzer stops

**Troubleshooting:**
- ❌ No sound → Check buzzer is passive, not active
- ❌ No sound → Verify buzzer polarity
- ❌ Continuous tone → Wrong buzzer type (active vs passive)
- ❌ Alarm doesn't trigger → Check time sync (serial monitor)

### Test 5: Multiple Alarms Test

1. Set 3 different alarms with different times
2. Verify all appear in "Active Alarms" list
3. Delete the middle alarm
4. Verify it's removed from the list
5. Refresh page, verify alarms persist

**Expected:**
- All alarms saved correctly
- Delete function works
- Alarms persist after page refresh

### Test 6: Persistence Test

1. Set 2-3 alarms
2. Note down the alarm times
3. Unplug ESP32 (power off)
4. Plug ESP32 back in (power on)
5. Access web interface
6. Check "Active Alarms"

**Expected:**
- All alarms restored after reboot
- Time resynchronizes from NTP
- Display shows correct time

### Test 7: Stress Test

1. Set maximum number of alarms (5)
2. Access web interface from multiple devices simultaneously
3. Try setting/deleting alarms from different devices
4. Verify no crashes or data corruption

**Expected:**
- System handles multiple clients
- All operations complete successfully
- No data loss or corruption

## Automated Testing (Optional) / 自動テスト（オプション）

### Serial Monitor Script

You can create a simple test script to automate serial output verification:

```python
# test_serial.py
import serial
import time

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
time.sleep(2)

print("Reading serial output...")
for i in range(20):
    line = ser.readline().decode('utf-8').strip()
    print(line)
    
    if "WiFi connected" in line:
        print("✓ WiFi connection successful")
    if "IP address" in line:
        print("✓ IP address obtained")
    if "HTTP server started" in line:
        print("✓ Web server running")

ser.close()
```

### Web API Testing

Test the REST API using curl:

```bash
# Get current time
curl http://192.168.1.100/time

# Get alarms list
curl http://192.168.1.100/alarms

# Set an alarm
curl -X POST http://192.168.1.100/setalarm \
  -H "Content-Type: application/json" \
  -d '{"hour":10,"minute":30,"melody":"1"}'

# Delete an alarm
curl -X POST http://192.168.1.100/deletealarm \
  -H "Content-Type: application/json" \
  -d '{"index":0}'
```

## Performance Testing / パフォーマンステスト

### Response Time Test

Measure web server response time:

```bash
# Install Apache Bench (if not installed)
# apt-get install apache2-utils

# Test 100 requests
ab -n 100 -c 10 http://192.168.1.100/
```

**Expected:**
- Average response time: < 200ms
- No failed requests
- Consistent performance

### Memory Test

Monitor ESP32 memory usage:

Add to `loop()` function temporarily:

```cpp
static unsigned long lastMemCheck = 0;
if (millis() - lastMemCheck > 10000) {
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
  lastMemCheck = millis();
}
```

**Expected:**
- Free heap stable (not decreasing over time)
- Minimum 50KB free heap
- No memory leaks

## Common Issues and Solutions / よくある問題と解決策

### Issue 1: Display shows wrong time

**Symptoms:**
- Display shows time but it's incorrect
- Time is off by hours

**Solutions:**
1. Check timezone offset in code (32400 for JST)
2. Verify NTP server is accessible
3. Check internet connection
4. Wait 1-2 minutes for time to sync

### Issue 2: Alarm doesn't trigger

**Symptoms:**
- Alarm time passes but no sound
- Buzzer doesn't activate

**Solutions:**
1. Verify alarm is enabled (check web interface)
2. Check current time is correct
3. Ensure buzzer is connected to GPIO25
4. Test buzzer manually with tone() function
5. Check alarm time is in 24-hour format

### Issue 3: Web interface inaccessible

**Symptoms:**
- Cannot access web page
- Browser shows "Cannot connect"

**Solutions:**
1. Verify ESP32 connected to WiFi (check serial monitor)
2. Check IP address is correct
3. Ensure device is on same WiFi network
4. Try accessing from different device
5. Restart ESP32

### Issue 4: Buzzer makes continuous tone

**Symptoms:**
- Buzzer plays single continuous tone
- No melody

**Solutions:**
1. **You are using active buzzer** - Replace with passive buzzer
2. This is the most common issue
3. Passive buzzers have no internal oscillator
4. Active buzzers only make single tone

### Issue 5: Random resets/crashes

**Symptoms:**
- ESP32 restarts randomly
- Watchdog timer resets
- Unstable operation

**Solutions:**
1. Check power supply (minimum 500mA)
2. Use quality USB cable
3. Add decoupling capacitors
4. Reduce WiFi transmit power
5. Check for code infinite loops

## Test Report Template / テストレポートテンプレート

Use this template to document your testing:

```
Date: __________
ESP32 Board: __________
Display Model: __________
Buzzer Type: __________

[ ] Test 1: Serial Monitor - PASS/FAIL
    Notes: ________________________________

[ ] Test 2: Display - PASS/FAIL
    Notes: ________________________________

[ ] Test 3: Web Interface - PASS/FAIL
    Notes: ________________________________

[ ] Test 4: Alarm Trigger - PASS/FAIL
    Notes: ________________________________

[ ] Test 5: Multiple Alarms - PASS/FAIL
    Notes: ________________________________

[ ] Test 6: Persistence - PASS/FAIL
    Notes: ________________________________

[ ] Test 7: Stress Test - PASS/FAIL
    Notes: ________________________________

Overall Result: PASS/FAIL
Comments: ________________________________
```

## Success Criteria / 成功基準

Your ESP32 Alarm Clock is working correctly if:

✅ **Display**
- Shows correct current time
- Updates every second
- Readable brightness

✅ **Web Interface**
- Accessible from WiFi devices
- Displays current time
- Can add/delete alarms
- Responsive on mobile

✅ **Alarms**
- Trigger at correct time
- Play selected melody
- Repeat for 60 seconds
- Persist after reboot

✅ **Stability**
- No random resets
- Stable WiFi connection
- Consistent NTP sync
- No memory leaks

## Next Steps / 次のステップ

After successful testing:

1. **Permanent Installation**
   - Move from breadboard to prototype board or PCB
   - Use solid wire instead of jumpers
   - Add enclosure for protection

2. **Customization**
   - Add more melodies
   - Adjust alarm duration
   - Change display brightness
   - Add additional features

3. **Optimization**
   - Reduce power consumption
   - Implement sleep modes
   - Add battery backup
   - Improve web UI

4. **Sharing**
   - Document your build
   - Share photos/videos
   - Contribute improvements
   - Help others troubleshoot
