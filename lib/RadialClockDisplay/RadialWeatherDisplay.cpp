#include "RadialWeatherDisplay.h"

RadialWeatherDisplay::RadialWeatherDisplay(Adafruit_NeoPixel& strip,
        const OpenMeteoForecastProvider& fcast, bool beginStrip) : _strip(strip), _fcast(fcast) {
    if (beginStrip) {
        _strip.setBrightness(50);
        _strip.begin();
        clear();
    }
}

void RadialWeatherDisplay::clear() {
    _strip.clear();
    _strip.show();
}

bool RadialWeatherDisplay::poll() const {
    static time_t prev;
    time_t lct = time(NULL);
    uint8_t sec = lct % 60;
    if (_fcast.hasForecast(true) && ((sec > 0 && sec < 10) || (sec > 30 && sec < 40))) {
        if (prev != lct) {
            drawWeather(prev = lct);
        }   
        return true;
    }
    return false;
}

void RadialWeatherDisplay::drawWeather(const time_t& time) const {
    // const uint32_t WARM_NORM = 0x440000, WARM_TICK = 0x551111, COLD_NORM = 0x000044, COLD_TICK = 0x111155;
    const uint32_t WARM_NORM = 0x200000, WARM_TICK = 0x301010, COLD_NORM = 0x000020, COLD_TICK = 0x101030;
    const uint8_t ledsCount = _strip.numPixels();
    const uint8_t ledsPerTick = 5U;
    const Forecast& fcast = _fcast.getForecast();
    const bool oddsec = time & 0x01, iswarm = fcast.temperature > 0;
    const int t = roundf(fcast.temperature + (iswarm ? -0.249f : 0.249f)); // -0.249f : 0.249f >> 0.75
    for(uint8_t i = 0; i < ledsCount; ++i) {
        if (i == 0 || (t > 0 ? i <= t : i >= ledsCount + t)) {
            if (oddsec || i % ledsPerTick) {
                _strip.setPixelColor(i, iswarm ? WARM_NORM : COLD_NORM);
            }
            else _strip.setPixelColor(i, iswarm ? WARM_TICK : COLD_TICK);
        }
        else _strip.setPixelColor(i, 0x000000);
    }
    _strip.show();
}
