# Screenshots & Interface / スクリーンショット・インターフェース

## Web Interface Preview / Webインターフェースプレビュー

### Main Page / メインページ

The web interface features a clean, mobile-friendly design:

```
┌─────────────────────────────────────┐
│     🕐 ESP32 Alarm Clock            │
├─────────────────────────────────────┤
│                                     │
│         Current Time Display        │
│              12:34                  │
│                                     │
├─────────────────────────────────────┤
│                                     │
│        Set New Alarm                │
│                                     │
│  Hour (0-23):  [  7  ]              │
│  Minute (0-59): [ 30  ]             │
│                                     │
│  Melody: [Melody 1 (Simple) ▼]     │
│                                     │
│         [ Add Alarm ]               │
│                                     │
├─────────────────────────────────────┤
│                                     │
│        Active Alarms                │
│                                     │
│  ┌───────────────────────────────┐  │
│  │ 07:30 - Melody 1 ✓   [Delete]│  │
│  └───────────────────────────────┘  │
│                                     │
│  ┌───────────────────────────────┐  │
│  │ 12:00 - Melody 2 ✓   [Delete]│  │
│  └───────────────────────────────┘  │
│                                     │
└─────────────────────────────────────┘
```

### Color Scheme / 配色

- **Background**: Light gray (#f0f0f0)
- **Cards**: White with subtle shadow
- **Primary Button**: Green (#4CAF50)
- **Delete Button**: Red (#f44336)
- **Text**: Dark gray (#333)
- **Current Time**: Green (#4CAF50)

### Responsive Design / レスポンシブデザイン

The interface adapts to different screen sizes:

#### Mobile Phone (Portrait)
- Single column layout
- Touch-friendly buttons
- Large input fields
- Easy to use one-handed

#### Tablet
- Wider layout (max 600px)
- Centered content
- Comfortable spacing

#### Desktop Browser
- Same layout as tablet
- Centered on screen
- Mouse and keyboard friendly

## Display States / ディスプレイの状態

### TM1637 7-Segment Display

#### 1. Connecting to WiFi
```
┌─────────────┐
│ - - : - -   │  Shows dashes while connecting
└─────────────┘
```

#### 2. Normal Time Display
```
┌─────────────┐
│ 1 2 : 3 4   │  Shows current time (HH:MM)
└─────────────┘
        ↑
      Colon blinks
```

#### 3. Early Morning (Leading Zero)
```
┌─────────────┐
│ 0 7 : 3 0   │  7:30 AM with leading zero
└─────────────┘
```

#### 4. Late Night (24-hour format)
```
┌─────────────┐
│ 2 3 : 4 5   │  11:45 PM (23:45)
└─────────────┘
```

## LED Indicators / LED インジケーター

### Display Brightness Levels

```
Brightness 0x0f (Maximum):
████████████████  Very bright, good for daytime

Brightness 0x07 (Medium):
████████          Good for indoor use

Brightness 0x03 (Low):
████              Good for nighttime

Brightness 0x00 (Minimum):
██                Very dim, barely visible
```

## User Flow / ユーザーフロー

### Setting an Alarm

```
1. User opens browser
   ↓
2. Navigates to ESP32 IP address
   ↓
3. Sees main page with current time
   ↓
4. Enters alarm time (hour and minute)
   ↓
5. Selects melody
   ↓
6. Clicks "Add Alarm"
   ↓
7. Sees success message
   ↓
8. Alarm appears in "Active Alarms" list
   ↓
9. Alarm is saved to ESP32 flash memory
```

### When Alarm Triggers

```
1. Current time matches alarm time
   ↓
2. ESP32 detects match
   ↓
3. Buzzer starts playing selected melody
   ↓
4. Melody plays completely
   ↓
5. Short pause (0.3 seconds)
   ↓
6. Melody repeats
   ↓
7. Continues for 60 seconds
   ↓
8. Buzzer stops automatically
```

## API Endpoints / APIエンドポイント

### Visual API Flow

```
GET /
├─ Returns: Full HTML web interface
└─ Status: 200 OK

GET /time
├─ Returns: {"time": "12:34"}
└─ Status: 200 OK

GET /alarms
├─ Returns: {"alarms": [...]}
└─ Status: 200 OK

POST /setalarm
├─ Receives: {"hour": 7, "minute": 30, "melody": "1"}
├─ Returns: {"message": "Alarm set successfully"}
└─ Status: 200 OK

POST /deletealarm
├─ Receives: {"index": 0}
├─ Returns: {"message": "Alarm deleted"}
└─ Status: 200 OK
```

## Error States / エラー状態

### Web Interface Errors

```
┌─────────────────────────────────────┐
│     ⚠️ Error                         │
├─────────────────────────────────────┤
│                                     │
│  Maximum 5 alarms                   │
│                                     │
│  You have reached the maximum       │
│  number of alarms. Please delete    │
│  an existing alarm before adding    │
│  a new one.                         │
│                                     │
│         [ OK ]                      │
│                                     │
└─────────────────────────────────────┘
```

### Connection Errors

```
┌─────────────────────────────────────┐
│     ⚠️ Cannot Connect                │
├─────────────────────────────────────┤
│                                     │
│  Unable to reach ESP32              │
│                                     │
│  Please check:                      │
│  • ESP32 is powered on              │
│  • You're on the same WiFi          │
│  • IP address is correct            │
│                                     │
│         [ Retry ]                   │
│                                     │
└─────────────────────────────────────┘
```

## Icons & Symbols / アイコン・シンボル

The web interface uses Unicode symbols for visual clarity:

- 🕐 - Clock (in title)
- ✓ - Enabled alarm (checkmark)
- ✗ - Disabled alarm (cross)
- ⚠️ - Warning/error

## Accessibility / アクセシビリティ

### Features

- **Large touch targets**: Minimum 44x44px
- **Clear contrast**: WCAG AA compliant
- **Mobile-friendly**: Works on all screen sizes
- **No JavaScript required**: Basic functionality works without JS
- **Semantic HTML**: Proper heading structure
- **Form labels**: All inputs properly labeled

## Future UI Enhancements / 今後のUI改善案

Potential improvements for future versions:

1. **Dark Mode** - For nighttime use
2. **Alarm Groups** - Weekday vs weekend alarms
3. **Snooze Button** - Virtual snooze via web interface
4. **Custom Melodies** - Upload your own
5. **Schedule View** - Calendar display of alarms
6. **Statistics** - Track alarm usage
7. **Multi-language** - Full Japanese interface option
8. **Themes** - Customizable colors
9. **Widgets** - Weather, news, etc.
10. **Voice Control** - Integration with smart assistants

## Browser Compatibility / ブラウザ互換性

Tested and working on:

- ✅ Chrome (Desktop & Mobile)
- ✅ Safari (iOS & macOS)
- ✅ Firefox (Desktop & Mobile)
- ✅ Edge (Desktop & Mobile)
- ✅ Samsung Internet
- ✅ Opera

## Loading States / ローディング状態

### Initial Page Load

```
Step 1: Connecting...
Step 2: Loading interface...
Step 3: Fetching alarms...
Step 4: Ready!
```

### Adding Alarm

```
User clicks "Add Alarm"
  ↓
Button shows "Adding..."
  ↓
Request sent to ESP32
  ↓
Response received
  ↓
Success message shown
  ↓
Alarm list refreshed
  ↓
Button returns to "Add Alarm"
```

## Performance / パフォーマンス

Expected loading times:

- **Initial page load**: < 1 second
- **Time update**: < 100ms
- **Add/delete alarm**: < 500ms
- **Alarm list refresh**: < 300ms

## Mobile App Alternative / モバイルアプリの代替

While this project uses a web interface, you can create a "native-like" experience:

### iOS (Safari)

1. Open web interface
2. Tap Share button
3. Select "Add to Home Screen"
4. Icon appears on home screen
5. Opens in full-screen mode

### Android (Chrome)

1. Open web interface
2. Tap Menu (⋮)
3. Select "Add to Home screen"
4. Icon appears on home screen
5. Opens like a native app

This creates a Progressive Web App (PWA) experience without needing to develop separate native apps!
