# Project LATA - Dev
Library for **End devices** to communicate with Project LATA server.
Programs are  available for Wi-Fi module and any embedded base device.

## Example
Navigate to examples directory. The 'communication-device.ino' is for Communication devices and 'dev-example.ino' is for the targeted end device. 

Burn the program, preferrably using Arduino IDE.

# Dependency 
1. SoftwareSerial (for logging purpose)
2. ESP8266WiFi (only for Wi-Fi communication devices)
3. ESP8266HTTPClient (only for Wi-Fi communication devices)

## Note
1. For Serial Communiation (in the program), baud rate between Communication device and End device should be same.
2. Use your **instance Id** created using Web Application.
3. Mobile Application is under development.
