# Wiring Instructions / 配線手順

## English

### Components Connection

#### TM1637 4-Digit 7-Segment Display

The TM1637 is a 4-pin display module:

1. **CLK (Clock)** → ESP32 GPIO22
2. **DIO (Data I/O)** → ESP32 GPIO21
3. **VCC (Power)** → ESP32 3.3V (or 5V)
4. **GND (Ground)** → ESP32 GND

**Note**: The TM1637 can work with both 3.3V and 5V. If using 5V, connect to the 5V pin on ESP32.

#### Passive Buzzer (Electronic Melody)

The passive buzzer has 2 pins:

1. **+ (Positive/Signal)** → ESP32 GPIO25
2. **- (Negative/Ground)** → ESP32 GND

**Important**: Use a **passive buzzer**, not an active buzzer. Passive buzzers can play different tones/melodies, while active buzzers only produce a single tone.

### Step-by-Step Wiring

1. **Place ESP32 on breadboard**
   - Position the ESP32 development board in the center of your breadboard
   - Ensure both sides have accessible rows

2. **Connect TM1637 Display**
   - Insert the TM1637 module on the breadboard
   - Use jumper wires to connect:
     - TM1637 CLK → ESP32 GPIO22 (yellow wire recommended)
     - TM1637 DIO → ESP32 GPIO21 (orange wire recommended)
     - TM1637 VCC → ESP32 3.3V (red wire recommended)
     - TM1637 GND → ESP32 GND (black wire recommended)

3. **Connect Buzzer**
   - Insert the passive buzzer on the breadboard
   - Connect buzzer + (longer leg) → ESP32 GPIO25
   - Connect buzzer - (shorter leg) → ESP32 GND

4. **Double-check connections**
   - Verify all connections match the diagram
   - Ensure no short circuits between pins
   - Check power connections (3.3V/5V and GND)

### Visual Reference

```
                 ESP32 DevKit
                 ┌─────────┐
                 │   USB   │
                 ├─────────┤
    (Display CLK)│ GPIO22  │
    (Display DIO)│ GPIO21  │
                 │   ...   │
     (Buzzer Sig)│ GPIO25  │
                 │   ...   │
          (Power)│  3.3V   │
         (Ground)│  GND    │ ←─┐
                 │   ...   │   │
                 └─────────┘   │
                               │
         TM1637                │    Passive Buzzer
         ┌────────┐            │    ┌──────┐
    CLK  │ ●      │            │    │  +   │
    DIO  │ ●      │            │    │      │
    VCC  │ ●      │            │    │  -   │
    GND  │ ●      │────────────┴────┴──────┘
         └────────┘
```

---

## 日本語

### 部品の接続

#### TM1637 4桁7セグメントディスプレイ

TM1637は4ピンのディスプレイモジュールです：

1. **CLK (クロック)** → ESP32 GPIO22
2. **DIO (データI/O)** → ESP32 GPIO21
3. **VCC (電源)** → ESP32 3.3V (または5V)
4. **GND (グランド)** → ESP32 GND

**注意**: TM1637は3.3Vと5Vの両方で動作します。5Vを使用する場合は、ESP32の5Vピンに接続してください。

#### パッシブブザー（電子オルゴール）

パッシブブザーは2ピン：

1. **+ (プラス/信号)** → ESP32 GPIO25
2. **- (マイナス/グランド)** → ESP32 GND

**重要**: **パッシブブザー**を使用してください（アクティブブザーではありません）。パッシブブザーは異なる音程/メロディーを再生できますが、アクティブブザーは単一の音しか出せません。

### 配線手順

1. **ESP32をブレッドボードに配置**
   - ESP32開発ボードをブレッドボードの中央に配置
   - 両側のピンにアクセスできることを確認

2. **TM1637ディスプレイの接続**
   - TM1637モジュールをブレッドボードに挿入
   - ジャンパーワイヤーで接続：
     - TM1637 CLK → ESP32 GPIO22 (黄色のワイヤー推奨)
     - TM1637 DIO → ESP32 GPIO21 (オレンジのワイヤー推奨)
     - TM1637 VCC → ESP32 3.3V (赤のワイヤー推奨)
     - TM1637 GND → ESP32 GND (黒のワイヤー推奨)

3. **ブザーの接続**
   - パッシブブザーをブレッドボードに挿入
   - ブザーの+ (長い足) → ESP32 GPIO25
   - ブザーの- (短い足) → ESP32 GND

4. **接続の再確認**
   - すべての接続が図と一致することを確認
   - ピン間にショートがないことを確認
   - 電源接続（3.3V/5VとGND）を確認

### 配線のヒント

- **色分け**: 配線の種類ごとに色を統一すると、トラブルシューティングが容易になります
  - 赤: 電源 (VCC)
  - 黒: グランド (GND)
  - 黄/オレンジ: 信号線
  
- **配線の長さ**: できるだけ短いジャンパーワイヤーを使用すると、ノイズが減ります

- **ブレッドボードの使い方**: ブレッドボードの電源レール（+ -）を活用すると配線が整理されます

### よくある配線ミス

1. ❌ CLKとDIOの接続を逆にする
2. ❌ アクティブブザーを使用する（パッシブブザーが必要）
3. ❌ 電源（VCC）とグランド（GND）を逆接続
4. ❌ GPIO番号を間違える

### 配線確認チェックリスト

- [ ] TM1637 CLK → GPIO22
- [ ] TM1637 DIO → GPIO21
- [ ] TM1637 VCC → 3.3V または 5V
- [ ] TM1637 GND → GND
- [ ] ブザー + → GPIO25
- [ ] ブザー - → GND
- [ ] 電源とグランドが正しく接続されている
- [ ] ピン間にショートがない
