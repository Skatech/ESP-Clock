#ifndef CommandHelpers_h
#define CommandHelpers_h

#include <Arduino.h>
#include <IPAddress.h>

class CommandHelpers {
public:
    static unsigned int getValueOffset(const String& input, const String& prefix, char suffix);
    static bool printOrUpdateValue(const String& input, const String& name, String& value);
    static bool printOrUpdateValue(const String& input, const String& name, int& value);
    static bool printOrUpdateValue(const String& input, const String& name, float& value);
    static bool printOrUpdateValue(const String& input, const String& name, IPAddress& value);
};

#endif