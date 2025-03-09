#include "OpenMeteoForecastProvider.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

time_t parseTimeStringISO8601(const char* input) {
    if (strlen(input) >= 16 && input[10] == 'T') {
        struct tm tmm {0};
        if (sscanf(input, "%4d-%2d-%2dT%2d:%2d",
                &tmm.tm_year, &tmm.tm_mon, &tmm.tm_mday,
                &tmm.tm_hour, &tmm.tm_min) == 5) {
            tmm.tm_year -= 1900;
            tmm.tm_mon -= 1;
            return mktime(&tmm);
        }
    }
    return -1;
}

bool getOpenMeteoForecast(const String& request, Forecast& fcast) {
    WiFiClient wifi;
    HTTPClient http;
    bool result = false;
    if (http.begin(wifi, request)) {
        const auto httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK) {
            StaticJsonDocument<740> doc;
            auto jsonError = deserializeJson(doc, http.getString());
            if (jsonError.code() == DeserializationError::Code::Ok) {
                JsonObject cw = doc[F("current_weather")];
                fcast.time = parseTimeStringISO8601(cw[F("time")].as<const char*>());
                fcast.interval = cw[F("interval")].as<unsigned int>();
                fcast.temperature = cw[F("temperature")].as<float>();
                fcast.windspeed = cw[F("windspeed")].as<float>();
                fcast.winddirection =  cw[F("winddirection")].as<float>();
                fcast.weathercode = cw[F("weathercode")].as<unsigned int>();
                result = true;
            }
            else Serial.println(F("Forecast data deserialization failed: ") + String(jsonError.code()));
        }
        else Serial.println(F("Forecast request failed: ") + http.errorToString(httpCode));
    }
    else Serial.println(F("Forecast connection failed"));
    http.end();
    return result;
}

void OpenMeteoForecastProvider::init(float latitude, float longitude) {
    _request = F("http://api.open-meteo.com/v1/forecast?latitude={LAT}&longitude={LON}&current_weather=true");
    _request.replace(F("{LAT}"), String(latitude));
    _request.replace(F("{LON}"), String(longitude));
}

bool OpenMeteoForecastProvider::poll() {
    static unsigned long last {0}, tout {1000};
    static unsigned int errs {0};
    if (WiFi.isConnected() && !_request.isEmpty()) {
        if (millis() - last >= tout) {
            if (getOpenMeteoForecast(_request, _fcast)) {
                uint32_t secup = _fcast.secondsUpToDate();
                tout = min(3600U, max(60U, secup)) * 1000UL;
                last = millis();
                errs = 0U;
                if (_fcast.interval - secup > 10) Serial.printf(
                    PSTR("Forecast update interval desync too big, interval: %u, next update: %d\n"),
                        _fcast.interval, secup);
                return true;
            }
            tout = (errs < 5 ? 60U : 300U) * 1000UL;
            last = millis();
            errs += errs < UINT_MAX ? 1 : 0;
        }
    }
    return false;
}

bool OpenMeteoForecastProvider::hasForecast(bool upToDateOnly) const {
    if (_fcast.time > 0) {
        return upToDateOnly ? _fcast.secondsUpToDate() > 0U : true;
    }
    return false;
}

const Forecast& OpenMeteoForecastProvider::getForecast() const {
    return _fcast;
}
