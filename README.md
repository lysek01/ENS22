# ENS22 Library

The `ENS22` library is designed for communicating with the ENS22 NB-IoT modem over UART using an ESP32. The library allows for establishing PDP contexts, setting network profiles, and supporting MQTT over NB-IoT connections. It simplifies the process of sending and receiving data over the modem while providing functions to handle network connections and data management efficiently.

## Description

The `ENS22` library is aimed at controlling the ENS22 modem, designed for specific communication protocols such as MQTT. It allows easy setup and configuration of the modem and facilitates communication over NB-IoT networks. 

- [ENS22-E - DevKit Board](https://www.digikey.cz/cs/products/detail/telit-cinterion/DEVKIT-ENS22-E-NB-IOT/12759396)
- [ENS22-E - AT Command set](https://eshop.sectron.cz/UserFiles/Image/Thales_ens22-e_atc_v02000.pdf)

## Installation

### Library Manager

The simplest way to install the library is using the Arduino Library Manager:  
Open Arduino IDE -> Sketch -> Include Library -> Manage Libraries, and search for `ENS22`, then click install.

### ZIP

Alternatively, download and save the [ENS22 library](https://github.com/lysek01/ENS22/archive/refs/heads/main.zip) in `.zip` format.  
Open Arduino IDE -> Sketch -> Include Library -> Add .ZIP Library... and select the downloaded file.

## Connection

### Hardware

The library was designed for use with the ESP32. It communicates with the ENS22 modem via a hardware serial interface. For proper communication, the modem should be connected using the RX, TX, RTS, and CTS pins.

### Schematic

The diagram below shows the connection between the ESP32 and the ENS22-E DevKit. This setup uses UART communication with RTS/CTS hardware flow control, ensuring smooth data transmission and reception.

![ESP32-ENS22](https://github.com/user-attachments/assets/13d10b5d-5d2a-483b-b278-651d8f2dd1a3)

When connecting to the ASC0_A pins on the ENS22-E DevKit, **make sure to remove the jumpers** from these pins first. The wires should be connected to the **left-side pins** (the ones next to the labels), which are used for communication.

The table outlines the specific connections between the ESP32 and the ENS22-E DevKit:

| **ESP32**      | **ENS22-E DevKit**     |
|----------------|------------------------|
| RX2 (GPIO 16)   | TXD0 (ASC0_A)          |
| TX2 (GPIO 17)   | RXD0 (ASC0_A)          |
| RTS (GPIO 18)  | RTS0 (ASC0_A)          |
| CTS (GPIO 19)  | CTS0 (ASC0_A)          |
| GND            | GND                    |

This provides the necessary UART communication for controlling the ENS22-E modem via the ESP32.

## Functions

### Constructor

- `ENS22(int hwSerial = 1)`
  - Initializes the `ENS22` object by specifying the hardware serial port to use for communication with the modem.
  - **Parameters**:
    - `hwSerial`: The hardware serial port used for communication (default is Serial1).

### Setup

- `begin(int rxPin, int txPin, int rtsPin, int ctsPin, long baudrate)`
  - Initializes communication with the modem.
  - **Parameters**:
    - `rxPin`: Receive pin for UART.
    - `txPin`: Transmit pin for UART.
    - `rtsPin`: RTS pin (ready to send).
    - `ctsPin`: CTS pin (clear to send).
    - `baudrate`: Communication baud rate (optional, default is 115200).
  
- `defPDP(const char *PDP_type, const char *APN)`
  - Defines the PDP (Packet Data Protocol) context.
  - **Parameters**:
    - `PDP_type`: PDP type, e.g., "IP, IPV6, IPV4V6, NONIP".
    - `APN`: Access Point Name.

### Connection

- `setProfile(const char *conType, const char *user, const char *passwd, const char *apn, const char *dns)`
  - Configures network profile.
  - **Parameters**:
    - `conType`: Connection type (e.g., GPRS0, GPRS6, none).
    - `user`: Network username.
    - `passwd`: Network password.
    - `apn`: Access Point Name.
    - `dns`: DNS server IP address.
    
### MQTT

- `connect(const char *broker, int port, const char *clientId, const char *user, const char *passwd, int keepAlive, unsigned int timeout)`
  - Establishes an MQTT connection to the broker.
  - **Parameters**:
    - `broker`: MQTT broker address.
    - `port`: Broker port (usually 1883).
    - `clientId`: Unique client identifier.
    - `user`: MQTT username (optional).
    - `passwd`: MQTT password (optional).
    - `keepAlive`: MQTT keep-alive interval (optional, default is 60 seconds).
    - `timeout`: Connection timeout duration in milliseconds (optional, default is 30 seconds).

- `disconnect()`
  - Disconnects from the MQTT broker.
  
#### Publish

- `publish(const char *topic, const char *message, int qos)`
  - Publishes a message to a topic.
  - **Parameters**:
    - `topic`: Topic to publish to.
    - `message`: The message content. The `const char*` can be replaced with `String`, `int`, or `float`.
    - `qos`: Quality of Service level (0, 1, or 2), (optional, default is 0).

#### Subscribe

- `subscribe(const char *topic, int qos)`
  - Subscribes to a specific topic.
  - **Parameters**:
    - `topic`: Topic to subscribe to.
    - `qos`: Quality of Service level (0, 1, or 2), (optional, default is 0).
  
  Once you subscribe to a topic, you can use the read() function to retrieve messages.

- `read(unsigned int size, unsigned int timeout)`
  - **Parameters**:
    - `size`: Maximum number of bytes to read (optional, default is 128).
    - `timeout`: Time to wait for a response in milliseconds (optional, default 30 is seconds).
  - Returns the message content as String.

### Utility Functions

- `getIP()`
  - Retrieves the current IP address assigned to the modem.

- `ping(const char *ip, int attempts, unsigned int timeout)`
  - Pings a specified IP address or domain to check network connectivity and returns the average response time in milliseconds.
  - **Parameters**:
    - `ip`: The target IP address or domain to ping (e.g., "8.8.8.8" or "example.com").
    - `attempts`: Number of ping attempts (optional, default is 4).
    - `timeout`: Timeout for each attempt in milliseconds (optional, default 2 is seconds).

### Modem Control

- `debug()`
  - Enables the debug mode for the ENS22 library. When debug mode is active, every AT command sent to the modem and the corresponding response will be printed to the serial monitor, allowing for easier troubleshooting and diagnostics during development.
    
- `restart()`
  - Restarts the modem.

- `reset()`
  - Resets the modem to factory settings.

## Example

The following example demonstrates how to set up the modem, set its parameters, and receive MQTT messages:

### Code

```cpp
#include <ENS22.h>

ENS22 modem(1);

void setup() {
  Serial.begin(9600);
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
    if (modem.subscribe("vsb/iot/heating")) {
      Serial.println("Topic subscribed successfully");
    }
  } else {
    Serial.println("Failed to connect to MQTT broker");
  }
}

void loop() {
  String response = modem.read();
  if (response.length() > 0) {
    Serial.print("Recieved message: ");
    Serial.println(response);
  }
}
