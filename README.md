# NerdMiner Bitaxe Fleet Monitor

**ESP32 Display for Monitoring Your Bitaxe Mining Fleet**

A fork of [NerdMiner_v2](https://github.com/BitMaker-hub/NerdMiner_v2) modified to display real-time stats from your Bitaxe swarm on an ESP32-2432S028R touchscreen.

![image](images/bgNerdMinerV2.png)

## Features

- **Bitaxe Swarm Monitor** - View all your miners at a glance (up to 6-7 on screen)
- **BTC Candlestick Chart** - 24-hour price action from Binance
- **Live Stats** - Hashrate, temps, power, efficiency per miner
- **Color-coded Status** - Green/red for online/offline, temp warnings
- **Touch Controls** - Navigate screens, rotate display, toggle backlight

## Requirements

This project requires **two repositories** to work together:

| Component | Description |
|-----------|-------------|
| [bitaxe-monitor](https://github.com/LDevine15/bitaxe-monitor) | Python API server that polls your Bitaxe miners |
| This repo | ESP32 firmware that displays the data |

### Hardware

- ESP32-2432S028R (2.8" touchscreen) - [Aliexpress](https://s.click.aliexpress.com/e/_DdXkvLv)

---

## Setup Instructions

### Step 1: Clone Both Repositories

```bash
# Clone the Python API server
git clone https://github.com/LDevine15/bitaxe-monitor.git

# Clone this firmware repo
git clone https://github.com/YOUR_USERNAME/nerdminer_v2_fleetmonitor.git
```

### Step 2: Configure bitaxe-monitor

```bash
cd bitaxe-monitor
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

**Create `config.yaml`** with your Bitaxe miner IPs (see config.yaml.example):

```yaml
devices:
  - name: "bitaxe-1"
    ip: "192.168.1.100"
    enabled: true
  - name: "bitaxe-2"
    ip: "192.168.1.101"
    enabled: true
  # Add more miners as needed...

logging:
  poll_interval: 30
  database_path: "./data/metrics.db"
  log_level: "INFO"

safety:
  max_temp_warning: 65
  max_temp_shutdown: 70
```

**Start the data logger** (collects metrics from your miners):
```bash
python run_logger.py
```

### Step 3: Start the API Server

In a separate terminal:

```bash
cd bitaxe-monitor
source venv/bin/activate
python api_server.py
```

You should see:
```
Starting API server for X device(s)
* Running on all addresses (0.0.0.0)
* Running on http://192.168.1.XXX:5001
```

**Test it works:**
```bash
curl http://YOUR_COMPUTER_IP:5001/swarm
```

**Tip:** Use `tmux` or `screen` to keep both `run_logger.py` and `api_server.py` running persistently.

### Step 4: Configure the ESP32 Firmware

Find your computer's IP address:
```bash
# Mac
ifconfig | grep "inet " | grep -v 127.0.0.1

# Linux
hostname -I
```

Edit `src/monitor.h` line 40:
```cpp
#define getBitaxeSwarm "http://192.168.1.XXX:5001/swarm"  // YOUR computer's IP
```

**Note:** Uses port 5001 (not 5000) because macOS AirPlay Receiver uses 5000.

### Step 5: Flash the Firmware

Using PlatformIO:
```bash
cd nerdminer_v2_fleetmonitor

# Build
pio run -e ESP32-2432S028R

# Upload (with device connected via USB w/ bootloader active)
pio run -e ESP32-2432S028R --target upload
```

### Step 6: Connect ESP32 to WiFi

On first boot, the ESP32 creates a WiFi access point:
1. Connect to: `NerdMinerAP` (password: `MineYourCoins`)
2. Browser opens automatically (or go to `192.168.4.1`)
3. Enter your WiFi credentials
4. Save & restart

---

## Usage

### Touch Controls

| Area | Action |
|------|--------|
| Right side tap | Next screen |
| Left side tap | Previous screen |
| Top-right corner | Toggle display on/off |
| Bottom-left corner | Rotate screen 180 degrees |

### What You'll See

**Swarm Summary (top line):**
```
0.85 TH/s | 4/4 | 22.3 J/TH | 190W
  ^           ^      ^         ^
  |           |      |         Total power
  |           |      Avg efficiency
  |           Active/Total miners
  Total hashrate
```

**Individual Miners:**
```
bitaxe-1  212.5  65C  48W 22.6J
  ^         ^     ^    ^    ^
  Name    Hash  Temp Power Eff
```

**Color Coding:**
- **Green name** = online, **Red name** = offline
- **Green temp** = < 65C, **Orange** = 65-70C, **Red** = > 70C

---

## Troubleshooting

### "Swarm API Offline" on ESP32

1. Check API server is running: `curl http://YOUR_IP:5001/health`
2. Verify ESP32 and computer are on the same WiFi network
3. Check firewall isn't blocking port 5001
4. Monitor serial output: `pio device monitor`

### Miners Showing "OFFLINE"

1. Verify `config.yaml` has correct miner IPs
2. Ensure `run_logger.py` is running
3. Check database has recent data: `python stats.py summary bitaxe-1`

### "Loading BTC Chart..." Stuck

- Binance API may be rate limited
- Check ESP32 internet connection
- Wait 5 minutes for automatic retry

---

## Customization

### Change Update Intervals

Edit `src/monitor.h`:
```cpp
#define UPDATE_CHART_min 5   // BTC chart (default: 5 min)
#define UPDATE_SWARM_sec 10  // Swarm data (default: 10 sec)
```

### Show More/Fewer Miners

Edit `src/drivers/displays/esp23_2432s028r.cpp` around line 642:
```cpp
int displayCount = min(swarmData.miner_count, 6);  // Change 6 to preference
```
---

## Credits

- Original [NerdMiner_v2](https://github.com/BitMaker-hub/NerdMiner_v2) by BitMaker-hub
- [bitaxe-monitor](https://github.com/LDevine15/bitaxe-monitor) Python monitoring tool
