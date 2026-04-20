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

## Tank Node Circuit Daigram:




Sidd
AWS Cloud Engineer + Maker