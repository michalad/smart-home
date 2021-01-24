#include <MqttGateway.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
String DEVICE_NAME = "mqtt-gateway-1";

IPAddress ip(192, 168, 100, 171);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 0, 0);

InternetProps internetProps = { ip , gateway , subnet, mac };
MqttProps mqttProps  = {"192.168.100.151", 1883, "mqtt", "mqtt", DEVICE_NAME, 1, 0};

EthernetClient *ethernetClient;
PubSubClient *mqttClient;
MqttGateway *mqttGateway;

void setup() {
  Serial.begin(115200);
  
  LinkedList<MqttItem> *mqttItems = new LinkedList<MqttItem>();  
              // RELAY BUTTON
  mqttItems->add({22, A7, "/mqtt-gateway-1/heating-a8", ON_OFF});
  mqttItems->add({24, -1, "/mqtt-gateway-1/heating-a7", ON_OFF});
  mqttItems->add({26, -1, "/mqtt-gateway-1/heating-a6", ON_OFF});
  mqttItems->add({28, -1, "/mqtt-gateway-1/heating-a5", ON_OFF});
  mqttItems->add({30, -1, "/mqtt-gateway-1/heating-a4", ON_OFF});
  mqttItems->add({32, -1, "/mqtt-gateway-1/heating-a3", ON_OFF});
  mqttItems->add({34, -1, "/mqtt-gateway-1/heating-a2", ON_OFF});
  mqttItems->add({36, -1, "/mqtt-gateway-1/heating-a1", ON_OFF});

  mqttItems->add({37, -1, "/mqtt-gateway-1/heating-b8", ON_OFF});
  mqttItems->add({35, -1, "/mqtt-gateway-1/heating-b7", ON_OFF});
  mqttItems->add({33, -1, "/mqtt-gateway-1/heating-b6", ON_OFF});
  mqttItems->add({31, -1, "/mqtt-gateway-1/heating-b5", ON_OFF});
  mqttItems->add({29, -1, "/mqtt-gateway-1/heating-b4", ON_OFF});
  mqttItems->add({27, -1, "/mqtt-gateway-1/heating-b3", ON_OFF});
  mqttItems->add({25, -1, "/mqtt-gateway-1/heating-b2", ON_OFF});
  mqttItems->add({23, -1, "/mqtt-gateway-1/heating-b1", ON_OFF});

  mqttItems->add({41, -1, "/mqtt-gateway-1/heating-c8", ON_OFF});
  mqttItems->add({43, -1, "/mqtt-gateway-1/heating-c7", ON_OFF});
  mqttItems->add({45, -1, "/mqtt-gateway-1/heating-c6", ON_OFF});
  mqttItems->add({47, -1, "/mqtt-gateway-1/heating-c5", ON_OFF});
  mqttItems->add({46, -1, "/mqtt-gateway-1/heating-c4", ON_OFF});
  mqttItems->add({44, -1, "/mqtt-gateway-1/heating-c3", ON_OFF});
  mqttItems->add({42, -1, "/mqtt-gateway-1/heating-c2", ON_OFF});
  mqttItems->add({40, -1, "/mqtt-gateway-1/heating-c1", ON_OFF});



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
