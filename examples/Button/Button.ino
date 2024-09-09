#include <ENS22.h>

ENS22 modem(1);

const int buttonPin = 2;
int buttonState = 0;
int lastButtonState = 0;
int currentState = 0;
int currentButtonState = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  // lastButtonState = digitalRead(buttonPin);

  delay(1000);

  // modem.debug(); // Allow debug mode
  modem.begin(16, 17, 18, 19, 115200);  // RX=16, TX=17, RTS=18, CTS=19

  if (modem.restart()) {
    Serial.println("Modem restarted successfully");
  }

  if (modem.defPDP("IP", "lpwa.vodafone.com")) {
    Serial.println("PDP context configured");
  } else {
    Serial.println("Failed to configure PDP context");
  }

  if (modem.setProfile("GPRS0", "easy", "connect", "lpwa.vodafone.com", "8.8.8.8")) {
    Serial.println("Profile set successfully");
  } else {
    Serial.println("Failed to set profile");
  }

  Serial.print("IP: ");
  Serial.println(modem.getIP());

  Serial.print("Ping AVG: ");
  Serial.println(modem.ping("google.com"));

  if (modem.connect("test.mosquitto.org", 1883, "clientID")) {
    Serial.println("Connected to MQTT broker");
  } else {
    Serial.println("Failed to connect to MQTT broker");
  }
}

void loop() {
  currentButtonState = digitalRead(buttonPin);

  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentButtonState != buttonState) {
      buttonState = currentButtonState;
      if (buttonState == LOW) {
        currentState = !currentState;

        if (currentState) {
          if (modem.publish("vsb/iot/button", "ON")) {
            Serial.println("Current state: ON");
          }
        } else {
          if (modem.publish("vsb/iot/button", "OFF")) {
            Serial.println("Current state: OFF");
          }
        }
        
      }
    }
  }
  lastButtonState = currentButtonState;
}
