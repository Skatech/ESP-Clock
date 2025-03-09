#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h> 
#include <OpenMeteoForecastProvider.h>

class RadialWeatherDisplay {
    Adafruit_NeoPixel& _strip;
    const OpenMeteoForecastProvider& _fcast;
    void drawWeather(const time_t& time) const;

public:
    RadialWeatherDisplay(const RadialWeatherDisplay&) = delete;
    RadialWeatherDisplay& operator=(const RadialWeatherDisplay&) = delete;
    RadialWeatherDisplay(Adafruit_NeoPixel& strip, const OpenMeteoForecastProvider& fcast, bool beginStrip);
    void clear();
    bool poll() const;
};