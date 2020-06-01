#include <MqttGateway.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
String DEVICE_NAME = "mqtt-gateway-1";

IPAddress ip(192, 168, 0, 175);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 0, 0);

InternetProps internetProps = { ip , gateway , subnet, mac };
MqttProps mqttProps  = {"192.168.0.23", 1883, "mqtt", "mqtt", DEVICE_NAME};

EthernetClient *ethernetClient;
PubSubClient *mqttClient;
MqttGateway *mqttGateway;

void setup() {
  Serial.begin(115200);
  LinkedList<MqttItem> *mqttItems = new LinkedList<MqttItem>();  
  mqttItems->add({A12, A4, "/mqtt-gateway-1/heating-14", ON_OFF});
  mqttItems->add({A13, A5, "/mqtt-gateway-1/heating-15", ON_OFF});
  mqttItems->add({A14, A6, "/mqtt-gateway-1/heating-16", ON_OFF});
  mqttItems->add({A15, A7, "/mqtt-gateway-1/heating-17", ON_OFF});

  ethernetClient = new EthernetClient();
  mqttClient = new PubSubClient(*ethernetClient);

  mqttGateway = new MqttGateway(internetProps, mqttProps, ethernetClient, mqttClient, mqttItems);
  mqttClient->setCallback(callback);
  mqttGateway->setup();
}

void loop() {
  mqttGateway->loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  mqttGateway->callback(topic, payload, length);
}
