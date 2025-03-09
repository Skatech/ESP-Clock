#include <LittleFS.h>
#include <ArduinoJson.h>
#include <CommandHelpers.h>
#include "ConfigUtils.h"

#ifndef CONFIG_UTILS_JSON_DOC_CAPACITY
#error Undefined constant CONFIG_UTILS_JSON_DOC_CAPACITY (in platformio.ini)
//#define CONFIG_UTILS_JSON_DOC_CAPACITY 256
#endif

bool readJsonFile(const char *path, JsonDocument& doc) {
    File file = LittleFS.open(path, "r");
    if (file) {
        DeserializationError result = deserializeJson(doc, file);
        file.close();
        return result == DeserializationError::Ok;
    }
    return false;
}

bool writeJsonFile(const char *path, const JsonDocument& doc) {
    File file = LittleFS.open(path, "w");
    if (file) {
        const size_t size = serializeJson(doc, file);
        file.close();
        return size > 0;
    }
    return false;
}

bool ConfigUtils::loadFromJSON(
        const FieldHandle* handles, size_t count, void* obj, const char* filePath) {
    StaticJsonDocument<CONFIG_UTILS_JSON_DOC_CAPACITY> doc;
    if (!readJsonFile(filePath, doc)) {
        return false;
    }
    for(size_t i = 0; i < count; ++i) {
        const FieldHandle& f = handles[i];
        if (f.type == FieldType::StringT) {
            if (doc.containsKey(f.name))
                f.field_ref<String>(obj) = doc[f.name].as<String>();
        }
        else if (f.type == FieldType::IntT) {
            if (doc.containsKey(f.name))
                f.field_ref<int>(obj) = doc[f.name];
        }
        else if (f.type == FieldType::FloatT) {
            if (doc.containsKey(f.name))
                f.field_ref<float>(obj) = doc[f.name];
        }
        else if (f.type == FieldType::IPAddressT) {
            if (doc.containsKey(f.name))
                f.field_ref<IPAddress>(obj).fromString(doc[f.name].as<String>());
        }
    }
    #ifdef CONFIG_UTILS_JSON_TRACE
        Serial.print("JSON Document capacity usage: ");
        Serial.print(doc.memoryUsage());
        Serial.print(" of ");
        Serial.println(doc.capacity());
    #endif
    return true;
}

bool ConfigUtils::saveToJSON(
        const FieldHandle* handles, size_t count, void* obj, const char* filePath) {
    StaticJsonDocument<CONFIG_UTILS_JSON_DOC_CAPACITY> doc;
    for(size_t i = 0; i < count; ++i) {
        const FieldHandle& f = handles[i];
        if (f.type == FieldType::StringT) {
            doc[f.name] = f.field_ref<String>(obj);
        }
        else if (f.type == FieldType::IntT) {
            doc[f.name] = f.field_ref<int>(obj);
        }
        else if (f.type == FieldType::FloatT) {
            doc[f.name] = f.field_ref<float>(obj);
        }
        else if (f.type == FieldType::IPAddressT) {
            doc[f.name] = f.field_ref<IPAddress>(obj).toString();
        }
    }
    #ifdef CONFIG_UTILS_JSON_TRACE
        Serial.printf("JSON Document capacity usage: %u of %u\n", doc.memoryUsage(), doc.capacity());
    #endif
    return writeJsonFile(filePath, doc);
}

void ConfigUtils::printAllFields(const FieldHandle* handles, size_t count, void* obj) {
    for(size_t i = 0; i < count; ++i) {
        const FieldHandle& f = handles[i];
        Serial.print(f.name);
        Serial.print(": ");        
        if (f.type == FieldType::StringT) {
            Serial.println(f.field_ref<String>(obj));
        }
        else if (f.type == FieldType::IntT) {
            Serial.println(f.field_ref<int>(obj));
        }
        else if (f.type == FieldType::FloatT) {
            Serial.println(f.field_ref<float>(obj));
        }
        else if (f.type == FieldType::IPAddressT) {
            Serial.println(f.field_ref<IPAddress>(obj));
        }
    }
}

bool ConfigUtils::printOrUpdateField(const FieldHandle* handles, size_t count, void* obj, const String& command) {
    for(size_t i = 0; i < count; ++i) {
        const FieldHandle& f = handles[i];
        if (f.type == FieldType::StringT) {
            if (CommandHelpers::printOrUpdateValue(command, f.name, f.field_ref<String>(obj)))
                return true;
        }
        else if (f.type == FieldType::IntT) {
            if (CommandHelpers::printOrUpdateValue(command, f.name, f.field_ref<int>(obj)))
                return true;
        }
        else if (f.type == FieldType::FloatT) {
            if (CommandHelpers::printOrUpdateValue(command, f.name, f.field_ref<float>(obj)))
                return true;
        }
        else if (f.type == FieldType::IPAddressT) {
            if (CommandHelpers::printOrUpdateValue(command, f.name, f.field_ref<IPAddress>(obj)))
                return true;
        }
    }
    return false;
}
