#ifndef DeviceConfig_h
#define DeviceConfig_h

#include <Arduino.h>
#include <IPAddress.h>

class DeviceConfig {
public:
    String ssid;
    String password;
    String hostname;

    IPAddress address;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns;

    String timezone = "UTC0";
    String timeserver1;
    String timeserver2;
    String timeserver3;

    float latitude = 0.0f;
    float longitude = 0.0f;

    String displaycsv;

    bool load();
    bool save();
    bool exec(const String& command);
};

#endif
