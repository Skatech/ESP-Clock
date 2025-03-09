#include "Forecast.h"

const __FlashStringHelper* Forecast::getWorldSide(float degrees, bool shortcut) {
    switch ((int)(degrees + 22.5f + 360.0f) % 360 / 45) {
        case 0: return shortcut ? F("N")  : F("North");
        case 1: return shortcut ? F("NE") : F("North-East");
        case 2: return shortcut ? F("E")  : F("East");
        case 3: return shortcut ? F("SE") : F("South-East");
        case 4: return shortcut ? F("S")  : F("South");
        case 5: return shortcut ? F("SW") : F("South-West");
        case 6: return shortcut ? F("W")  : F("West");
        case 7: return shortcut ? F("NW") : F("North-West");
        default: return F("?");
    }
}

const __FlashStringHelper* Forecast::getWeatherCodeDescription(unsigned int code) {
    switch (code) {
        case 0: return F("Clear sky");
        case 1: return F("Mainly clear");
        case 2: return F("Partly cloudy");
        case 3: return F("Overcast");
        case 45: return F("Fog");
        case 48: return F("Rime fog");
        case 51: return F("Light drizzle");
        case 53: return F("Moderate drizzle");
        case 55: return F("Dense drizzle");
        case 56: return F("Light freezing drizzle");
        case 57: return F("Dense freezing drizzle");
        case 61: return F("Slight rain");
        case 63: return F("Moderate rain");
        case 65: return F("Heavy rain");
        case 66: return F("Light freezing rain");
        case 67: return F("Heavy freezing rain");
        case 71: return F("Slight snow fall");
        case 73: return F("Moderate snow fall");
        case 75: return F("Heavy snow fall");
        case 77: return F("Snow grains");
        case 80: return F("Slight rain showers");
        case 81: return F("Moderate rain showers");
        case 82: return F("Violent rain showers");
        case 85: return F("Slight snow showers");
        case 86: return F("Heavy snow showers");
        case 95: return F("Slight or moderate thunderstorm");
        case 96: return F("Thunderstorm with slight hail");
        case 99: return F("Thunderstorm with heavy hail");
        default: return F("Invalid weather code");
    }
}

String Forecast::getWindWorldSide(bool shortcut) const {
    return getWorldSide(winddirection, shortcut);
}

String Forecast::getWeatherDescription() const {
    return getWeatherCodeDescription(weathercode);
}

String Forecast::toString(const char* format) const {
    String builder(format);
    while (builder.indexOf(F("%M")) >= 0) {
        builder.replace(F("%M"), String(time));
    }
    while (builder.indexOf(F("%m")) >= 0) {
        builder.replace(F("%m"), String(interval));
    }
    while (builder.indexOf(F("%w")) >= 0) {
        builder.replace(F("%w"), String(weathercode));
    }
    while (builder.indexOf(F("%W")) >= 0) {
        builder.replace(F("%W"), getWeatherCodeDescription(weathercode));
    }
    while (builder.indexOf(F("%t")) >= 0) {
        builder.replace(F("%t"), String(temperature, 1));
    }
    while (builder.indexOf(F("%T")) >= 0) {
        builder.replace(F("%T"), String((temperature * 1.8f) + 32, 1));
    }
    while (builder.indexOf(F("%s")) >= 0) {
        builder.replace(F("%s"), String(windspeed / 3.6f, 1));
    }
    while (builder.indexOf(F("%S")) >= 0) {
        builder.replace(F("%S"), String(windspeed, 1));
    }
    while (builder.indexOf(F("%d")) >= 0) {
        builder.replace(F("%d"), String(winddirection, 1));
    }
    while (builder.indexOf(F("%e")) >= 0) {
        builder.replace(F("%e"), getWorldSide(winddirection, true));
    }
    while (builder.indexOf(F("%E")) >= 0) {
        builder.replace(F("%E"), getWorldSide(winddirection, false));
    }
    while (builder.indexOf(F("%%")) >= 0) {
        builder.replace(F("%%"), F("%"));
    }
    return builder;
}

String Forecast::toJSONString() const {
    return toString(R"({"time":%M,"interval":%m,"weathercode":%w,"description":"%W","temperature":%t,"windspeed":%s,"winddir":%d,"windside":"%E","windsideabbr":"%e"})");
}

uint32_t Forecast::secondsUpToDate() const {
    time_t lct = ::time(NULL);
    struct tm* gmt = gmtime(&lct);
    return (uint32_t)min((time_t)UINT32_MAX, max((time_t)0, time + interval - mktime(gmt)));
}
