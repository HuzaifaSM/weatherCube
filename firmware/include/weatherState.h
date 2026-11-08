#ifndef WEATHER_STATE_H
#define WEATHER_STATE_H

// display states the cube can show
enum class WeatherState {
    clear,
    cloudy,
    rain,
    snow,
    storm,
    offline
};

// maps an Open-Meteo (WMO) weather code to a display state. unknown codes fall back to cloudy
WeatherState stateFromWmoCode(int wmoCode);

#endif