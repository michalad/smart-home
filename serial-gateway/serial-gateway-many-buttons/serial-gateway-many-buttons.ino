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

#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay

#define BUTTON_PIN A1
#define BUTTON_PIN_2 A2

#define NUM_BUTTONS 6 // SET NUMBER OF BUTTONS
Bounce DEBOUNCERS[NUM_BUTTONS];
#define NUM_RELAYS 9 // SET NUMBER OF RELYS
const int RELAYS[NUM_RELAYS] = {4, 5, 6, 7, 8, 9, 10, 11, 12};
const int BUTTONS[NUM_BUTTONS] = {A1, A2, A3, A4, A5, 2};
// rely -> button
# define NUM_OF_MAPPINGS 6
const int mapping[NUM_OF_MAPPINGS][2] = { // MAP BUTTONS TO RELAYS
  {0, 1},
  {1, 1},
  {2, 1},
  {3, 2},
  {4, 2},
  {5, 3}
};

void before() {
  for (int relyNum = 0; relyNum < NUM_RELAYS; relyNum++) {
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
