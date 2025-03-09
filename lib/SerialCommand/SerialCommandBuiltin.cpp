#include <SerialCommand.h>

#ifndef SERIAL_COMMAND_BUILTIN_RESERVE
#define SERIAL_COMMAND_BUILTIN_RESERVE 64
#endif

void __attribute__((weak)) onSerialCommand(const String& input) {
    // Serial.println("Define handler onSerialCommand(const String&)");
}

SerialCommand __scmd_internal(SERIAL_COMMAND_BUILTIN_RESERVE);

void __attribute__((used)) serialEvent() {
    if (__scmd_internal.update()) {
        onSerialCommand(__scmd_internal.value());
        __scmd_internal.clear();
    }
}
