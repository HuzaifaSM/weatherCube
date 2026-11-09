#include "ledPatterns.h"
#include <Arduino.h>
#include "config.h"

namespace {

// brightness levels below are 0-255 before maxBrightness cap
struct LedLevels {
    uint8_t clear;
    uint8_t rain;
    uint8_t storm;
    uint8_t offline;
};

constexpr unsigned long clearPeriodMs = 4000UL;
constexpr unsigned long rainPeriodMs = 700UL;
constexpr unsigned long snowPeriodMs = 3500UL;
constexpr unsigned long offlinePeriodMs = 2000UL;
constexpr unsigned long offlineOnMs = 500UL;

constexpr uint8_t cloudyYellowLevel = 70;
constexpr uint8_t cloudyGreenLevel = 35;
constexpr uint8_t stormGreenLevel = 25;

unsigned long nextStormChangeMs = 0;
uint8_t stormLevel = 0;

void setBrightness(uint8_t pin, uint8_t level) {
    const uint16_t scaled = (static_cast<uint16_t>(level) * maxBrightness) / 255U;
    analogWrite(pin, static_cast<uint8_t> (scaled));
}

void writeLevels(const LedLevels &levels) {
    setBrightness(ledPinClear, levels.clear);
    setBrightness(ledPinRain, levels.rain);
    setBrightness(ledPinStorm, levels.storm);
    setBrightness(ledPinOffline, levels.offline);
}

// triangle wave from 0 to 255 and back, repeating every periodMs
uint8_t triangleWave(unsigned long nowMs, unsigned long periodMs) {
    const unsigned long phase = nowMs % periodMs;
    const unsigned long half = periodMs / 2;
    const unsigned long rising = (phase < half) ? phase : (periodMs - phase);
    return static_cast<uint8_t>((rising * 255UL) / half);
}

// squares level so fades look even to eye rather than abrupt at bottom
uint8_t easeLevel(uint8_t level) {
    return static_cast<uint8_t>((static_cast<uint16_t>(level) * level) / 255U);
}

LedLevels clearPattern(unsigned long nowMs) {
    const uint8_t breath = easeLevel(triangleWave(nowMs, clearPeriodMs));
    return {breath, 0, 0, 0};
}

LedLevels cloudyPattern() {
    return {cloudyYellowLevel, cloudyGreenLevel, 0, 0};
}

LedLevels rainPattern(unsigned long nowMs) {
    return {0, triangleWave(nowMs, rainPeriodMs), 0, 0};
}

LedLevels snowPattern(unsigned long nowMs) {
    return {0, easeLevel(triangleWave(nowMs, snowPeriodMs)), 0, 0};
}

LedLevels stormPattern(unsigned long nowMs) {
    // signed difference keeps correct when millis() wraps after ~49 days
    if (static_cast<long>(nowMs - nextStormChangeMs) >= 0) {
        const bool flash = (random(0, 6) == 0);
        stormLevel = flash ? 255 : static_cast<uint8_t>(random(10, 70));
        nextStormChangeMs = nowMs + static_cast<unsigned long>(random(30, 200));
    }
    return {0, stormGreenLevel, stormLevel, 0};
}

LedLevels offlinePattern(unsigned long nowMs) {
    const bool lit = (nowMs % offlinePeriodMs) < offlineOnMs;
    return {0, 0, 0, static_cast<uint8_t>(lit ? 255 : 0)};
}

void testLed(uint8_t pin, const __FlashStringHelper *label) {
    Serial.print(F("Start-up test: "));
    Serial.print(label);
    Serial.print(F(" on pin "));
    Serial.println(pin);

    setBrightness(pin, 255);
    delay(500);
    setBrightness(pin, 0);
    delay(200);
}

} // namespace

void initialiseLeds() {
    pinMode(ledPinClear, OUTPUT);
    pinMode(ledPinRain, OUTPUT);
    pinMode(ledPinStorm, OUTPUT);
    pinMode(ledPinOffline, OUTPUT);
    writeLevels({0, 0, 0, 0});

    // unconnected analogue pin picks up electrical noise: makes storm flicker different on every powerup
    randomSeed(analogRead(A0));
}

void runStartupTest() {
    testLed(ledPinClear, F("yellow LED (clear)"));
    testLed(ledPinRain, F("green LED (rain)"));
    testLed(ledPinStorm, F("red LED (storm)"));
    testLed(ledPinOffline, F("red LED (offline)"));
}

void renderPattern(WeatherState state, unsigned long nowMs) {
    switch (state) {
        case WeatherState::clear:
            writeLevels(clearPattern(nowMs));
            break;
        case WeatherState::cloudy:
            writeLevels(cloudyPattern());
            break;
        case WeatherState::rain:
            writeLevels(rainPattern(nowMs));
            break;
        case WeatherState::snow:
            writeLevels(snowPattern(nowMs));
            break;
        case WeatherState::storm:
            writeLevels(stormPattern(nowMs));
            break;
        case WeatherState::offline:
            writeLevels(offlinePattern(nowMs));
            break;
    }
}