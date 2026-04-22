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
ESP8266 NodeMCU \
Ultrasonic Sensor \
Water Flow Sensor \ 
Relay Module \
OLED SSD1306 \
5V Power Supply 

## Nodes:  
### We have two nodes available.
1: Tank node \
2: Pump node
   
## Wiring: 
### WIRING — Tank Node (Minimal)
### Ultrasonic (JSN-SR04T)
Sensor	ESP8266 \
VCC	5V \
GND	GND \
TRIG	D5 \
ECHO	D6 
### Flow Sensor
Sensor	ESP8266 \
VCC	5V \
GND	GND \
SIGNAL	D7 

### Tank Node Circuit Daigram:
<img width="1022" height="855" alt="image" src="https://github.com/user-attachments/assets/84bbbf73-f582-47c3-baca-8c0bb469c970" />


### WIRING — Pump Node (Minimal)
### Relay
Relay	ESP8266 \
IN	   D1 \
VCC	5V \
GND	GND 
### Button
Button   ESP8266 \
One-Side   D6 \
Other      GND 
### OLED (U8g2 I2C)
OLED	ESP8266 \
SDA	D2 \
SCL	D3 \
VCC	3.3V \
GND	GND 

## Pump Node Circuit Daigram:
<img width="817" height="860" alt="image" src="https://github.com/user-attachments/assets/26bad651-c249-44fe-8ca9-3da887f2b205" />


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


# AWS IoT Core Configuration Guide
## Prerequisites
* AWS account 
* Access to AWS Console 

## Step 1: Create IoT Policy
* Click Create Policy 
* Enter: 
* Name: iot-full-access-policy 
* Add Policy Statement:  \
{ \
  "Effect": "Allow", \
  "Action": "iot:*", \
  "Resource": "*" \
} 
* Click Create

## Step 2: Create a Thing
* Go to AWS Console → IoT Core 
* Navigate to Manage → Things 
* Click Create Thing 
* Select Create single thing 
* Enter: 
* Thing Name: water-pump-node (or your choice)
* Select Auto-generate certificate 
* Download the following files:  
	* Device Certificate (certificate.pem.crt) 
	* Private Key (private.pem.key)  
	* Public Key 
* Finish

## Step 3: Get MQTT Endpoint
* Go to Settings 
* Copy: 
	* Endpoint (example: xxxx-ats.iot.ap-south-1.amazonaws.com) 

Before configuring the IoT MQTT application, generate the .p12 certificate with a password by running the command below. \
*openssl pkcs12 -export -out <Certificate_name>.p12 -inkey <key.pem file> -in <cert.pem file>*


## Configuration of IoT MQTT Android Application
### Prerequisites
Before starting, make sure you have the following:
* An Android MQTT client application installed on your mobile device 
* MQTT broker endpoint 
* Client ID  
* SSL/TLS certificates, if required by the broker 
* Active internet connection 
   
## Configuration Steps
1. Install the IoT MQTT Android Application (Tested In Andriod Phone so refering tio Andsiod OS.)   
Download and install a compatible MQTT client application from the Google Play Store.    
2. Open the Application.    
Launch the MQTT application on your Android device.    
3. Create a New Connection  
Select the option to add a new broker connection or create a new profile.   
4. Enter Broker Details   

*Provide the required broker information:*   
Connection Name: Any friendly name for identification.   
Server URL / Host: MQTT broker endpoint.   
Port: Usually 1883 for non-SSL or 8883 for SSL (Using SSL so 8883).   
Client ID: Unique client identifier.        
5. Configure Security Settings.  
	* If your broker uses secure communication:    
	* Enable SSL/TLS.      
	* Upload or select the required CA certificate.    
	* Add client certificate and private key if the application supports mutual TLS authentication.      
6. Enter Authentication Details.     
7. Save the Configuration.     
Save the broker profile after entering all required details.     
8. After successfully configuring the broker, add buttons or a toggle switch to manually turn the water pump ON/OFF. \            
	* Subscribe to the topic water/pump/control and use payloads such as on, off, reset, and auto for ON/OFF, RESET & AUTO control.     
    

    
    
   
        
Sidd
AWS Cloud Engineer + Maker
