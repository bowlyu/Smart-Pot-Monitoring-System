// Blynk set up
#define BLYNK_TEMPLATE_ID "TMPL6O8CYOG9N"
#define BLYNK_TEMPLATE_NAME "test"
#define BLYNK_AUTH_TOKEN "AWvOVIx4I97GZj-tLmrUNRVbUCRSJRig"
#define BLYNK_PRINT Serial

#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include "BlynkSimpleEsp32.h"

// ====== 請改成你自己的 Wi-Fi 帳密 ======
char auth[] = BLYNK_AUTH_TOKEN;
const char* ssid = "Factory2_2.4G";
const char* pass = "118factory2";

// ====== Blynk Datastream 設定 ======
// V2: 手機 & Web 的 Video widget（/stream）
// V3: Web Page Image Button（/jpg）
#define VPIN_VIDEO       V2
#define VPIN_SNAPSHOT    V3

// ====== 選擇相機模組：這裡是 AI Thinker ESP32-CAM ======
#define CAMERA_MODEL_AI_THINKER

// AI Thinker ESP32-CAM 腳位設定
#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27

  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
#else
  #error "目前程式只支援 CAMERA_MODEL_AI_THINKER"
#endif

WiFiServer server(80);

// ====== MJPEG 串流狀態（非阻塞設計） ======
WiFiClient streamClient;          // 目前的串流 client
bool streamClientActive = false;  // 是否正在串流
unsigned long streamStartTime = 0;
unsigned long lastStreamFrameTime = 0;

const unsigned long STREAM_FRAME_INTERVAL = 40;        // 每 40ms 一張 ≈ 25fps
const unsigned long STREAM_MAX_DURATION   = 60UL * 1000; // 一條串流最多 60 秒
const char* MJPEG_BOUNDARY = "frame";

// 先宣告函式，避免「not declared in this scope」錯誤
void startCamera();
void sendIndexHtml(WiFiClient &client);
void sendSingleJpeg(WiFiClient &client);
void sendMjpegStream(WiFiClient &client);
void handleClient(WiFiClient &client);

/*************************************************
 *  啟動相機
 *************************************************/
void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    // 有 PSRAM，用比較好的畫質
    config.frame_size   = FRAMESIZE_VGA;  // 640x480
    config.jpeg_quality = 12;            // 0(最好)~63(最差)
    config.fb_count     = 2;
  } else {
    // 沒 PSRAM 降低一點
    config.frame_size   = FRAMESIZE_QVGA; // 320x240
    config.jpeg_quality = 15;
    config.fb_count     = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    // 如果相機啟動失敗，就直接卡在這邊反覆重開
    delay(5000);
    ESP.restart();
  } else {
    Serial.println("Camera init success.");
  }
}

/*************************************************
 *  BLYNK_CONNECTED：這裡一次幫你設定好
 *  - V2 → http://IP/stream
 *  - V3 → http://IP/jpg
 *************************************************/
BLYNK_CONNECTED() {
  IPAddress ip = WiFi.localIP();
  String base = String("http://") + ip.toString();

  String urlStream = base + "/stream";
  String urlJpg    = base + "/jpg";

  Blynk.setProperty(VPIN_VIDEO,    "url", urlStream);
  Blynk.setProperty(VPIN_SNAPSHOT, "url", urlJpg);

  Serial.print("Set Blynk Video URL = ");
  Serial.println(urlStream);
  Serial.print("Set Blynk Snapshot URL = ");
  Serial.println(urlJpg);
}

/*************************************************
 *  /index or / : 簡單測試頁
 *************************************************/
void sendIndexHtml(WiFiClient &client) {
  IPAddress ip = WiFi.localIP();
  String ipStr = ip.toString();

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html><head><meta charset='utf-8'>");
  client.println("<title>ESP32-CAM Smart Plant Viewer</title></head><body>");
  client.print("<h2>ESP32-CAM @ ");
  client.print(ipStr);
  client.println("</h2>");
  client.println("<h3>/stream (MJPEG)</h3>");
  client.println("<img src='/stream' style='max-width:100%;'><br>");
  client.println("<h3>/jpg (Snapshot)</h3>");
  client.println("<img src='/jpg' style='max-width:100%;'><br>");
  client.println("<p>這個頁面只是測試用，真正使用在 Blynk App / Web Dashboard。</p>");
  client.println("</body></html>");
}

/*************************************************
 *  /jpg：單張 JPEG 快照（多 client 可輪流來抓）
 *************************************************/
void sendSingleJpeg(WiFiClient &client) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed (jpg)");
    client.println("HTTP/1.1 500 Internal Server Error");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Camera capture failed");
    client.stop();
    return;
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: image/jpeg");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
  client.println("Pragma: no-cache");
  client.println("Expires: 0");
  client.print("Content-Length: ");
  client.println(fb->len);
  client.println("Connection: close");
  client.println();
  client.write(fb->buf, fb->len);
  client.println();

  esp_camera_fb_return(fb);
  client.stop();
}

// 在 loop() 裡呼叫：如果有串流 client，每次送一張 frame
void handleStreamFrame() {
  if (!streamClientActive) {
    return;
  }

  // 如果 client 已經斷線，關掉
  if (!streamClient.connected()) {
    Serial.println("Stream client disconnected");
    streamClient.stop();
    streamClientActive = false;
    return;
  }

  unsigned long now = millis();

  // 超過最大串流時間，自動收掉，避免永遠卡死
  if (now - streamStartTime > STREAM_MAX_DURATION) {
    Serial.println("Stream timeout, closing client");
    streamClient.stop();
    streamClientActive = false;
    return;
  }

  // 控制 FPS：不到 FRAME_INTERVAL 就先不送新 frame
  if (now - lastStreamFrameTime < STREAM_FRAME_INTERVAL) {
    return;
  }
  lastStreamFrameTime = now;

  // 拍一張照片
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed (stream)");
    streamClient.stop();
    streamClientActive = false;
    return;
  }

  // 寫入 MJPEG frame
  streamClient.print("--");
  streamClient.println(MJPEG_BOUNDARY);
  streamClient.println("Content-Type: image/jpeg");
  streamClient.print("Content-Length: ");
  streamClient.println(fb->len);
  streamClient.println();
  size_t written = streamClient.write(fb->buf, fb->len);
  streamClient.println();

  // 還回 frame buffer
  esp_camera_fb_return(fb);

  if (!written) {
    Serial.println("Stream write failed, closing client");
    streamClient.stop();
    streamClientActive = false;
  }
}

/*************************************************
 *  /stream：MJPEG 串流（一次給一個 client）
 *  這裡加了「時間 / 張數上限」，避免永遠卡死
 *************************************************/
// /stream：啟動 MJPEG 串流（非阻塞，只送 header，之後在 loop 裡慢慢送 frame）
void sendMjpegStream(WiFiClient &client) {
  // 如果已經有一個串流在跑，就拒絕新的
  if (streamClientActive) {
    Serial.println("Stream already active, reject new /stream client");
    client.println("HTTP/1.1 503 Service Unavailable");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Stream already in use");
    client.stop();
    return;
  }

  // 記錄這個 client，之後在 loop() 裡持續送 frame 給它
  streamClient = client;      // 複製 WiFiClient（底層會指向同一個 socket）
  streamClientActive = true;
  streamStartTime = millis();
  lastStreamFrameTime = 0;

  streamClient.println("HTTP/1.1 200 OK");
  streamClient.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
  streamClient.println("Access-Control-Allow-Origin: *");
  streamClient.println("Connection: close");
  streamClient.println();

  Serial.println("MJPEG stream started (non-blocking)");
}

/*************************************************
 *  處理每個 HTTP Client 的請求
 *************************************************/
void handleClient(WiFiClient &client) {
  // 簡單的 timeout，避免怪 client 卡住
  unsigned long timeout = millis() + 2000;
  while (!client.available()) {
    if (millis() > timeout) {
      client.stop();
      return;
    }
    delay(1);
  }

  // 讀第一行 HTTP request（例如：GET / HTTP/1.1）
  String req = client.readStringUntil('\r');
  client.readStringUntil('\n'); // 丟掉剩下的 \n

  // 把剩下的 headers 丟掉
  while (client.available()) client.read();

  Serial.print("Request: ");
  Serial.println(req);

  if (req.startsWith("GET /stream")) {
    sendMjpegStream(client);
  } else if (req.startsWith("GET /jpg")) {
    sendSingleJpeg(client);
  } else if (req.startsWith("GET / ") || req.startsWith("GET /index")) {
    sendIndexHtml(client);
    // sendIndexHtml 會自己寫完 header + body，這裡關連線
    client.stop();
  } else {
    // 其他路徑回 404
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("404 Not Found");
    client.stop();
  }
}

/*************************************************
 *  setup()
 *************************************************/
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println("ESP32-CAM Smart Plant Viewer + Blynk");

  // 啟用相機
  startCamera();

  // 連 Blynk（裡面會幫你連 WiFi）
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // 關閉 WiFi 省電睡眠，比較穩
  WiFi.setSleep(false);

  // 啟動 HTTP server
  server.begin();

  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Test URL: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/stream");
}

/*************************************************
 *  loop()
 *************************************************/
void loop() {
  Blynk.run();

  // 處理新的 HTTP client
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client");
    handleClient(client);   // 這裡不會卡住太久
    Serial.println("Client handled");
  }

  // 非阻塞 MJPEG：這裡每圈送一張 frame 給目前的 /stream client
  handleStreamFrame();
}
