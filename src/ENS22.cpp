#include "ENS22.h"

ENS22::ENS22(int hwSerial) : serial(&Serial1)
{
}

void ENS22::begin(int rxPin, int txPin, int rtsPin, int ctsPin, long baudrate)
{
    this->rxPin = rxPin;
    this->txPin = txPin;
    this->rtsPin = rtsPin;
    this->ctsPin = ctsPin;

    pinMode(rtsPin, OUTPUT);
    pinMode(ctsPin, INPUT);

    serial->begin(baudrate, SERIAL_8N1, rxPin, txPin);
    digitalWrite(rtsPin, LOW);

    if (waitForCTS(10000))
    {
        if (sendCommand("ATE0").indexOf("OK") != -1)
        {
            Serial.println("Modem connected successfully");
        }
    }
    else
    {
        Serial.println("No response from modem");
    }
}

bool ENS22::defPDP(const char *PDP_type, const char *APN, int mode, const char *opName)
{
    int attempts = 0;

    String response = sendCommand("AT+CGDCONT?", 5000);
    String searchTerm = "+CGDCONT: 0,\"" + String(PDP_type) + "\",\"" + String(APN) + "\"";

    if (response.indexOf(searchTerm) == -1)
    {

        while (attempts < 5)
        {
            if (sendCommand("AT+CGATT=0", 5000).indexOf("OK") != -1)
            {
                break;
            }
            attempts++;
        }

        if (attempts == 5)
            return false;

        if (sendCommand("AT+CGDCONT=0,\"" + String(PDP_type) + "\",\"" + String(APN) + "\",0,0,0", 5000).indexOf("OK") == -1)
            return false;

        if (!restart())
            return false;
    }

    attempts = 0;
    while (attempts < 10)
    {
        if (sendCommand("AT+COPS=" + String(mode) + ",2,\"" + String(opName) + "\",9", 10000).indexOf("OK") != -1)
        {
            return true;
        }
        attempts++;
    }
    return false;
}

bool ENS22::setProfile(const char *conType, const char *user, const char *passwd, const char *apn, const char *dns)
{
    if (sendCommand("AT^SICS=0,\"conType\",\"" + String(conType) + "\"").indexOf("OK") == -1)
        return false;

    if (sendCommand("AT^SICS=0,\"alphabet\",\"1\"").indexOf("OK") == -1)
        return false;

    if (sendCommand("AT^SICS=0,\"user\",\"" + String(user) + "\"").indexOf("OK") == -1)
        return false;

    if (sendCommand("AT^SICS=0,\"passwd\",\"" + String(passwd) + "\"").indexOf("OK") == -1)
        return false;

    if (sendCommand("AT^SICS=0,\"apn\",\"" + String(apn) + "\"").indexOf("OK") == -1)
        return false;

    if (sendCommand("AT^SICS=0,\"dns1\",\"" + String(dns) + "\"").indexOf("OK") == -1)
        return false;

    int attempts = 0;
    while (attempts < 15)
    {
        String ip = getIP();
        if (ip != "No IP")
        {
            return true;
        }
        attempts++;
    }

    return false;
}

bool ENS22::connect(const char *broker, int port, const char *clientId, const char *user, const char *passwd, int keepAlive, unsigned int timeout)
{
    if (sendCommand("AT^SISS=0,\"srvType\",\"Mqtt\"").indexOf("OK") == -1)
        return false;

    if (sendCommand("AT^SISS=0,\"alphabet\",\"1\"").indexOf("OK") == -1)
        return false;

    if (sendCommand("AT^SISS=0,\"address\",\"mqtt://" + String(broker) + ":" + String(port) + "\"").indexOf("OK") == -1)
        return false;

    if (user != nullptr && strlen(user) > 0)
    {
        if (sendCommand("AT^SISS=0,\"user\",\"" + String(user) + "\"").indexOf("OK") == -1)
            return false;
    }

    if (passwd != nullptr && strlen(passwd) > 0)
    {
        if (sendCommand("AT^SISS=0,\"passwd\",\"" + String(passwd) + "\"").indexOf("OK") == -1)
            return false;
    }
    if (sendCommand("AT^SISS=0,\"clientId\",\"" + String(clientId) + "\"").indexOf("OK") == -1)
        return false;

    if (sendCommand("AT^SISS=0,\"keepAlive\",\"" + String(keepAlive) + "\"").indexOf("OK") == -1)
        return false;

    if (sendCommand("AT^SISS=0,\"conID\",\"0\"").indexOf("OK") == -1)
        return false;

    String response = sendCommand("AT^SISO=0,2", timeout, "Connection accepted");

    if (response.indexOf("Connection accepted") != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ENS22::publish(const char *topic, const char *message, int qos)
{
    sendCommand("AT^SISD=0,setParam,cmd,\"publish\"");
    if (String(topic) != lastPubTopic || qos != lastPubQos)
    {
        sendCommand("AT^SISD=0,setParam,topic,\"" + String(topic) + "\"");
        sendCommand("AT^SISD=0,setParam,qos,\"" + String(qos) + "\"");
        lastPubTopic = topic;
        lastPubQos = qos;
    }
    sendCommand("AT^SISD=0,setParam,hcContent,\"" + String(message) + "\"");
    return sendCommand("AT^SISU=0").indexOf("OK") != -1;
}

bool ENS22::publish(const char *topic, const String &message, int qos)
{
    return publish(topic, message.c_str(), qos);
}

bool ENS22::publish(const char *topic, int message, int qos)
{
    return publish(topic, String(message).c_str(), qos);
}

bool ENS22::publish(const char *topic, float message, int qos)
{
    return publish(topic, String(message, 2).c_str(), qos);
}

bool ENS22::subscribe(const char *topic, int qos)
{
    sendCommand("AT^SISD=0,setParam,cmd,\"subscribe\"");
    if (String(topic) != lastSubTopic || qos != lastSubQos)
    {
        sendCommand("AT^SISD=0,setParam,topicFilter,\"" + String(topic) + "\"");
        sendCommand("AT^SISD=0,setParam,topicQos,\"" + String(qos) + "\"");
        lastSubTopic = topic;
        lastSubQos = qos;
    }
    return sendCommand("AT^SISU=0").indexOf("OK") != -1;
}

bool ENS22::disconnect()
{
    return sendCommand("AT^SISC=0").indexOf("OK") != -1;
}

bool ENS22::restart()
{
    if (sendCommand("AT+NRB", 15000, "^SYSSTART").indexOf("^SYSSTART") != -1)
    {
        return sendCommand("ATE0").indexOf("OK") != -1;
    }

    return false;
}

bool ENS22::reset()
{
    if (sendCommand("AT&F0").indexOf("OK") != -1)
    {
        return restart();
    }

    return false;
}

void ENS22::debug()
{
    debugState = true;
}

void ENS22::manual(unsigned int timeout)
{
    if (Serial.available())
    {
        String command = Serial.readStringUntil('\n');

        if (command.length() > 0)
        {
            sendCommand(command, timeout);
        }
    }
}

int ENS22::ping(const char *ip, int attempts, unsigned int timeout)
{
    String command = "AT^SISX=\"Ping\",0,\"" + String(ip) + "\"," + String(attempts) + "," + String(timeout);
    String response = sendCommand(command, (attempts * timeout) + 2 * timeout, "\nOK");
    String searchTerm = "^SISX: \"Ping\",3,0,";
    int avgStart = response.lastIndexOf(searchTerm);

    if (avgStart != -1)
    {
        String avgLine = response.substring(avgStart + searchTerm.length());
        int avgPing = avgLine.substring(avgLine.lastIndexOf(",") + 1).toInt();
        return avgPing;
    }

    return -1;
}

String ENS22::getIP()
{
    String response = sendCommand("AT+CGPADDR", 2000, "\"OK");
    int startIndex = response.indexOf("\"");
    int endIndex = response.indexOf("\"", startIndex + 1);

    if (startIndex != -1 && endIndex != -1)
    {
        response = response.substring(startIndex + 1, endIndex);
        response.trim();
        return response;
    }

    return "No IP";
}

String ENS22::sendCommand(const String &command, unsigned int timeout, const String &expectedResponse, bool advanced)
{
    if (!waitForCTS())
        return "CTS not ready";

    while (serial->available())
    {
        serial->read();
    }

    serial->print(command + "\r\n");

    if (debugState)
    {
        Serial.print("--> Command: ");
        Serial.println(command);
    }

    String response = readResponse(expectedResponse, timeout, advanced);

    if (debugState)
    {
        Serial.print("--> Response: ");
        Serial.println(response);
    }

    return response;
}

String ENS22::readResponse(const String &expectedResponse, unsigned int timeout, bool advanced)
{
    String response = "";
    unsigned long startTime = millis();

    while (millis() - startTime < timeout)
    {
        if (serial->available())
        {
            char c = char(serial->read());
            if (c != '\0')
            {
                response += c;
            }

            if (!advanced && response.indexOf(expectedResponse) != -1)
            {
                delay(100);
                break;
            }
        }
    }

    response.trim();
    return response;
}

bool ENS22::waitForCTS(unsigned int timeout)
{
    unsigned long startTime = millis();
    while (digitalRead(ctsPin) != LOW)
    {
        if (millis() - startTime > timeout)
        {
            Serial.println("CTS not ready.");
            return false;
        }
    }
    return true;
}

String ENS22::read(unsigned int size, unsigned int timeout)
{
    if (readResponse("^SIS: 0,", timeout).indexOf("^SIS: 0,") != -1)
    {
        String response = sendCommand("AT^SISR=0," + String(size));

        int startIndex = response.indexOf("\r\n");
        int endIndex = response.indexOf("OK", startIndex + 1);

        if (startIndex != -1 && endIndex != -1 && endIndex > startIndex)
        {
            response = response.substring(startIndex + 1, endIndex);
            response.replace("\0", "");
            response.replace("\r", "");
            response.replace("\n", "");
            response.trim();
            if (response.length() > 0)
            {
                return response;
            }
        }
    }
}