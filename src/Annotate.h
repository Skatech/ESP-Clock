#ifndef Annotate_h
#define Annotate_h

#include <Arduino.h>

class Annotate {
    bool critical = false;
public:    
    Annotate(const char* title = nullptr) {
        if (title) {
            Serial.print(title);
        }
    }

    Annotate(const __FlashStringHelper* title) {
        Serial.print(title);
    }

    Annotate& makeCritical() {
        critical = true;
        return *this;
    }

    bool operator()(bool result) {
        if(result) {
            Serial.println("OK");
            return true;
        }
        if(critical) {
            Serial.println("FAILED, SYSTEM HALTED");
            while(true) {
                ESP.deepSleep(UINT32_MAX);
            }
        }
        Serial.println("FAILED");
        return false;
    }
};

#endif
