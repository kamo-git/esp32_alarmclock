# ESP32 Alarm Clock with TM1637 Display

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-blue.svg)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/Framework-Arduino-00979D.svg)](https://www.arduino.cc/)

ESP32ベースのアラームクロックシステム。TM1637 7セグメントディスプレイ、電子オルゴール（ブザー）、NTP時刻同期、WiFi経由でのアラーム設定機能を搭載。

---

## 🚀 Quick Start / クイックスタート

**New to this project?** Start here: **[QUICKSTART.md](QUICKSTART.md)** - Get running in 5 steps (~35 minutes)!

このプロジェクトが初めてですか？ **[QUICKSTART.md](QUICKSTART.md)** から始めてください - 5ステップで完成（約35分）！

---

## 📚 Documentation / ドキュメント

| Document | Description |
|----------|-------------|
| **[Quick Start Guide](QUICKSTART.md)** | Get started in 5 steps! / 5ステップで始める |
| **[Wiring Instructions](WIRING.md)** | Detailed wiring guide / 詳細な配線ガイド |
| **[Configuration Guide](CONFIGURATION.md)** | Customize settings / 設定のカスタマイズ |
| **[Examples](EXAMPLES.md)** | Usage examples and modifications / 使用例とカスタマイズ |
| **[Testing Guide](TESTING.md)** | How to test your build / テスト方法 |
| **[Architecture](ARCHITECTURE.md)** | System design details / システム設計の詳細 |
| **[Screenshots](SCREENSHOTS.md)** | UI/UX documentation / UI/UXドキュメント |
| **[Summary](SUMMARY.md)** | Project overview / プロジェクト概要 |

---

---

## 🎯 Features / 機能

### Hardware / ハードウェア
- ✅ TM1637 4桁7セグメントディスプレイでの時刻表示
- ✅ パッシブブザーによるメロディー再生
- ✅ 5本のワイヤーのみでシンプルな配線

### Software / ソフトウェア
- ✅ NTPサーバーによる自動時刻同期（JST対応）
- ✅ WiFi経由でのアラーム設定
- ✅ スマートフォン対応のWebインターフェース
- ✅ 複数のアラーム設定（最大5個）
- ✅ 2種類のメロディー選択
- ✅ アラーム設定の永続化（再起動後も保持）

### Interface / インターフェース
- ✅ レスポンシブWebデザイン
- ✅ リアルタイム時刻表示
- ✅ RESTful API
- ✅ ゼロ依存関係（外部ライブラリ不要）

---

## 💡 Demo / デモ

### Hardware Setup / ハードウェア構成
```
┌─────────────┐
│  ESP32 Dev  │
│   Board     │
└─────┬───────┘
      │
      ├─GPIO22──→ TM1637 CLK
      ├─GPIO21──→ TM1637 DIO
      ├─GPIO25──→ Buzzer +
      ├─3.3V───→ TM1637 VCC
      └─GND────→ TM1637 GND, Buzzer -
```

### Web Interface Preview / Webインターフェースプレビュー
```
🕐 ESP32 Alarm Clock
─────────────────────
Current Time: 12:34

Set New Alarm
  Hour: [__]  Minute: [__]
  Melody: [Melody 1 ▼]
  [  Add Alarm  ]

Active Alarms
  ┌─────────────────────┐
  │ 07:30 - Melody 1 ✓  │ [Delete]
  └─────────────────────┘
  ┌─────────────────────┐
  │ 12:00 - Melody 2 ✓  │ [Delete]
  └─────────────────────┘
```

---

## 必要な部品 (Required Components)

| 部品 | 数量 | 備考 |
|------|------|------|
| ESP32 開発ボード | 1 | DevKit等 |
| TM1637 4桁7セグメントディスプレイ | 1 | アノードコモン対応 |
| 圧電ブザー（パッシブブザー） | 1 | 電子オルゴール用 |
| ブレッドボード | 1 | プロトタイピング用 |
| ジャンパーワイヤー | 適量 | - |

## 配線図 (Wiring Diagram)

```
ESP32          TM1637 Display
GPIO22  ----->  CLK
GPIO21  ----->  DIO
3.3V    ----->  VCC
GND     ----->  GND

ESP32          Buzzer
GPIO25  ----->  +（Signal）
GND     ----->  -（GND）
```

### ピン配置詳細

| ESP32 GPIO | 接続先 | 用途 |
|------------|--------|------|
| GPIO 22 | TM1637 CLK | ディスプレイクロック |
| GPIO 21 | TM1637 DIO | ディスプレイデータ |
| GPIO 25 | ブザー+ | メロディー出力 |
| 3.3V | TM1637 VCC | 電源 |
| GND | TM1637 GND, ブザー- | グランド |

## セットアップ (Setup)

### 1. 開発環境のインストール

[PlatformIO](https://platformio.org/)をインストールしてください。Visual Studio Code拡張機能が推奨されます。

```bash
# VS Codeの場合
# 拡張機能 "PlatformIO IDE" をインストール
```

### 2. プロジェクトのクローン

```bash
git clone https://github.com/kamo-git/esp32_alarmclock.git
cd esp32_alarmclock
```

### 3. WiFi設定の変更

`src/main.cpp`を開き、以下の行を自分のWiFi環境に合わせて変更します：

```cpp
const char* ssid = "YourWiFiSSID";        // 自分のWiFi SSID
const char* password = "YourWiFiPassword"; // 自分のWiFiパスワード
```

### 4. ビルドとアップロード

```bash
# PlatformIO CLIの場合
pio run --target upload

# VS Codeの場合
# PlatformIO: Upload を実行
```

### 5. シリアルモニターで確認

```bash
pio device monitor

# 出力例:
# Connecting to WiFi...
# WiFi connected
# IP address: 192.168.1.100
# HTTP server started
```

## 使い方 (Usage)

### アラーム設定用Webアプリへのアクセス

1. ESP32がWiFiに接続されたら、シリアルモニターに表示されるIPアドレスを確認
2. スマートフォンまたはPCのブラウザで `http://[IPアドレス]` にアクセス
3. Webインターフェースが表示されます

### アラームの設定

1. 「Set New Alarm」セクションで時刻を入力（24時間形式）
   - Hour: 0-23
   - Minute: 0-59
2. メロディーを選択
   - Melody 1: シンプルな4音
   - Melody 2: スケール（ドレミファソラシド）
3. 「Add Alarm」ボタンをクリック

### アラームの削除

「Active Alarms」セクションに表示されているアラームの「Delete」ボタンをクリック

### 時刻表示

- TM1637ディスプレイに現在時刻がHH:MM形式で表示されます
- NTPサーバーから自動で時刻を取得・同期します（日本標準時 JST）

## カスタマイズ (Customization)

### タイムゾーンの変更

`src/main.cpp`の以下の行を変更：

```cpp
// 32400 = UTC+9 (JST)
// UTC+0の場合は 0
// UTC+8の場合は 28800
NTPClient timeClient(ntpUDP, "pool.ntp.org", 32400, 60000);
```

### メロディーの追加

`src/main.cpp`で新しいメロディーを定義：

```cpp
int melody3[] = {NOTE_E4, NOTE_D4, NOTE_C4, 0};
int melody3Duration[] = {200, 200, 400, 0};
```

### ピン配置の変更

`src/main.cpp`の以下の定義を変更：

```cpp
#define CLK_PIN 22  // TM1637 CLK pin
#define DIO_PIN 21  // TM1637 DIO pin
#define BUZZER_PIN 25  // Buzzer pin
```

## トラブルシューティング (Troubleshooting)

### WiFiに接続できない

- SSIDとパスワードが正しいか確認
- ESP32が2.4GHz WiFiの範囲内にあるか確認（5GHzは非対応）
- シリアルモニターで接続状態を確認

### ディスプレイが表示されない

- 配線を確認（CLK, DIO, VCC, GND）
- TM1637の電源電圧を確認（3.3Vまたは5V）
- ディスプレイの明るさ設定を変更してみる

### ブザーが鳴らない

- パッシブブザーを使用しているか確認（アクティブブザーは不可）
- 配線を確認
- 別のGPIOピンで試してみる

### 時刻がずれる

- NTPサーバーへの接続を確認
- タイムゾーン設定を確認
- WiFi接続が安定しているか確認

## ライセンス (License)

MIT License - 詳細は[LICENSE](LICENSE)ファイルを参照

## 作者 (Author)

kamo-git

## 貢献 (Contributing)

プルリクエストを歓迎します。大きな変更の場合は、まずissueを開いて変更内容を議論してください。