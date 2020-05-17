#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define D0 A14
#define D1 A15


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 175);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 0, 0);

const char* mqttServer = "192.168.0.23";
const int mqttPort = 1883;
const char* mqttUser = "mqtt";
const char* mqttPassword = "mqtt";

const char* TOPIC_1 = "/smart-item-1/socket-1/cmd";
const char* TOPIC_2 = "/smart-item-1/socket-2/cmd";

EthernetClient ethernetClient;
PubSubClient client(ethernetClient);
unsigned long lastRetry = millis();


boolean isInternetNotConnected() {
  return !ethernetClient.connected();
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  if (client.connect("SmartItem2", mqttUser, mqttPassword )) {
    Serial.println("connected");
    client.subscribe(TOPIC_1);
    client.subscribe(TOPIC_2);
  } else {
    Serial.print("failed with state ");
    Serial.println(client.state());
    delay(1000);
  }
}

void setup() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);

  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  delay(1000);

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  if (!isInternetNotConnected()) {
    connectToMqtt();
  }
}

boolean canReconnect() {
  return millis() - lastRetry > 60000;
}

void loop() {
  if (!client.connected()) {
    if (canReconnect()) {
      Serial.println("Connection to mqtt lost, reconnecting in 60 seconds...");
      connectToMqtt();
      lastRetry = millis();
    }
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

  if (!strncmp((char *)topic, TOPIC_1, 1000)) {
    handleRelay(D0, payload, length);
  }
  if (!strncmp((char *)topic, TOPIC_2, 1000)) {
    handleRelay(D1, payload, length);
  }

  Serial.println();
  Serial.println("-----------------------");

}

void handleRelay(int pin, byte* payload, unsigned int length ) {
  if (!strncmp((char *)payload, "on", length)) {
    digitalWrite(pin, HIGH);
  } else if (!strncmp((char *)payload, "off", length)) {
    digitalWrite(pin, LOW);
  }
}
