#include "serialLink.h"
#include <Arduino.h>
#include "config.h"

namespace {

constexpr uint8_t lineBufferSize = 16;

char lineBuffer[lineBufferSize];
uint8_t lineLength = 0;
bool discardingLine = false;

// accepts exactly "W," followed by 1-3 digits, otherwise rejects
bool parseMessage(const char *line, int &wmoCode) {
    if (line[0] != 'W'|| line[1] != ',') {
        return false;
    }

    const char *digits = line + 2;
    if (*digits == '\0') {
        return false;
    }

    int value = 0;
    uint8_t digitCount = 0;
    for (const char *character = digits; *character != '\0'; character++) {
        if (*character < '0' || *character > '9') {
            return false;
        }
        digitCount++;
        if (digitCount > 3) {
            return false;
        }
        value = value * 10 + (*character - '0');
    }

    wmoCode = value;
    return true;
}

} // namespace

void initialiseSerialLink() {
    Serial.begin(serialBaudRate);
    lineLength = 0;
    discardingLine = false;
}

bool pollWeatherMessage(int &wmoCode) {
    while (Serial.available() > 0) {
        const char incoming = static_cast<char>(Serial.read());

        if (incoming == '\r') {
            continue;
        }

        if (incoming == '\n') {
            const bool lineWasValidLength = !discardingLine;
            lineBuffer[lineLength] = '\0';
            lineLength = 0;
            discardingLine = false;

            if (lineWasValidLength && parseMessage(lineBuffer, wmoCode)) {
                return true;
            }
            continue;
        }

        if (discardingLine) {
            continue;
        }

        if (lineLength < lineBufferSize - 1) {
            lineBuffer[lineLength] = incoming;
            lineLength++;
        } else {
            discardingLine = true;
        }
    }

    return false;
}