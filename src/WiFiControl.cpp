// #include <WiFi.h>
// #include <ESP8266WiFi.h>
#include <ESP8266NetBIOS.h>

// #include <ESPmDNS.h>
#include "WiFiControl.h"

const __FlashStringHelper* WiFiControl::currentStatusDescr() {
    switch (WiFi.status()) {
        case WL_IDLE_STATUS: return F("IDLE");
        case WL_NO_SSID_AVAIL: return F("NO_SSID");
        case WL_SCAN_COMPLETED: return F("SCAN_COMPLETED");
        case WL_CONNECTED: return F("CONNECTED");
        case WL_CONNECT_FAILED: return F("CONNECT_FAILED");
        case WL_CONNECTION_LOST: return F("CONNECTION_LOST");
        case WL_DISCONNECTED: return F("DISCONNECTED");
        case WL_NO_SHIELD: return F("NO_SHIELD");
        case WL_WRONG_PASSWORD: return F("WRONG_PASSWORD");
    }
    return F("UNKNOWN");
}

bool WiFiControl::initNetBIOS(const String& hostname) {
    if(hostname.length()) {
        return NBNS.begin(hostname.c_str());
        //return MDNS.begin(hostname.c_str()); // MDNS responder started
    }
    NBNS.end();
    //MDNS.end();
    return true;
}

bool WiFiControl::initNetwork(const IPAddress& address, const IPAddress& gateway,
            const IPAddress& subnet, const IPAddress& dns, const String& hostname) {
    return (!WiFi.isConnected() || WiFi.disconnect()) && WiFi.mode(WIFI_STA)
        && WiFi.config(address, gateway, subnet, dns)
        && WiFi.hostname(hostname) && initNetBIOS(hostname);
}

bool WiFiControl::beginConnect(const String& ssid, const String& password) {
    if (WiFi.disconnect() && ssid.length()) {
        const auto status = WiFi.begin(ssid, password);
        return status == WL_DISCONNECTED || status == WL_CONNECTED;
    }
    return false;
}

void logConnectionStatus(wl_status_t status, wl_status_t prev_status) {
    if (status == WL_CONNECTED) {
        #ifdef ESP32
        digitalWrite(LED_BUILTIN, LOW);
        #endif
        #ifdef ESP8266
        digitalWrite(LED_BUILTIN, HIGH);
        #endif
        Serial.print("Connected, IP: ");
        Serial.println(WiFi.localIP());
    }
    else {
        #ifdef ESP32
        digitalWrite(LED_BUILTIN, HIGH);
        #endif
        #ifdef ESP8266
        digitalWrite(LED_BUILTIN, LOW);
        #endif
        Serial.print("Disconnected, status: ");
        Serial.println(WiFiControl::currentStatusDescr());
    }
}

// weak, can be redefined in user code
void __attribute__((weak)) onConnectionUpdated(wl_status_t status, wl_status_t prev_status) {
    logConnectionStatus(status, prev_status);
}

// Return true if connection status changed
bool WiFiControl::watchConnection() {
    static auto prev_status = WL_DISCONNECTED;
    const auto status = WiFi.status();

    if (status != prev_status) {
        onConnectionUpdated(status, prev_status);
        prev_status = status;
        return true;
    }

    return false;
}
