#include <ESP8266WiFi.h>
#include <espnow.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <U8g2lib.h>


bool blinkState = true;
unsigned long lastBlink = 0;

int waveOffset = 0;
unsigned long lastAnim = 0;

// ---------------- PINS ----------------
#define RELAY D1
#define BUTTON D6

// ---------------- OLED ----------------
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, D3, D2, U8X8_PIN_NONE);

// ---------------- DATA STRUCT ----------------
typedef struct {
  float level;
  int flow;
} Data;

Data incoming;

// ---------------- STATES ----------------
bool pumpState = false;
bool manualMode = false;
int noFlowCounter = 0;

// ---------------- AWS ----------------
const char* ssid = "<wifi_SSID>";
const char* password = "<WiFi pASSWORD>";
const char* mqtt_server = "<>";

WiFiClientSecure net;
PubSubClient client(net);

int lastLevelInt = -1;

// 🔐 Certificates (paste from AWS)
static const char* root_ca PROGMEM = R"EOF(-----BEGIN CERTIFICATE-----
<Certificate>
-----END CERTIFICATE-----)EOF";

static const char* client_cert PROGMEM = R"EOF(-----BEGIN CERTIFICATE-----
<Certificate>
-----END CERTIFICATE-----)EOF";

const char* private_key PROGMEM = R"EOF(-----BEGIN RSA PRIVATE KEY-----
<Certificate>
-----END RSA PRIVATE KEY-----)EOF";

BearSSL::X509List cert(root_ca);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(private_key);

#define LEVEL_LOW 20
#define LEVEL_HIGH 80

unsigned long pumpStartTime = 0;
bool waitingForFlow = false;
bool dryRunLock = false;
unsigned long lastDataReceived = 0;

bool lastPumpState = false;
String lastEvent = "NONE";

// ---------------- ESP-NOW CALLBACK ----------------
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incoming, incomingData, sizeof(incoming));
  lastDataReceived = millis();  // track last data time
}

// ---------------- AWS CONNECT ----------------
void connectAWS() {
  client.setServer(mqtt_server, 8883);

  while (!client.connected()) {
    Serial.print("Connecting to AWS...");
    if (client.connect("PumpNode")) {
      Serial.println("Connected!");
      client.subscribe("water/pump/control");
    } else {
      Serial.print("Failed: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// ---------------- OLED DRAW ----------------
void drawDisplay() {

  u8g2.clearBuffer();

  // ---------------- TITLE ----------------
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(28, 9, "WATER TANK");

  // ---------------- BORDER ----------------
  u8g2.drawFrame(1, 1, 128, 64);

  // ---------------- LEVEL BAR ----------------
  u8g2.drawStr(2, 20, "LEVEL");

  u8g2.drawFrame(2, 24, 124, 14);

  int fillWidth = map(incoming.level, 0, 100, 0, 120);

  // fill bar
  u8g2.drawBox(4, 26, fillWidth, 10);

  // animated water wave
  for (int i = 0; i < fillWidth; i += 8) {
    int y = (waveOffset + i) % 2;
    u8g2.drawPixel(4 + i, 26 + y);
  }

  // percentage text
  char buf[10];
  sprintf(buf, "%d%%", (int)incoming.level);
  u8g2.drawStr(95, 20, buf);

  // ---------------- PUMP STATUS ----------------
  u8g2.setFont(u8g2_font_5x8_tf);

  u8g2.drawStr(2, 48, "PUMP:");

  if (pumpState) {
    if (blinkState) u8g2.drawStr(36, 48, "ON ");
    else       u8g2.drawStr(36, 48, "   ");
  } else {
    u8g2.drawStr(36, 48, "OFF");
  }

  // FLOW
  u8g2.drawStr(64, 48, "W-FLOW: ");

  if (incoming.flow > 0) {
    if (blinkState) u8g2.drawStr(100, 48, "YES");
    else       u8g2.drawStr(98, 48, "   ");
  } else {
    u8g2.drawStr(100, 48, "NO ");
  }

  // ---------------- MODE ----------------
  u8g2.drawStr(2, 60, "MODE:");

  if (manualMode)
    u8g2.drawStr(42, 60, "MANUAL");
  else
    u8g2.drawStr(42, 60, "AUTO");

  // small rotating symbol when pump ON
  if (pumpState) {
    int x = 112;
    int y = 58;

    switch (waveOffset % 4) {
      case 0: u8g2.drawLine(x, y-2, x, y+2); break;
      case 1: u8g2.drawLine(x-2, y, x+2, y); break;
      case 2: u8g2.drawLine(x-2, y-2, x+2, y+2); break;
      case 3: u8g2.drawLine(x-2, y+2, x+2, y-2); break;
    }
  }

  u8g2.sendBuffer();
}

void callback(char* topic, byte* payload, unsigned int length) {

  String msg = "";

  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  msg.trim();

  Serial.print("MQTT Received: ");
  Serial.println(msg);

  // -------- JSON SUPPORT ----------
  if (msg.indexOf("reset") >= 0) {
    dryRunLock = false;
    waitingForFlow = false;
    manualMode = false;
    lastEvent = "NONE";
    Serial.println("RESET DONE");
    return;
  }

  if (msg.indexOf("on") >= 0) {
    digitalWrite(RELAY, HIGH);
    pumpState = true;
    manualMode = true;
    dryRunLock = false;
    waitingForFlow = false;
    Serial.println("PUMP MANUAL ON");
    return;
  }

  if (msg.indexOf("off") >= 0) {
    digitalWrite(RELAY, LOW);
    pumpState = false;
    manualMode = true;
    waitingForFlow = false;
    Serial.println("PUMP MANUAL OFF");
    return;
  }

  if (msg.indexOf("auto") >= 0) {
    manualMode = false;
    dryRunLock = false;
    waitingForFlow = false;
    Serial.println("AUTO MODE ENABLED");
    return;
  }
}

void publishEvent(String ev) {

  if (!client.connected()) return;

  String payload = "{";
  payload += "\"event\":\"" + ev + "\",";
  payload += "\"level\":" + String(incoming.level) + ",";
  payload += "\"flow\":" + String(incoming.flow) + ",";
  payload += "\"pump\":" + String(pumpState);
  payload += "}";

  client.publish("water/pump/status", payload.c_str());

  Serial.println("MQTT SENT:");
  Serial.println(payload);

  lastEvent = ev;
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(RELAY, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  digitalWrite(RELAY, LOW);

  // OLED
  u8g2.begin();

  // ESP-NOW
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  // WiFi + AWS
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.println(WiFi.localIP());
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync...");
  time_t now = time(nullptr);
  while (now < 100000) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nTime synced!");
  connectAWS();
  client.setCallback(callback);
}

// ---------------- LOOP ----------------
void loop() {

  if (!client.connected()) {
    connectAWS();
  }

  client.loop();

  unsigned long now = millis();
  String event = "NONE";

  // -------- MANUAL MODE ----------
  if (manualMode) {
    drawDisplay();
    delay(300);
    return;
  }

  // -------- NO DATA ----------
  if (now - lastDataReceived > 10000) {
    publishEvent("NO_DATA");
  }

  // -------- FULL TANK ----------
  if (incoming.level >= LEVEL_HIGH) {

    if (pumpState) {
      digitalWrite(RELAY, LOW);
      pumpState = false;
      publishEvent("TANK_FULL");
    }

    waitingForFlow = false;
    dryRunLock = false;
  }

  // -------- START ----------
  else if (!dryRunLock && incoming.level < LEVEL_LOW && !pumpState) {

    digitalWrite(RELAY, HIGH);
    pumpState = true;
    waitingForFlow = true;
    pumpStartTime = now;

    publishEvent("PUMP_ON");
  }

  // -------- FLOW CHECK ----------
  if (waitingForFlow) {

    if (now - pumpStartTime > 15000) {

      if (incoming.flow > 0) {
        waitingForFlow = false;
      } else {
        digitalWrite(RELAY, LOW);
        pumpState = false;
        waitingForFlow = false;
        dryRunLock = true;

        publishEvent("DRY_RUN");
      }
    }
  }

  drawDisplay();
  delay(100);
}