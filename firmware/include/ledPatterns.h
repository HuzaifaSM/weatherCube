#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

#include "weatherState.h"

// sets LED pin as outputs, switches off and seeds the random number generator (for storm flicker)
void initialiseLeds();

// lights each LED in turn and names over Serial
void runStartupTest();

// draws one frame of pattern from given state
void renderPattern(WeatherState state, unsigned long nowMs);

#endif