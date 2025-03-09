#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h> 
#include <time.h>

#define DEFAULT_COLOR_SCHEME F("32,0B0800,001100,FF0000,3333AA,000044")

class RadialClockDisplay {
    Adafruit_NeoPixel& _strip; // tick-mark, second-mark, minute-mark, hours-day, hours-night
    uint32_t _colorTm, _colorSm, _colorMm, _colorHd, _colorHn;
    void drawTime(const time_t& time) const;
    void drawFail(const time_t& time, bool isConnected) const;

public:
    RadialClockDisplay(const RadialClockDisplay&) = delete;
    RadialClockDisplay& operator=(const RadialClockDisplay&) = delete;
    RadialClockDisplay(Adafruit_NeoPixel& strip, bool beginStrip); //(60, PIN, NEO_GRB + NEO_KHZ800)
    void clear();
    bool test();
    void poll(bool isConnected) const;

    // Try set brightness and color scheme from hex string  (37-character long)
    // format: brightness, tick-mark, second-mark, minute-mark,
    //         hours-day, hours-night (BB,TTTTTT,SSSSSS,MMMMMM,DDDDDD,NNNNNN)
    bool trySetColorScheme(const String& input);

    // Returns color scheme hex string (37-character long),
    // format: brightness, tick-mark, second-mark, minute-mark,
    //         hours-day, hours-night (BB,TTTTTT,SSSSSS,MMMMMM,DDDDDD,NNNNNN)
    String getColorScheme() const;
};