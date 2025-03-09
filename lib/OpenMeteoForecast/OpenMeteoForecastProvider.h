#pragma once
#include <Arduino.h>
#include "Forecast.h"

class OpenMeteoForecastProvider {
    String _request;
    Forecast _fcast;

public:
    void init(float latitude, float longitude);
    bool poll();
    bool hasForecast(bool upToDateOnly) const;
    const Forecast& getForecast() const;
};
