#include <Arduino.h>
#include <LittleFS.h>
#include <Adafruit_NeoPixel.h> 
#include "DeviceConfig.h"
#include "WiFiControl.h"
#include "Annotate.h"
#include "webui.h"

#include <DateTime.h>
#include <SNTPControl.h>
#include <OpenMeteoForecastProvider.h>
#include <RadialClockDisplay.h>
#include <RadialWeatherDisplay.h>

#define LED_STRIP_PIN       D5     // GPIO14
#define LED_STRIP_PIXELS    60

DeviceConfig cfg;
OpenMeteoForecastProvider forecast;
Adafruit_NeoPixel strip(LED_STRIP_PIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);
RadialClockDisplay display(strip, true);
RadialWeatherDisplay displayw(strip, forecast, false);

String getDeviceInfo() {
    DateTime time = DateTime::now();
    String json = "{\"time\":{TME},\"forecast\":{FCS}}";

    json.replace("{TME}", String(time.toSecondsSinceEpoch()));

    json.replace("{FCS}", forecast.hasForecast(true)
        ? forecast.getForecast().toJSONString() : F("{}"));

    return json;
}

// bool containsRecord(const String& input, const __FlashStringHelper* record, char separator = ',') {
//     int index = input.indexOf(record);
//     size_t nextp = index + strlen_P((const char *)record);
//     return index >= 0 && (index == 0 ? true : separator == input.charAt(index - 1))
//         && (nextp == input.length() ? true : separator == input.charAt(nextp));
// }

// String getDeviceInfo(const String& types) {
//     DateTime time = DateTime::now();
//     String json = "{\"time\":{TME},\"forecast\":{FCS}}";

//     json.replace("{TME}", containsRecord(types, F("time"))
//         ? String(time.toSecondsSinceEpoch()) : F(""));

//     json.replace("{FCS}", types.indexOf(F("forecast")) >= 0 && forecast.hasForecast(true)
//         ? forecast.getForecast().toJSONString() : F("{}"));

//     return json;
// }

void updateDisplay() {
    if (!displayw.poll()) {
        display.poll(WiFi.isConnected());
    }
}

bool initNetwork() {
    return WiFiControl::initNetwork(cfg.address,
        cfg.gateway, cfg.subnet, cfg.dns, cfg.hostname);
}

bool beginConnect() {
    return WiFiControl::beginConnect(cfg.ssid, cfg.password);
}

bool initSNTP() {
    return SNTPControl::setup(cfg.timezone,
        cfg.timeserver1, cfg.timeserver2, cfg.timeserver3);
}

void onSerialCommand(const String& input) {
    Serial.println(F(""));
    if (cfg.exec(input)) {
    }
    else if (input == F("device-status")) {
        Serial.println(WiFi.isConnected()
            ? "Connected, IP: " + WiFi.localIP().toString() + ", MAC: " + WiFi.macAddress()
            : "Disconnected (" + String(WiFiControl::currentStatusDescr()) + "), MAC: " + WiFi.macAddress());
        Serial.printf_P(PSTR("System time: %s (%s UTC)\n"), DateTime::now().toString().c_str(),
            DateTime::now().toDateTimeUTC().toTimeString().c_str());
        Serial.printf_P(PSTR("SNTP service: %s\n"), SNTPControl::isEnabled() ? "ON" : "OFF");
        Serial.printf_P(PSTR("Uptime: %ldms\n"), millis());
        Serial.printf_P(PSTR("Free memory: %d bytes (%.0f%%), frag: %d%%\n"),
                ESP.getFreeHeap(), ESP.getFreeHeap() * 100.0f / (80 * 1024),
                ESP.getHeapFragmentation());
        Serial.printf_P(PSTR("Firmware version: %.1f\n"), FIRMWARE_VERSION);
    }
    else if (input == F("device-restart")) {
        Serial.println(F("Restarting..."));
        delay(500);
        ESP.restart();
    }
    else if (input.startsWith(F("device-time="))) {
        DateTime newtime = DateTime::parseISOString(input.substring(input.indexOf('=') + 1).c_str());
        DateTime::setSystemTime(newtime);
    }
    else if (input == F("network-list")) {
        const int8_t count = WiFi.scanNetworks();
        for (int8_t n = 0; n < count; n++) {
            Serial.println(WiFi.SSID(n));
        }
    }
    else if (input == F("network-restart")) {
        Annotate(F("Initializing network... "))(initNetwork());
    }
    else if (input == F("network-reconnect")) {
        Annotate(F("Connecting... "))(beginConnect());
    }
    else if (input == F("sntp-restart")) {
        Annotate(F("Time service starting... "))(initSNTP());
    }
    else if (input == F("sntp-stop")) {
        Annotate(F("Time service stopping... "))(true);
        SNTPControl::stop();
    }
    else if (input == F("display-test")) {
        Annotate(F("Testing display... "))(display.test());
    }
    else if (input == F("display-info")) {
        Serial.print(F("WS2812 strip, ") + String(strip.numPixels()));
        Serial.println(F(" leds, color scheme: ") + display.getColorScheme());
    }
    else if (input.startsWith(F("display-csv="))) {
        String csv = input.substring(input.indexOf('=') + 1);
        if (Annotate(F("Changing display color scheme... "))(display.trySetColorScheme(
                csv.equalsIgnoreCase(F("default")) ? String(DEFAULT_COLOR_SCHEME) : csv)))
            cfg.displaycsv = csv;
    }
    else if (input.startsWith(F("forecast"))) {
        if (forecast.hasForecast(false)) {
            Serial.print(forecast.getForecast().toString());
            uint32 validSec = forecast.getForecast().secondsUpToDate();
            if (validSec > 0U) {
                Serial.printf_P(PSTR(" (valid up to %s, %us)\n"),
                    DateTime(DateTime::now() + validSec).toTimeString().c_str(), validSec);
            }
            else Serial.println(F(" (OUTDATED)"));
        }
        else Serial.println(F("No forecast available"));
    }
    else if (input.startsWith(F("t="))) {
        float t = input.substring(input.indexOf('=') + 1).toFloat();
        const_cast<Forecast&>(forecast.getForecast()).temperature = t;
        Serial.printf_P(PSTR("Set forecast temperature to: %f\n"),
            forecast.getForecast().temperature);
    }
    else Serial.println(F(R""""(
Commands:
    config-show, config-save, option?, option=VALUE,
    device-status, device-restart, device-time=YYYYMMDDTHHMMSS
    network-list, network-restart, network-reconnect,
    sntp-restart, sntp-stop
    display-test, display-info, display-csv=default or BB,TTTTTT,SSSSSS,MMMMMM,DDDDDD,NNNNNN,
    forecast)""""));
    Serial.println();
}

void onConnectionUpdated(wl_status_t status, wl_status_t prev_status) {
    // if(status == WL_CONNECTED) {
    //     updateDisplayText("http://" + cfg.hostname, "IP:" + cfg.address.toString());
    // }
    // else {
    //     updateDisplayText("http://" + cfg.hostname, WiFiControl::currentStatusDescr());
    // }
    logConnectionStatus(status, prev_status);
}


// RAM:   [====      ]  40.8% (used 33436 bytes from 81920 bytes) == 80KiB
// Flash: [====      ]  36.1% (used 376655 bytes from 1044464 bytes)


void initPeripherials() {
    Serial.print(F("  display color scheme... "));
    if (!cfg.displaycsv.isEmpty()) {
        Annotate()(display.trySetColorScheme(cfg.displaycsv));
    }
    else Serial.println(F("DEFAULT"));
    Annotate(F("  display tests... "))(display.test());
}

void setup(void) {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.begin(SERIAL_SPEED);

    Serial.println(F("\n\n\nESP-CLock Initializing:"));
    Annotate(F("  filesystem... ")).makeCritical()(LittleFS.begin());
    Annotate(F("  configuration... "))(cfg.load());

    Annotate(F("  initializing network... "))(initNetwork());
    Annotate(F("  connecting... "))(beginConnect());

    initWebUI();
    Annotate(F("  time service... "))(initSNTP());
    Annotate(F("  forecast service... "))(true);
    forecast.init(cfg.latitude, cfg.longitude);

    initPeripherials(); // clock display
}

void loop(void) {
    WiFiControl::watchConnection();
    if (WiFi.isConnected()) {
        loopWebUI();
        if (forecast.poll()) {
            // Serial.print(DateTime::now().toTimeString());
            // Serial.print(" Forecast updated: ");
            // Serial.println(forecast.getForecast().toString());
        }
    }
    updateDisplay();
}