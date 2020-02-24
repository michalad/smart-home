// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable serial gateway
#define MY_GATEWAY_SERIAL

// Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#endif

// Enable inclusion mode
#define MY_INCLUSION_MODE_FEATURE
// Enable Inclusion mode button on gateway
#define MY_INCLUSION_BUTTON_FEATURE

// Inverses behavior of inclusion button (if using external pullup)
//#define MY_INCLUSION_BUTTON_EXTERNAL_PULLUP

// Set inclusion mode duration (in seconds)
#define MY_INCLUSION_MODE_DURATION 60
// Digital pin used for inclusion mode button
#define MY_INCLUSION_MODE_BUTTON_PIN  3

#include <SPI.h>
#include <MySensors.h>
#include <Bounce2.h>

// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE

#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay

#define BUTTON_PIN A1
#define BUTTON_PIN_2 A2

#define NUM_BUTTONS 24 // SET NUMBER OF BUTTONS
Bounce DEBOUNCERS[NUM_BUTTONS];
#define NUM_RELAYS 24 // SET NUMBER OF RELYS
const int RELAYS[NUM_RELAYS] = {23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, A15, A14, A13, A12, A11, A10, A9, A8};
const int BUTTONS[NUM_BUTTONS] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, A7, A6, A5, A4, A3, A2, A1, A0};
// rely -> button
# define NUM_OF_MAPPINGS 24
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
  {21, 21},
  {22, 22},
  {23, 23}
};

void before() {
  for (int relyNum = 0; relyNum < NUM_RELAYS; relyNum++) {
    digitalWrite(RELAYS[relyNum], RELAY_OFF);
    pinMode(RELAYS[relyNum], OUTPUT);
  }

  for (int relyIndex = 0; relyIndex < NUM_RELAYS; relyIndex++) {
    saveRelyState(relyIndex, loadState(relyIndex));
  }

}

void setup() {
  delay(1000);

  for (int buttonIndex = 0; buttonIndex < NUM_BUTTONS; buttonIndex++) {
    int curretnButtonPin = BUTTONS[buttonIndex];
    pinMode(curretnButtonPin, INPUT_PULLUP);
    Bounce *debouncer = new Bounce();
    debouncer->attach(curretnButtonPin);
    debouncer->interval(5);
    DEBOUNCERS[buttonIndex] = *debouncer;
  }
  //printMapping();
}

void printMapping() {
  for (int i = 0; i < NUM_OF_MAPPINGS; i++) {
    Serial.print("Przycisk: ");
    Serial.print(BUTTONS[mapping[i][0]]);
    Serial.print(" Przekaźnik: ");
    Serial.println(RELAYS[mapping[i][1]]);
  }
}

void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Relay", "2.1");
  for (int sensor = 0; sensor < NUM_RELAYS; sensor++) {
    present(sensor, S_LIGHT);
  }
}

void loop() {
  for (int buttonIndex = 0; buttonIndex < NUM_BUTTONS; buttonIndex++) {
    Bounce *debouncer = &DEBOUNCERS[buttonIndex];
    if (debouncer->update()) {
      int value = debouncer->read();
      if (value == LOW) {
        int assignedRelay = findAssignedRelay(buttonIndex);
        if (assignedRelay >= 0) {
          saveRelyState(assignedRelay, !loadState(assignedRelay));
          send(MyMessage(assignedRelay, V_LIGHT).set(loadState(assignedRelay)));
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
  saveState(relyNum, enabled);
}

void receive(const MyMessage &message) {
  if (message.isAck()) {
    Serial.println("This is an ack from gateway");
  }
  if (message.type == V_LIGHT) {
    saveRelyState(message.sensor, message.getBool());
  } else {
    Serial.print("Unknown message: ");
    Serial.println(message.type);
  }
}
