#include "CommandHelpers.h"

unsigned int CommandHelpers::getValueOffset(
        const String& input, const String& prefix, char suffix) {
    const auto optlen = prefix.length();
    return (input.length() > optlen &&
            input.startsWith(prefix) && suffix == input.charAt(optlen))
        ? optlen + 1
        : 0;
}

bool CommandHelpers::printOrUpdateValue(const String& input, const String& name, String& value) {
    const auto offset = getValueOffset(input, name, '=');
    if (offset > name.length()) {
        value = input.substring(offset);
        return true;    
    }
    if (getValueOffset(input, name, '?') == input.length()) {
        Serial.println(value);
        return true;
    }
    return false;
}

bool CommandHelpers::printOrUpdateValue(const String& input, const String& name, int& value) {
    const auto offset = getValueOffset(input, name, '=');
    if (offset > name.length()) {
        value = input.substring(offset).toInt();
        return true;    
    }
    if (getValueOffset(input, name, '?') == input.length()) {
        Serial.println(value);
        return true;
    }
    return false;
}

bool CommandHelpers::printOrUpdateValue(const String& input, const String& name, float& value) {
    const auto offset = getValueOffset(input, name, '=');
    if (offset > name.length()) {
        value = input.substring(offset).toFloat();
        return true;    
    }
    if (getValueOffset(input, name, '?') == input.length()) {
        Serial.println(value);
        return true;
    }
    return false;
}

bool CommandHelpers::printOrUpdateValue(const String& input, const String& name, IPAddress& value) {
    const auto offset = getValueOffset(input, name, '=');
    if (offset > name.length()) {
        value.fromString(input.length() ? input.substring(offset) : "0.0.0.0");
        return true;
    }
    if (getValueOffset(input, name, '?') == input.length()) {
        Serial.println(value);
        return true;
    }
    return false;
}
