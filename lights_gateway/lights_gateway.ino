#include <MqttGateway.h>

byte mac[] = {
  0x02, 0x18, 0xC6, 0x50, 0x50, 0xD0
};
String DEVICE_NAME = "mqtt-gateway-2";

IPAddress ip(192, 168, 100, 170);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 0, 0);

InternetProps internetProps = { ip , gateway , subnet, mac };
MqttProps mqttProps  = {"192.168.100.151", 1883, "mqtt", "mqtt", DEVICE_NAME, 0, 1};

EthernetClient *ethernetClient;
PubSubClient *mqttClient;
MqttGateway *mqttGateway;

void setup() {
  Serial.begin(115200);
  
  LinkedList<MqttItem> *mqttItems = new LinkedList<MqttItem>();  
              // RELAY BUTTON
  mqttItems->add({23, 22, "/mqtt-gateway-2/light-1", ON_OFF});
  mqttItems->add({25, 24, "/mqtt-gateway-2/light-2", ON_OFF});
  mqttItems->add({27, 26, "/mqtt-gateway-2/light-3", ON_OFF});
  mqttItems->add({29, 28, "/mqtt-gateway-2/light-4", ON_OFF});
  mqttItems->add({31, 30, "/mqtt-gateway-2/light-5", ON_OFF});
  mqttItems->add({33, 32, "/mqtt-gateway-2/light-6", ON_OFF});
  mqttItems->add({35, 34, "/mqtt-gateway-2/light-7", ON_OFF});
  mqttItems->add({37, 36, "/mqtt-gateway-2/light-8", ON_OFF});
  mqttItems->add({39, 38, "/mqtt-gateway-2/light-9", ON_OFF});
  mqttItems->add({41, 40, "/mqtt-gateway-2/light-10", ON_OFF});
  mqttItems->add({43, 42, "/mqtt-gateway-2/light-11", ON_OFF});
  mqttItems->add({45, 44, "/mqtt-gateway-2/light-12", ON_OFF});
  mqttItems->add({17, 2, "/mqtt-gateway-2/light-13", ON_OFF});
  mqttItems->add({16, 3, "/mqtt-gateway-2/light-14", ON_OFF});
  mqttItems->add({15, 4, "/mqtt-gateway-2/light-15", ON_OFF});
  mqttItems->add({14, 5, "/mqtt-gateway-2/light-16", ON_OFF});
  mqttItems->add({A15, A7, "/mqtt-gateway-2/light-17", ON_OFF});
  mqttItems->add({A14, A6, "/mqtt-gateway-2/light-18", ON_OFF});
  mqttItems->add({A13, A5, "/mqtt-gateway-2/light-19", ON_OFF});
  mqttItems->add({A12, A4, "/mqtt-gateway-2/light-20", ON_OFF});
  mqttItems->add({A11, A3, "/mqtt-gateway-2/light-21", ON_OFF});
  mqttItems->add({A10, A2, "/mqtt-gateway-2/light-22", ON_OFF});
  mqttItems->add({A9, A1, "/mqtt-gateway-2/light-23", ON_OFF});
  mqttItems->add({A8, A0, "/mqtt-gateway-2/light-24", ON_OFF});

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
