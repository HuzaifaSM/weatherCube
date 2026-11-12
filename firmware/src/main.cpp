#include <Arduino.h>
#include "config.h"
#include "ledPatterns.h"
#include "serialLink.h"
#include "weatherState.h"

// starts offline
static WeatherState currentState = WeatherState::offline;
static unsigned long lastMessageMs = 0;
static bool hasReceivedMessage = false;

static const __FlashStringHelper *stateName(WeatherState state) {
    switch (state) {
        case WeatherState::clear:
            return F("clear");
        case WeatherState::cloudy:
            return F("cloudy");
        case WeatherState::rain:
            return F("rain");
        case WeatherState::snow:
            return F("snow");
        case WeatherState::storm:
            return F("storm");
        case WeatherState::offline:
            return F("offline");
    }
    return F("unknown");
}

static void changeState(WeatherState newState) {
    if (newState == currentState) {
        return;
    }

    currentState = newState;
    Serial.print(F("State: "));
    Serial.println(stateName(newState));
}

void setup() {
    initialiseSerialLink();
    initialiseLeds();
    runStartupTest();
    Serial.println(F("Weather cube ready. Send W,<wmoCode e.g. W,95"));
}

void loop () {
    const unsigned long nowMs = millis();

    int wmoCode = 0;
    if (pollWeatherMessage(wmoCode)) {
        hasReceivedMessage = true;
        lastMessageMs = nowMs;
        Serial.print(F("Received code: "));
        Serial.println(wmoCode);
        changeState(stateFromWmoCode(wmoCode));
    } else if (hasReceivedMessage && (nowMs - lastMessageMs) > offlineTimeoutMs) {
        changeState(WeatherState::offline);
    }

    renderPattern(currentState, nowMs);
}