#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Bounce2.h>

/**

   BEGIN DEVICE SPECIFIC CONFIG

*/

#define RELAY_ON 0
#define RELAY_OFF 1
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
String DEVICE_NAME = "mqtt-gateway-1";
String RELAYS_PATTERN = "heating";


IPAddress ip(192, 168, 0, 175);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 0, 0);

#define NUM_BUTTONS 22 // SET NUMBER OF BUTTONS
Bounce DEBOUNCERS[NUM_BUTTONS];
#define NUM_RELAYS 22 // SET NUMBER OF RELYS
int state[NUM_RELAYS];
const int RELAYS[NUM_RELAYS] = {23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, A15, A14, A13, A12, A11, A10, A9, A8};
const int BUTTONS[NUM_BUTTONS] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, A7, A6, A5, A4, A3, A2, A1, A0};
// rely -> button
# define NUM_OF_MAPPINGS 22
const int mapping[NUM_OF_MAPPINGS][2] = { // MAP BUTTONS TO RELAYS
  {0, 0},
  {1, 1},
  {2, 2},
  {3, 3},
  {4, 4},
  {5, 5},
  {6, 6},
  {7, 7},
  {8, 8},
  {9, 9},
  {10, 10},
  {11, 11},
  {12, 12},
  {13, 13},
  {14, 14},
  {15, 15},
  {16, 16},
  {17, 17},
  {18, 18},
  {19, 19},
  {20, 20},
  {21, 21}
};

/**

   END DEVICE SPECIFIC CONFIG

*/

// MQTT CONFIG
const char* mqttServer = "192.168.0.23";
const int mqttPort = 1883;
const char* mqttUser = "mqtt";
const char* mqttPassword = "mqtt";
// END MQTT CONFIG

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
  if (client.connect((char*)DEVICE_NAME.c_str(), mqttUser, mqttPassword )) {
    for (int relyNum = 0; relyNum < NUM_RELAYS; relyNum++) {
      client.subscribe(toCharArray(getCmdTopic(relyNum)));
    }
    Serial.println("Connected" );
  } else {
    Serial.print("failed with state ");
    Serial.println(client.state());
    delay(1000);
  }
}

const char* toCharArray(String string) {
  return (char*)string.c_str();
}

void setup() {
  Serial.begin(115200);
  Serial.println("SETUP");
  initRelays();
  setupButtons();
  Ethernet.begin(mac, ip);
  delay(3000);

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  Serial.println("Checking is internet connected");
  connectToMqtt();
}

void setupButtons() {
  for (int buttonIndex = 0; buttonIndex < NUM_BUTTONS; buttonIndex++) {
    int curretnButtonPin = BUTTONS[buttonIndex];
    pinMode(curretnButtonPin, INPUT_PULLUP);
    Bounce *debouncer = new Bounce();
    debouncer->attach(curretnButtonPin);
    debouncer->interval(100);
    DEBOUNCERS[buttonIndex] = *debouncer;
  }
}

void initRelays() {
  for (int relyNum = 0; relyNum < NUM_RELAYS; relyNum++) {
    digitalWrite(RELAYS[relyNum], RELAY_OFF);
    state[relyNum] = RELAY_OFF;
    pinMode(RELAYS[relyNum], OUTPUT);
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
  buttonsLoop();
}

void buttonsLoop() {
  for (int buttonIndex = 0; buttonIndex < NUM_BUTTONS; buttonIndex++) {
    Bounce *debouncer = &DEBOUNCERS[buttonIndex];
    if (debouncer->update()) {
      int value = debouncer->read();
      if (value == LOW) {
        int assignedRelay = findAssignedRelay(buttonIndex);
        if (assignedRelay >= 0) {
          saveRelyState(assignedRelay, !state[assignedRelay]);
          send(assignedRelay, state[assignedRelay]);
        }
      }
    }
  }
}

int findAssignedRelay(const int button) {
  for (int i = 0; i < NUM_OF_MAPPINGS; i++) {
    if (mapping[i][0] == button) {
      int relyNumber = mapping[i][1];
      return relyNumber;
    }
  }
  return -1;
}

void saveRelyState(const int relyNum, const bool enabled) {
  digitalWrite(RELAYS[relyNum], enabled ? RELAY_ON : RELAY_OFF);
  state[relyNum] = enabled;
}

void send(int assignedRelay, int currentState) {
  client.publish(toCharArray(getStateTopic(assignedRelay)), toPayload(currentState), true);
}

const char* toPayload(const bool currentState) {
  return currentState ? "on" : "off";
}

const String getCmdTopic(int relayNUm) {
  return getTopic(relayNUm) + "/cmd";
}

const String getStateTopic(int relayNUm) {
  return getTopic(relayNUm) + "/state";
}

String getTopic(int relayNUm) {
  return "/" + DEVICE_NAME + "/" + RELAYS_PATTERN + "-" + relayNUm;
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  for (int relyNum = 0; relyNum < NUM_RELAYS; relyNum++) {
    if (String(topic) == getCmdTopic(relyNum)) {
      handleRelay(relyNum, payload, length);
    }
  }
  Serial.println("-----------------------");

}

void handleRelay(int relyNum, byte* payload, unsigned int length ) {
  if (!strncmp((char *)payload, "off", length)) {
    saveRelyState(relyNum, false);
  } else if (!strncmp((char *)payload, "on", length)) {
    saveRelyState(relyNum, true);
  }
}
