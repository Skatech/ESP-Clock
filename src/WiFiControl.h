#ifndef WiFiControl_h
#define WiFiControl_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "DeviceConfig.h"

class WiFiControl {
public:
    // Current status description
    static const __FlashStringHelper* currentStatusDescr();
    // Initializes DNS responder, called automatically from initNetwork()
    static bool initNetBIOS(const String& hostname);
    // Initializes device as station
    static bool initNetwork(const IPAddress& address, const IPAddress& gateway,
            const IPAddress& subnet, const IPAddress& dns, const String& hostname);
    // Initiates connection procedure
    static bool beginConnect(const String& ssid, const String& password);
    // Must be called every loop iteration, returns true if connection status changed, otherwise false
    static bool watchConnection();
};

// Can be called from redefined onConnectionUpdated(bool connected)
void logConnectionStatus(wl_status_t status, wl_status_t prev_status);

#endif