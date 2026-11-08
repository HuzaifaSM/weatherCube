#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// pin assignments
constexpr uint8_t ledPinClear = 9; // clear, part of cloudy
constexpr uint8_t ledPinRain = 6; // rain, snow, part of cloudy
constexpr uint8_t ledPinStorm = 5; // storm
constexpr uint8_t ledPinOffline = 3; // offline

// serial link to laptop
constexpr unsigned long serialBaudRate = 9600;

// if no valid weather message arrives within time, show offline state
constexpr unsigned long offlineTimeoutMs = 30UL * 60UL * 1000UL;

// upper limit for LED brightness (0 to 255), keep glow gentle
constexpr uint8_t maxBrightness = 128;

#endif