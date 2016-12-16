#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

char ssid[] = "12connect";
int wifiStatus = WL_IDLE_STATUS;
WiFiClient client;
// mqtt server settings
// To not use dns, we set a static IP
//char server[] = "mqtt.fhict.nl";
IPAddress server(145, 85, 4, 91);
PubSubClient mqttClient(client);
// Get these at https://apps.fhict.nl/selfservice
char mqttUser[] = "i339322_timojansen";
char mqttPass[] = "Xjr1MGsdYEKtJ5";
// This is the client Id that will be used, the mac will be append!
String mqttClientId = "arduino-";
// This is the start of the topic that will be used. ClientID and button1 will be append
String subscribeTopic = "private/i339322_timojansen/#";

void callback(char* topic, byte* payload, unsigned int len) {
  // Wrtie the topic to the debugger (or check it...)
  Serial.println("MQTT topic: "+String(topic));
  // Convert the payload to a string.
  String payloadString = String((char*)payload);
  // Write the payload to the debugger (or do something with it)
  Serial.println(payloadString);
  // Currently this is mainly a switch, but you could do something with the payload.
  // The led will be switched on/off on any received message.
  // Like if(payloadString == "test") { .... }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Serial.println("Arduino C# chat starting..");

  uint8_t mac[6];
  WiFi.macAddress(mac);
  mqttClientId += macToStr(mac);
  Serial.println("Client ID: " + mqttClientId);
  connectWifi();
  if ( wifiStatus != WL_CONNECTED) {
    Serial.println("Could not connect to Wifi");
    while (true);
  }
   else {
    Serial.println("Connected to Wifi");
    delay(5000);
    Serial.println(WiFi.localIP());
  }
  mqttClient.setServer(server, 1883);
  // Set the callback, will be called when a message is received.
  mqttClient.setCallback(callback);
  //Try to connect to MQTT
  connectMqtt();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (wifiStatus != WL_CONNECTED)
    connectWifi();

  while (!mqttClient.connected())
    connectMqtt();

  mqttClient.loop();
}

// Call to (re)connect to wifi.
void connectWifi() {
  Serial.println("Initializing Wifi...");
  wifiStatus = WiFi.begin(ssid);
}

// Call to connect to the MQTT server
void connectMqtt(){
  if(!mqttClient.connect((char*) mqttClientId.c_str(),mqttUser,mqttPass))
  {
    Serial.println("ERROR: Cannot connect to MQTT server! Error code: " +String(mqttClient.state()));
  }
  else
  {
    Serial.println("Connected to MQTT");
    if(mqttClient.subscribe((char *)subscribeTopic.c_str())){
      Serial.println("Subscribed to topic: "+ subscribeTopic);
    }
  }
}

// Convert the MAC to a string
String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    // In case you want : in the mac address.
    //    if (i < 5)
    //      result += ':';
  }
  return result;
}
