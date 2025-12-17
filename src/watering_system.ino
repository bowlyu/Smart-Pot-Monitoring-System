/***************************************************
 * Smart Plant – ESP8266 + Soil Sensor + Relay + Blynk
 * Board: NodeMCU 1.0 (ESP-12E Module)
 ***************************************************/

// Debug print
#define BLYNK_PRINT Serial

// === Blynk Template & Auth ===
#define BLYNK_TEMPLATE_ID "TMPL6O8CYOG9N"
#define BLYNK_TEMPLATE_NAME "test"
#define BLYNK_AUTH_TOKEN "AWvOVIx4I97GZj-tLmrUNRVbUCRSJRig"

// === Wi-Fi 設定（請改成你的 AP） ===
char auth[] = BLYNK_AUTH_TOKEN;
const char* ssid = "Factory2_2.4G";
const char* pass = "118factory2";

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// === 硬體腳位定義 ===
#define SOIL_PIN   A0        // 土壤濕度感測器 A0 類比輸出接這裡
#define RELAY_PIN  D1        // 繼電器 IN 接的腳位（若接別腳請改這裡）

// === 繼電器觸發邏輯 ===
// 依你現在實測：HIGH = 開泵，LOW = 關泵
#define RELAY_ON   HIGH
#define RELAY_OFF  LOW

// === 自動澆水參數（可依需要調整） ===
const unsigned long PUMP_DURATION_MS = 3000;            // 每次澆水最長時間 5 秒
const unsigned long MIN_INTERVAL_MS  = 10UL * 1 * 1000; // 兩次澆水最短間隔 10 分鐘（測試可改小）
const int HYSTERESIS_PCT = 10;                          // 濕度遲滯：關泵門檻 = 低門檻 + 10%

// === 土壤濕度校正（A0 讀值 0~1023）===
// 建議 soilDryAdc 用 1023（ESP8266 analogRead 最大通常是 1023）
int soilDryAdc  = 1023;  // 空氣 / 很乾時 A0 值
int soilWetAdc  = 430;   // 插在水裡或非常濕時 A0 值

// ===================================================
// ✅ Blynk 虛擬腳位（重新分配：避開 V1/V2/V3）
// ===================================================
#define VPIN_SOIL        10   // V10: Soil Moisture %
#define VPIN_THRESHOLD   11   // V11: Slider -> Auto LOW threshold %
#define VPIN_AUTOMODE    12   // V12: Switch -> Auto mode ON/OFF
#define VPIN_MANUAL      13   // V13: Button -> Manual pump (push)
#define VPIN_PUMP_STATE  14   // V14: LED/Value -> Pump state (1=ON, 0=OFF)

BlynkTimer timer;

bool autoMode = true;
int  autoThreshold = 40;

bool pumpOn = false;
unsigned long pumpStartTime = 0;

// 改成「上次關泵時間」更合理
unsigned long lastPumpEndTime = 0;   // 0 = 尚未澆過水
int lastMoisturePct = 0;

// 將 A0 讀值轉成 0~100%（0 = 最乾，100 = 最濕）
int adcToPercent(int adcRaw)
{
  if (adcRaw < soilWetAdc) adcRaw = soilWetAdc;
  if (adcRaw > soilDryAdc) adcRaw = soilDryAdc;

  int pct = map(adcRaw, soilDryAdc, soilWetAdc, 0, 100);
  if (pct < 0)   pct = 0;
  if (pct > 100) pct = 100;
  return pct;
}

// 啟動水泵（一次澆水）
void startPumpOnce()
{
  unsigned long now = millis();

  // 以「上次關泵」為基準做間隔保護
  if (lastPumpEndTime != 0 && (now - lastPumpEndTime < MIN_INTERVAL_MS)) {
    Serial.println(F("[PUMP] Skip: interval too short."));
    return;
  }

  Serial.println(F("[PUMP] ON"));
  digitalWrite(RELAY_PIN, RELAY_ON);
  pumpOn = true;
  pumpStartTime = now;

  // ✅ 回報 Pump 狀態到 V14（不要再回寫手動按鈕）
  Blynk.virtualWrite(VPIN_PUMP_STATE, 1);
}

// 停止水泵
void stopPump()
{
  if (!pumpOn) return;

  Serial.println(F("[PUMP] OFF"));
  digitalWrite(RELAY_PIN, RELAY_OFF);
  pumpOn = false;

  // ✅ 記錄關泵時間（用來做 MIN_INTERVAL）
  lastPumpEndTime = millis();

  // ✅ 回報 Pump 狀態到 V14
  Blynk.virtualWrite(VPIN_PUMP_STATE, 0);
}

// 週期讀取土壤濕度並上傳 & 自動澆水判斷
void readSoilAndControl()
{
  int adcRaw = analogRead(SOIL_PIN);
  int moisturePct = adcToPercent(adcRaw);
  lastMoisturePct = moisturePct;

  Serial.print(F("Soil ADC = "));
  Serial.print(adcRaw);
  Serial.print(F("  -> Moisture = "));
  Serial.print(moisturePct);
  Serial.println(F("%"));

  // ✅ 上傳到 Blynk：V10
  Blynk.virtualWrite(VPIN_SOIL, moisturePct);

  // 自動澆水（只負責「開」）
  if (autoMode && !pumpOn) {
    if (moisturePct < autoThreshold) {
      Serial.println(F("[AUTO] Soil too dry, start pump."));
      startPumpOnce();
    }
  }
}

// 檢查是否超過澆水時間，或濕度已回升，時間到或夠濕就關泵
void pumpSafetyCheck()
{
  if (!pumpOn) return;

  unsigned long now = millis();
  bool timeUp = (now - pumpStartTime >= PUMP_DURATION_MS);

  int highThreshold = autoThreshold + HYSTERESIS_PCT;
  if (highThreshold > 100) highThreshold = 100;
  bool wetEnough = (lastMoisturePct >= highThreshold);

  if (timeUp || wetEnough) {
    if (timeUp)    Serial.println(F("[PUMP] Duration reached, stop pump."));
    if (wetEnough) Serial.println(F("[PUMP] Soil moisture high enough, stop pump."));
    stopPump();
  }
}

// ===================== Blynk 回呼 =====================

// V11：Slider 設定自動澆水「低門檻」
BLYNK_WRITE(VPIN_THRESHOLD)
{
  autoThreshold = param.asInt();
  Serial.print(F("[Blynk] New autoThreshold = "));
  Serial.println(autoThreshold);
}

// V12：Switch 自動模式 ON/OFF
BLYNK_WRITE(VPIN_AUTOMODE)
{
  autoMode = (param.asInt() == 1);
  Serial.print(F("[Blynk] Auto mode = "));
  Serial.println(autoMode ? "ON" : "OFF");
}

// V13：Button 手動啟動一次水泵（建議 Push）
BLYNK_WRITE(VPIN_MANUAL)
{
  int btn = param.asInt();
  if (btn == 1) {
    Serial.println(F("[Blynk] Manual pump button pressed."));
    startPumpOnce();
  }
  // 放開時不強制關泵，由 pumpSafetyCheck 控制
}

BLYNK_CONNECTED()
{
  // 連線後同步目前設定到 App
  Blynk.virtualWrite(VPIN_THRESHOLD, autoThreshold);
  Blynk.virtualWrite(VPIN_AUTOMODE, autoMode ? 1 : 0);
  Blynk.virtualWrite(VPIN_PUMP_STATE, pumpOn ? 1 : 0);
}

// =======================================================

void setup()
{
  Serial.begin(9600);
  delay(100);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);

  // 連線到 Blynk Cloud
  Blynk.begin(auth, ssid, pass);

  // 每 5 秒更新濕度 & 判斷要不要開泵
  timer.setInterval(5000L, readSoilAndControl);
  // 每 200 ms 檢查要不要關泵
  timer.setInterval(200L, pumpSafetyCheck);

  Serial.println(F("Smart Plant system started (Blynk + local logic)."));
}

void loop()
{
  Blynk.run();
  timer.run();
}