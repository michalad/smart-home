#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "m_and_m"; // Enter your WiFi name
const char* password =  "BCUEGCSA"; // Enter WiFi password
const char* mqttServer = "192.168.0.23";
const int mqttPort = 1883;
const char* mqttUser = "mqtt";
const char* mqttPassword = "mqtt";

WiFiClient espClient;
PubSubClient client(espClient);

boolean isWifiNotConnected() {
  return WiFi.status() != WL_CONNECTED;
}

void connectToWifi() {
  while (isWifiNotConnected()) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  };
  Serial.println("Connected to the WiFi network");
}

void connectToMqtt() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("SmartItem1", mqttUser, mqttPassword )) {
      Serial.println("connected");
      client.subscribe("/switch/light1/cmd");
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  connectToWifi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  connectToMqtt();
}

void loop() {
  if (isWifiNotConnected()) {
    Serial.println("Connection to wifi lost, reconnecting...");
    connectToWifi();
  }
  if (!client.connected() && !isWifiNotConnected()) {
    Serial.println("Connection to mqtt lost, reconnecting...");
    connectToMqtt();
  }

  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");

}
