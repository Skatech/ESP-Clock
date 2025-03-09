#ifndef ConfigUtils_h
#define ConfigUtils_h

#include <Arduino.h>

enum class FieldType { IntT, FloatT, StringT, IPAddressT };

class FieldHandle {
public:
    const char* name;
    FieldType type;
    size_t offset;

    template<typename T> T& field_ref(void *obj) const {
        return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(obj) + offset);
    }
};

class ConfigUtils {
public:
    static bool loadFromJSON(const FieldHandle* handles, size_t count, void* obj, const char* filePath);
    static bool saveToJSON(const FieldHandle* handles, size_t count, void* obj, const char* filePath);
    static void printAllFields(const FieldHandle* handles, size_t count, void* obj);
    static bool printOrUpdateField(const FieldHandle* handles, size_t count, void* obj, const String& command);
};

#endif