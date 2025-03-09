#ifndef Forecast_h
#define Forecast_h

#include <Arduino.h>

class Forecast {
public:
    time_t time;  // sampling time
    unsigned int interval; // server update interval
    float temperature; // °C
    float windspeed; // km/h
    float winddirection; // getWorldSide()
    unsigned int weathercode; // getWeatherCodeDescription()

    String getWindWorldSide(bool shortcut) const;
    String getWeatherDescription() const;
    /* Returns weather custom formatted string, options:
    %M     - Sampling time UTC, seconds since Jan 1 1970
    %m     - Update interval, seconds
    %W, %w - Weather description, weather code
    %t, %T - Temperature Celsius, temperature Fahrenheit
    %S, %s - Wind speed in km/h, wind speed in m/s
    %d     - Wind direction
    %E, %e - Wind world side full, wind world side short
    %%     - Escape sequence for % symbol */
    String toString(const char* format = "%W, %t'C, wind %E %sm/s") const;
    String toJSONString() const;
    static const __FlashStringHelper* getWorldSide(float degrees, bool shortcut);
    static const __FlashStringHelper* getWeatherCodeDescription(unsigned int code);

    /* Returns positive number of seconds forecast up to date, otherwise zero */
    uint32_t secondsUpToDate() const;
};

#endif