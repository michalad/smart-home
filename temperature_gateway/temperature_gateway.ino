#include <MqttGateway.h>

byte mac[] = {
  0xFE, 0xAD, 0xBE, 0xEF, 0xFE, 0xBE
};
String DEVICE_NAME = "mqtt-gateway-3";

IPAddress ip(192, 168, 100, 172);
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
  mqttItems->add({-1, 22, "/mqtt-gateway-3/temp-1", DS18B20});
  mqttItems->add({-1, 24, "/mqtt-gateway-3/temp-2", DS18B20});
  mqttItems->add({-1, 26, "/mqtt-gateway-3/temp-3", DS18B20});
  mqttItems->add({-1, 28, "/mqtt-gateway-3/temp-4", DS18B20});
  mqttItems->add({-1, 30, "/mqtt-gateway-3/temp-5", DS18B20});
  mqttItems->add({-1, 32, "/mqtt-gateway-3/temp-6", DS18B20});
  mqttItems->add({-1, 34, "/mqtt-gateway-3/temp-7", DS18B20});
  mqttItems->add({-1, 36, "/mqtt-gateway-3/temp-8", DS18B20});

  mqttItems->add({-1, 37, "/mqtt-gateway-3/temp-9", DS18B20});
  mqttItems->add({-1, 35, "/mqtt-gateway-3/temp-10", DS18B20});
  mqttItems->add({-1, 33, "/mqtt-gateway-3/temp-11", DS18B20});
  mqttItems->add({-1, 31, "/mqtt-gateway-3/temp-12", DS18B20});
  mqttItems->add({-1, 29, "/mqtt-gateway-3/temp-13", DS18B20});
  mqttItems->add({-1, 27, "/mqtt-gateway-3/temp-14", DS18B20});
  mqttItems->add({-1, 25, "/mqtt-gateway-3/temp-15", DS18B20});
  mqttItems->add({-1, 23, "/mqtt-gateway-3/temp-16", DS18B20});

  mqttItems->add({-1, A7, "/mqtt-gateway-3/temp-17", DS18B20});



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
