#include "RadialClockDisplay.h"

RadialClockDisplay::RadialClockDisplay(
        Adafruit_NeoPixel& strip, bool beginStrip) : _strip(strip) {
    trySetColorScheme(DEFAULT_COLOR_SCHEME);
    if (beginStrip) {
        _strip.begin();
        clear();
    }
}

void RadialClockDisplay::clear() {
    _strip.clear();
    _strip.show();
}

bool RadialClockDisplay::test() {
    uint32_t colors[] = { 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF, 0x000000 };
    for(const uint32_t& color : colors) {
        _strip.fill(color);
        _strip.show();
        delay(500);
    }
    return true;
}

void RadialClockDisplay::poll(bool isConnected) const {
    static time_t prev;
    time_t lct = time(NULL);
    if (prev != lct) {
        if ((prev = lct) < 1000000000LL) {
            drawFail(lct, isConnected);
        }
        else drawTime(lct);
    }
}

void RadialClockDisplay::drawFail(const time_t& time, bool isConnected) const {
    const uint8_t ledsCount = _strip.numPixels();
    const uint8_t ledsPerHour = ledsCount / 12;
    const uint32_t drawMask = time & 0b111111;
    for(uint8_t i = 0; i < ledsCount; ++i) {
        if (i % ledsPerHour) { // non-tick space
            if (drawMask & (1U << (i / ledsPerHour % 6))) {
                _strip.setPixelColor(i, isConnected ? 0x000022 : 0x440000);
            }
            else _strip.setPixelColor(i, 0x000000);
        }
        else _strip.setPixelColor(i, _colorTm); // draw ticks
    }
    _strip.show();
}

void RadialClockDisplay::drawTime(const time_t& time) const {
    tm* lct = localtime(&time);
    const uint8_t ss = lct->tm_sec, mm = lct->tm_min, hh = lct->tm_hour;
    const uint8_t ledsCount = _strip.numPixels();
    const uint8_t ledsPerHour = ledsCount / 12;
    const uint8_t ledsPerMinute = ledsCount / 60;

    for(uint8_t i = 0; i < ledsCount; ++i) {
        if (_colorSm && (ss == i / ledsPerMinute)) { // draw second marker
            _strip.setPixelColor(i, _colorSm);
        }
        else if (mm == i / ledsPerMinute && (ss % 2 == 0)) { // draw minute marker (even seconds only)
            _strip.setPixelColor(i, _colorMm);
        } 
        else if (((i % ledsPerHour) || (ss % 2)) &&  // draw hour marker (on tick leds odd seconds only)
                (12 - 1 + hh) % 12 == (ledsCount + i - 1 - mm * ledsPerHour / 60) % ledsCount / ledsPerHour) {
            _strip.setPixelColor(i, (hh < 6 || hh > 17)
                ? _colorHn
                : _colorHd);
        }
        else if (i % ledsPerHour) { // damp non-tick space
            _strip.setPixelColor(i, 0x000000);
        }
        else _strip.setPixelColor(i, _colorTm); // draw ticks
    }
    _strip.show();
}

bool RadialClockDisplay::trySetColorScheme(const String& input) {
    uint32_t bright, colorTm, colorSm, colorMm, colorHd, colorHn;
    if (input.length() == (2 + 6 * 5 + 5) && 
            sscanf(input.c_str(), "%2x,%6x,%6x,%6x,%6x,%6x", &bright,
                &colorTm, &colorSm, &colorMm, &colorHd, &colorHn) == 6) {
        _strip.setBrightness(bright);
        _colorTm = colorTm;
        _colorSm = colorSm;
        _colorMm = colorMm;
        _colorHd = colorHd;
        _colorHn = colorHn;
        return true;
    }
    return false;
}

String RadialClockDisplay::getColorScheme() const {
        char buf[2 + 6 * 5 + 5 + 1];
        sprintf(buf, "%02X,%06X,%06X,%06X,%06X,%06X", _strip.getBrightness(),
            _colorTm, _colorSm, _colorMm, _colorHd, _colorHn);
        return String(buf);
    }