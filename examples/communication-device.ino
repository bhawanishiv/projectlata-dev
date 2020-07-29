
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char *WIFI_SSID{"Your-Wi-Fi-SSID"};
const char *WIFI_PSWD{"Your-Wi-Fi-Password"};
const String SERVER_HOST{"http://projectlata.herokuapp.com/dev"};

void setup()
{
  Serial.begin(9600);
  connectWifi();
}

void loop()
{
  if (Serial.available() > 0)
  {
    String reqBody = Serial.readString();
    HTTPClient http;
    http.begin(SERVER_HOST);
    http.addHeader("Content-Type", "application/json");
    //Serial.println(postData);
    auto httpCode = http.POST(reqBody);
    //Serial.println(httpCode);
    String payload = http.getString();
    Serial.println(payload);
    Serial.flush();
    http.end();
  }
}

void connectWifi()
{
  WiFi.begin(WIFI_SSID, WIFI_PSWD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}
