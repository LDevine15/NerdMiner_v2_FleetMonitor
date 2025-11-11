# Bitaxe Integration Setup Guide

Custom ESP32-2432S028R display for monitoring Bitaxe swarm + BTC price charts.

## 🎯 Features Implemented

### New Screens (5 total)
1. **Miner Screen** - NerdMiner mining stats
2. **Clock Screen** - Time/date
3. **BTC Price** - Current BTC price
4. **BTC Candlestick Chart** ✨ NEW - 24h chart with 1h candles from Binance
5. **Bitaxe Swarm Monitor** ✨ NEW - Your bitaxe miners status

### Touch Controls
- **Right side tap**: Next screen
- **Left side tap**: Previous screen
- **Top-right corner**: Toggle display on/off
- **Bottom-left corner**: Rotate screen 180° ✨ NEW

---

## 📋 Setup Instructions

### Part 1: Python API Server

1. **Install dependencies:**
   ```bash
   cd ~/Code/bitaxe-monitor
   source venv/bin/activate
   pip install flask flask-cors
   ```

2. **Find your machine's IP address:**
   ```bash
   # On Mac
   ifconfig | grep "inet " | grep -v 127.0.0.1

   # On Linux
   hostname -I
   ```

   Note this IP (e.g., `192.168.1.100`)

3. **Start the API server:**
   ```bash
   python api_server.py
   ```

   Should see:
   ```
   Starting API server for X device(s)
   * Running on all addresses (0.0.0.0)
   * Running on http://192.168.1.100:5001
   ```

   **Note:** Using port 5001 because macOS AirPlay Receiver uses port 5000.

4. **Test the API** (from another terminal):
   ```bash
   curl http://192.168.1.100:5001/swarm
   ```

   Should return JSON with your miners' data.

5. **Keep it running** (optional - use tmux or screen):
   ```bash
   # Install tmux if needed
   brew install tmux

   # Start persistent session
   tmux new -s bitaxe-api
   python api_server.py

   # Detach: Ctrl+B, then D
   # Reattach later: tmux attach -t bitaxe-api
   ```

### Part 2: ESP32 Configuration

1. **Update API URL in NerdMiner code:**

   Edit: `src/monitor.h` line 40
   ```cpp
   #define getBitaxeSwarm "http://192.168.1.100:5001/swarm"  // CHANGE TO YOUR IP
   ```

   Replace `192.168.1.100` with YOUR computer's IP from Part 1, Step 2.
   **Note:** Port 5001 (not 5000) due to macOS AirPlay conflict.

2. **Compile and Upload:**

   Using PlatformIO (recommended):
   ```bash
   cd ~/Code/NerdMiner_v2

   # Build for ESP32-2432S028R
   pio run -e ESP32-2432S028R

   # Upload (with device connected via USB)
   pio run -e ESP32-2432S028R --target upload
   ```

   Or use Arduino IDE:
   - Open `src/NerdMinerV2.ino.cpp`
   - Select board: ESP32 Dev Module
   - Select your USB port
   - Click Upload

3. **Connect to WiFi:**

   On first boot, ESP32 creates a WiFi access point:
   - Connect to: `NerdMinerAP`
   - Browser opens automatically (or go to `192.168.4.1`)
   - Enter your WiFi credentials
   - Save & restart

---

## 🎮 How to Use

### Screen Navigation
Touch the **right side** of screen to cycle through:
1. NerdMiner stats
2. Clock
3. BTC current price
4. **BTC 24h candlestick chart** (updates every 5 min)
5. **Your Bitaxe swarm** (updates every 10 sec)

### Screen Rotation
Touch **bottom-left corner** to flip display 180° for different desk orientations.

### Display On/Off
Touch **top-right corner** to toggle backlight.

---

## 📊 What You'll See

### BTC Candlestick Chart
- **24 hourly candles** from Binance
- **Green = price up**, Red = price down
- Price range labels on left
- Current price at bottom
- Updates every 5 minutes

### Bitaxe Swarm Monitor
**Top line (swarm totals):**
```
0.85 TH/s | 4/4 | 22.3 J/TH | 190W
  ^           ^      ^         ^
  |           |      |         Total power
  |           |      Avg efficiency
  |           Active/Total miners
  Total hashrate
```

**Individual miners (up to 6 shown):**
```
bitaxe-1  212.5  65C  48W 22.6J
  ^         ^     ^    ^    ^
  Name    Hash  Temp Power Eff
```

**Color coding:**
- **Green name** = online
- **Red name** = offline
- **Green temp** = < 65°C
- **Orange temp** = 65-70°C
- **Red temp** = > 70°C

---

## 🔧 Troubleshooting

### "Swarm API Offline" error on ESP32

1. **Check Python server is running:**
   ```bash
   curl http://YOUR_IP:5001/health
   # Should return: {"status":"ok"}
   ```

2. **Verify ESP32 can reach server:**
   - ESP32 and computer must be on same WiFi network
   - Check firewall isn't blocking port 5001
   - Try pinging computer from another device on network

3. **Check serial monitor output:**
   ```bash
   pio device monitor
   ```
   Look for "Swarm API error" messages with HTTP codes.

### "Loading BTC Chart..." stuck

- Binance API might be rate limited
- Check internet connection on ESP32
- Wait 5 minutes for retry

### Miners showing "OFFLINE" when they're running

1. Check `config.yaml` has correct miner IPs
2. Verify `run_logger.py` is running and collecting data
3. Check database has recent data:
   ```bash
   python stats.py summary bitaxe-1
   ```

---

## 💡 Tips

1. **Auto-start API server on boot** (Mac):
   ```bash
   # Create launchd plist
   nano ~/Library/LaunchAgents/com.bitaxe.api.plist
   ```

   Add:
   ```xml
   <?xml version="1.0" encoding="UTF-8"?>
   <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
   <plist version="1.0">
   <dict>
       <key>Label</key>
       <string>com.bitaxe.api</string>
       <key>ProgramArguments</key>
       <array>
           <string>/Users/YOUR_USERNAME/Code/bitaxe-monitor/venv/bin/python</string>
           <string>/Users/YOUR_USERNAME/Code/bitaxe-monitor/api_server.py</string>
       </array>
       <key>RunAtLoad</key>
       <true/>
       <key>KeepAlive</key>
       <true/>
   </dict>
   </plist>
   ```

   Then:
   ```bash
   launchctl load ~/Library/LaunchAgents/com.bitaxe.api.plist
   ```

2. **Use static IP for your computer** so ESP32 URL doesn't change

3. **Monitor via serial console:**
   ```bash
   pio device monitor
   ```
   Shows debug output for API calls, chart updates, etc.

---

## 📁 Files Modified

### Python (bitaxe-monitor)
- `api_server.py` - NEW Flask API server
- `requirements.txt` - Added Flask dependencies

### ESP32 (NerdMiner_v2)
- `src/monitor.h` - Added data structures for charts & swarm
- `src/monitor.cpp` - Added Binance & swarm API functions
- `src/drivers/displays/esp23_2432s028r.cpp` - Added 2 new screens + rotation touch zone

---

## 🎨 Customization Ideas

### Change update intervals
Edit `src/monitor.h`:
```cpp
#define UPDATE_CHART_min 5   // BTC chart (default: 5 min)
#define UPDATE_SWARM_sec 10  // Swarm data (default: 10 sec)
```

### Show more/fewer miners per screen
Edit `esp23_2432s028r.cpp` line 642:
```cpp
int displayCount = min(swarmData.miner_count, 6);  // Change 6 to your preference
```

### Change screen order
Edit `esp23_2432s028r.cpp` line 794:
```cpp
CyclicScreenFunction esp32_2432S028RCyclicScreens[] = {
  esp32_2432S028R_MinerScreen,
  esp32_2432S028R_BitaxeSwarm,  // Move swarm to 2nd position
  esp32_2432S028R_ClockScreen,
  esp32_2432S028R_BTCprice,
  esp32_2432S028R_BTCCandlestick
};
```

### Remove screens you don't want
Just delete from the array above and recompile.

---

## 📸 What to Expect

Your desk display will now show:
- Live BTC price action with proper candlestick charts
- Real-time monitoring of all your Bitaxe miners
- Temperatures, hashrates, efficiency at a glance
- Online/offline status with color coding

Perfect for keeping an eye on your mining operation! 🚀

---

**Questions or issues?** Check the serial monitor output or API server logs for debugging info.
