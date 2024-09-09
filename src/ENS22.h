#ifndef ENS22_H
#define ENS22_H

#include <Arduino.h>
#include <HardwareSerial.h>

class ENS22
{
public:
    ENS22(int hwSerial = 1);
    void begin(int rxPin, int txPin, int rtsPin, int ctsPin, long baudrate = 115200);

    bool defPDP(const char *PDP_type, const char *APN);

    bool setProfile(const char *conType, const char *user, const char *passwd, const char *apn, const char *dns);

    bool connect(const char *broker, int port, const char *clientId, const char *user = "", const char *passwd = "", int keepAlive = 60, unsigned int timeout = 30000);

    bool publish(const char *topic, const char *message, int qos = 0);

    bool publish(const char *topic, const String &message, int qos = 0);

    bool publish(const char *topic, int message, int qos = 0);

    bool publish(const char *topic, float message, int qos = 0);

    bool subscribe(const char *topic, int qos = 0);

    bool disconnect();

    bool restart();
    bool reset();

    void debug();

    int ping(const char *ip, int attempts = 4, unsigned int timeout = 2000);

    String getIP();

    String read(unsigned int size = 128, unsigned int timeout = 30000);

private:
    HardwareSerial *serial;
    int rxPin, txPin, rtsPin, ctsPin;
    bool debugState = false;
    String lastPubTopic;
    int lastPubQos;
    String lastSubTopic;
    int lastSubQos;

    String sendCommand(const String &command, unsigned int timeout = 2000, const String &expectedResponse = "OK", bool advanced = false);
    String readResponse(const String &expectedResponse = "OK", unsigned int timeout = 2000, bool advanced = false);
    bool waitForCTS(int unsigned timeout = 1000);
};

#endif