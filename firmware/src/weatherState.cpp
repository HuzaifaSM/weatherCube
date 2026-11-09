#include "weatherState.h"

WeatherState stateFromWmoCode(int wmoCode) {
    // codes are listed explicitly rather than ranges, because WMO table has gaps that Open-Meteo never returns

    switch (wmoCode) {
        case 0:
        case 1:
            return WeatherState::clear;

        case 2:
        case 3:
        case 45:
        case 48:
            return WeatherState::cloudy;

        case 51:
        case 53:
        case 55:
        case 56:
        case 57:
        case 61:
        case 63:
        case 65:
        case 66:
        case 67:
        case 80:
        case 81:
        case 82:
            return WeatherState::rain;

        case 71:
        case 73:
        case 75:
        case 77:
        case 85:
        case 86:
            return WeatherState::snow;

        case 95:
        case 96:
        case 97:
        case 99:
            return WeatherState::storm;

        default:
            return WeatherState::cloudy;
    }
}