#include <ConfigUtils.h>
#include "DeviceConfig.h"
#include "Annotate.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof" // disable IPAddress fields layout warning
const char* __DeviceConfigFilePath = "/config/device-config.json";
const FieldHandle __DeviceConfigFields[] = {
    { "ssid", FieldType::StringT, offsetof(DeviceConfig, ssid) },
    { "password", FieldType::StringT, offsetof(DeviceConfig, password) },
    { "hostname", FieldType::StringT, offsetof(DeviceConfig, hostname) },

    { "address", FieldType::IPAddressT, offsetof(DeviceConfig, address) },
    { "gateway", FieldType::IPAddressT, offsetof(DeviceConfig, gateway) },
    { "subnet", FieldType::IPAddressT, offsetof(DeviceConfig, subnet) },
    { "dns", FieldType::IPAddressT, offsetof(DeviceConfig, dns) },

    { "timezone", FieldType::StringT, offsetof(DeviceConfig, DeviceConfig::timezone) },
    { "timeserver1", FieldType::StringT, offsetof(DeviceConfig, timeserver1) },
    { "timeserver2", FieldType::StringT, offsetof(DeviceConfig, timeserver2) },
    { "timeserver3", FieldType::StringT, offsetof(DeviceConfig, timeserver3) },

    { "latitude", FieldType::FloatT, offsetof(DeviceConfig, latitude) },
    { "longitude", FieldType::FloatT, offsetof(DeviceConfig, longitude) },

    { "displaycsv", FieldType::StringT, offsetof(DeviceConfig, displaycsv) },
};
#pragma GCC diagnostic pop

bool DeviceConfig::load() {
    return ConfigUtils::loadFromJSON(__DeviceConfigFields,
        sizeof(__DeviceConfigFields) / sizeof(FieldHandle), this, __DeviceConfigFilePath);
}

bool DeviceConfig::save() {
    return ConfigUtils::saveToJSON(__DeviceConfigFields,
        sizeof(__DeviceConfigFields) / sizeof(FieldHandle), this, __DeviceConfigFilePath);
}

bool DeviceConfig::exec(const String& command) {
    if (command == "config-show") {
        ConfigUtils::printAllFields(__DeviceConfigFields,
            sizeof(__DeviceConfigFields) / sizeof(FieldHandle), this);
    }
    else if (command == "config-save") {
        Annotate("Configuration writing... ")(save());
    }
    else return ConfigUtils::printOrUpdateField(__DeviceConfigFields,
        sizeof(__DeviceConfigFields) / sizeof(FieldHandle), this, command);
    return true;
}
