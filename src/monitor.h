#ifndef MONITOR_API_H
#define MONITOR_API_H

#include <Arduino.h>

// Monitor states
#define SCREEN_MINING   0
#define SCREEN_CLOCK    1
#define SCREEN_GLOBAL   2
#define NO_SCREEN       3   //Used when board has no TFT

//Time update period
#define UPDATE_PERIOD_h   5

//API BTC price (Update to USDT cus it's more liquidity and flow price updade)   

//#define getBTCAPI "https://api.coindesk.com/v1/bpi/currentprice.json" -- doesn't work anymore
//#define getBTCAPI "https://api.blockchain.com/v3/exchange/tickers/BTC-USDT" -- updates infrequently
#define getBTCAPI "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd"

#define UPDATE_BTC_min   1

//API Block height
#define getHeightAPI "https://mempool.space/api/blocks/tip/height"
#define UPDATE_Height_min 2

//APIs Global Stats
#define getGlobalHash "https://mempool.space/api/v1/mining/hashrate/3d"
#define getDifficulty "https://mempool.space/api/v1/difficulty-adjustment"
#define getFees "https://mempool.space/api/v1/fees/recommended"
#define UPDATE_Global_min 2

//API public-pool.io
// https://public-pool.io:40557/api/client/btcString
#define getPublicPool "https://public-pool.io:40557/api/client/" // +btcString
#define UPDATE_POOL_min   1

// Bitaxe Swarm API (set your Python server IP)
// Example: "http://192.168.1.100:5001/swarm"
#define getBitaxeSwarm "http://192.168.1.64:5001/swarm"  // Your Mac IP
#define UPDATE_SWARM_sec  10  // Update every 10 seconds

#define NEXT_HALVING_EVENT 1050000 //840000
#define HALVING_BLOCKS 210000

enum NMState {
  NM_waitingConfig,
  NM_Connecting,
  NM_hashing
};

typedef struct{
  uint8_t screen;
  bool rotation;
  NMState NerdStatus;
}monitor_data;

typedef struct{
  String globalHash; //hexahashes
  String currentBlock;
  String difficulty;
  String blocksHalving;
  float progressPercent;
  int remainingBlocks;
  int halfHourFee;
#ifdef NERDMINER_T_HMI
  int fastestFee;
  int hourFee;
  int economyFee;
  int minimumFee;
#endif
}global_data;

typedef struct {
  String completedShares;
  String totalMHashes;
  String totalKHashes;
  String currentHashRate;
  String templates;
  String bestDiff;
  String timeMining;
  String valids;
  String temp;
  String currentTime;
}mining_data;

typedef struct {
  String completedShares;
  String totalKHashes;
  String currentHashRate;
  String btcPrice;
  String blockHeight;
  String currentTime;  
  String currentDate;
}clock_data;

typedef struct {
  String currentHashRate;
  String valids;
  unsigned long currentHours;
  unsigned long currentMinutes;
  unsigned long currentSeconds;
}clock_data_t;

typedef struct {
  String completedShares;
  String totalKHashes;
  String currentHashRate;
  String btcPrice;
  String currentTime;
  String halfHourFee;
#ifdef NERDMINER_T_HMI
  String hourFee;
  String fastestFee;
  String economyFee;
  String minimumFee;
#endif
  String netwrokDifficulty;
  String globalHashRate;
  String blockHeight;
  float progressPercent;
  String remainingBlocks;
}coin_data;

typedef struct{
  int workersCount;       // Workers count, how many nerdminers using your address
  String workersHash;     // Workers Total Hash Rate
  String bestDifficulty;  // Your miners best difficulty
}pool_data;

// Candlestick data for BTC chart
typedef struct {
  float open;
  float high;
  float low;
  float close;
  unsigned long timestamp;
} candle_data;

typedef struct {
  candle_data candles[24];  // 24 hourly candles for 24h chart
  int count;                 // Number of valid candles
  float min_price;           // Min price for scaling
  float max_price;           // Max price for scaling
} btc_chart_data;

// Bitaxe swarm data from Python API
typedef struct {
  String name;
  bool online;
  float hashrate;     // GH/s
  float power;        // W
  float efficiency;   // J/TH
  float asic_temp;    // °C
  float vreg_temp;    // °C
} bitaxe_miner;

typedef struct {
  float total_hashrate;    // GH/s
  float total_power;       // W
  float avg_efficiency;    // J/TH
  int active_count;
  int total_count;
  bitaxe_miner miners[8];  // Support up to 8 miners
  int miner_count;
  bool data_valid;
} swarm_data;

void setup_monitor(void);

mining_data getMiningData(unsigned long mElapsed);
clock_data getClockData(unsigned long mElapsed);
coin_data getCoinData(unsigned long mElapsed);
pool_data getPoolData(void);
btc_chart_data getBTCChartData(void);  // Get 24h candlestick data from Binance
swarm_data getBitaxeSwarmData(void);   // Get swarm data from Python API
String getBTCprice(void);              // Get current BTC price

clock_data_t getClockData_t(unsigned long mElapsed);
String getPoolAPIUrl(void);

#endif //MONITOR_API_H
