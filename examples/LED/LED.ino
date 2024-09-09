#include <ENS22.h>

ENS22 modem(1);
const int ledPin = 2;

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, LOW);

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
    if (modem.subscribe("vsb/iot/led")) {
      Serial.println("Topic subscribed successfully");
    }
  } else {
    Serial.println("Failed to connect to MQTT broker");
  }
}

void loop() {
  String response = modem.read();
  if (response.length() > 0) {
    if (response == "ON") {
      digitalWrite(ledPin, HIGH);  // Zapnutí LED
      Serial.println("LED is ON");
    } else if (response == "OFF") {
      digitalWrite(ledPin, LOW);  // Vypnutí LED
      Serial.println("LED is OFF");
    }
  }
}