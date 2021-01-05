#define D1 (1u)
#define D2 (2u)
#define PIN_LED_1 (11u)
#define PIN_LED_2 (12u)

class Relay {
  int relayPin;
  long onTime;
  int relayState;
  unsigned long lastOnTime;
  Relay* nextRelay;

  public:
  Relay(int pin, long on) {
    relayPin = pin;
    pinMode(relayPin, OUTPUT);
    onTime = on;
    lastOnTime = 0;
    relayState = LOW;
    digitalWrite(relayPin, relayState);
    nextRelay = NULL;
  }

  Relay(int pin, long on, Relay* next) {
    relayPin = pin;
    pinMode(relayPin, OUTPUT);
    onTime = on;
    lastOnTime = 0;
    relayState = LOW;
    digitalWrite(relayPin, relayState);
    nextRelay = next;
  }


  void Update() {
    unsigned long currentTime = millis();

    if((relayState == HIGH) && (currentTime - lastOnTime >= onTime)){
      relayState = LOW;
      lastOnTime = currentTime;
      digitalWrite(relayPin, relayState);
      if(nextRelay != NULL){
        nextRelay->RelayOn();
      }
    }
  }

  void RelayOn() {
    relayState = HIGH;
    lastOnTime = millis();
    digitalWrite(relayPin, relayState);
  }

  void RelayOff() {
    relayState = LOW;
    lastOnTime = millis();
    digitalWrite(relayPin, relayState);
  }

  int GetState() {
    return relayState;
  }

};

int ledState = LOW;
int lastButtonState = LOW;
int buttonState;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelayMs = 300; // debounce time in ms (0.3s)


Relay q2(PIN_LED_1,5000); // Debounced IGN indicator
Relay q3(D1,15000); // IGN Inhibit timer / Self power latch
Relay q1(LED_BUILTIN, 10000);

void setup() {
  // initialize pin mode
  pinMode(D2, INPUT);
}

int getButtonState(int buttonPin) {
  int reading = digitalRead(buttonPin);
  // read button
  if (reading != lastButtonState) {
    // reset debouncing timer
    lastDebounceTime = millis();
  }
  if((millis() - lastDebounceTime > debounceDelayMs)){
    buttonState = reading;
  }
  lastButtonState = reading; //update the last button state
  return buttonState;
}

void waitForSecondsWithConsideringDebouncingDelay( int seconds ) {
  unsigned long delayTime = (1000 - debounceDelayMs) * (unsigned long)seconds;
  delay(delayTime);
}

void loop() {
  int ign = getButtonState(D2); // get button state with debouncing from Digital Port 2

  if(ign == HIGH) {
    q1.RelayOn();
    q2.RelayOn();
    q3.RelayOn();
  }
  else {
    q1.Update();
    q2.RelayOff();
    q3.Update();
  }
}
