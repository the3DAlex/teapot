#include <WiFi.h>
#include <ArduinoOTA.h>

const char* ssid = "TP-Link_9ED0";
const char* password = "55552332";
const uint attemps = 2;

void setup() 
{
  WiFi.begin(ssid, password);
  
  for (uint i = 0; i < attemps; i++) 
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      break;
    }
    delay(500);
  }

  ArduinoOTA.setHostname("esp32-ota");
  ArduinoOTA.begin();
}

void loop() 
{
  ArduinoOTA.handle();
}
