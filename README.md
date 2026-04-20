# Smart Home Water Pump Switch
Automatic water tank motor controller using ESP8266, ESP-NOW, AWS IoT Core, OLED display, ultrasonic sensor and flow sensor.

## Features:
* Automatic motor ON when tank low 
* Automatic motor OFF when tank full 
* Dry run protection 
* Water flow detection 
* AWS IoT notifications 
* Mobile MQTT manual ON/OFF/RESET 
* OLED live tank UI 
* ESP-NOW wireless tank sensor node

## Hardware Used: 
ESP8266 NodeMCU
Ultrasonic Sensor
Water Flow Sensor
Relay Module
OLED SSD1306
5V Power Supply

## Nodes:  
### We have two nodes available.
1: Tank node \
2: Pump node
   
## Wiring: 
### WIRING — Tank Node (Minimal)
### Ultrasonic (JSN-SR04T)
Sensor	ESP8266
VCC	5V
GND	GND
TRIG	D5
ECHO	D6 
### Flow Sensor
Sensor	ESP8266
VCC	5V
GND	GND
SIGNAL	D7

### Tank Node Circuit Daigram:

<img width="1536" height="1024" alt="ChatGPT Image Apr 20, 2026, 01_56_27 PM" src="https://github.com/user-attachments/assets/422128e9-c47b-4b24-88d1-5b95cfa7f2de" />

### WIRING — Pump Node (Minimal)
### Relay
Relay	ESP8266
IN	   D1
VCC	5V
GND	GND
### Button
Button   ESP8266
One-Side   D6
Other      GND
### OLED (U8g2 I2C)
OLED	ESP8266
SDA	D2
SCL	D3
VCC	3.3V
GND	GND

## Pump Node Circuit Daigram:
<img width="1536" height="1024" alt="ChatGPT Image Apr 20, 2026, 02_13_08 PM" src="https://github.com/user-attachments/assets/fc63343a-34f7-4227-b96a-f2cce1be5675" />


## AWS IoT Topics
Subscribe: \
water/pump/control \
Publish: \
water/pump/status

## Commands:
{"cmd":"on"} \
{"cmd":"off"} \
{"cmd":"reset"} \
{"cmd":"auto"} 

## Future Improvements
* Telegram Alerts
* Leak Detection
* Current Sensor
* Usage Analytics






Sidd
AWS Cloud Engineer + Maker
