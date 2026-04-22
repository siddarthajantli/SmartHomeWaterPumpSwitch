#include <ESP8266WiFi.h>
#include <espnow.h>

#define TRIG D5
#define ECHO D6
#define FLOW_PIN D7

volatile int flowCount = 0;

// Pump Node MAC
uint8_t receiverMAC[] = {0x80, 0x00, 0xB1, 0x0A, 0xET, 0x22}; //The Pump node MAC address should be written exactly as shown.

typedef struct {
  float level;
  int flow;
} Data;

Data data;

float tankHeight = 100; // cm

void ICACHE_RAM_ATTR flowISR() {
  flowCount++;
}

float getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(FLOW_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), flowISR, RISING);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  float distance = getDistance();
  float level = tankHeight - distance;

  int flowRate = flowCount;
  flowCount = 0;

  data.level = level;
  data.flow = flowRate;

  esp_now_send(receiverMAC, (uint8_t*)&data, sizeof(data));

  Serial.print("Level: ");
  Serial.print(level);
  Serial.print(" | Flow: ");
  Serial.println(flowRate);

  delay(2000);
}