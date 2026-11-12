# Weather mapping

Open-Meteo returns the current conditions as a numeric WMO weather interpretation code. The firmware reduces these to six states. The mapping lives in `firmware/src/weatherState.cpp` and is covered by unit tests in `firmware/test/test_weatherState/`.

## Code to state

| State | WMO codes | Meaning |
|---|---|---|
| Clear | 0, 1 | Clear sky, mainly clear |
| Cloudy | 2, 3, 45, 48 | Partly cloudy, overcast, fog, rime fog |
| Rain | 51, 53, 55, 56, 57, 61, 63, 65, 66, 67, 80, 81, 82 | Drizzle, freezing drizzle, rain, freezing rain, rain showers |
| Snow | 71, 73, 75, 77, 85, 86 | Snowfall, snow grains, snow showers |
| Storm | 95, 96, 99 | Thunderstorm, with or without hail |
| Offline | none | Set by the watchdog only, never by a code |

## Design notes

- **Fog counts as cloudy.** It is a low-visibility, grey condition, and a separate state would add little.
- **Unknown codes fall back to cloudy.** This keeps the cube showing something sensible if a code outside the list ever arrives.
- **Codes are listed explicitly, not as ranges.** The WMO table has gaps that Open-Meteo never returns, and an explicit list makes the intent clear and testable.
- **Offline is not a weather code.** It means "no valid message for too long" and is decided by the watchdog in `main.cpp`. See [serialProtocol.md](serialProtocol.md).

## State to LED behaviour

| State | Prototype (four LEDs) | Final ring (planned) |
|---|---|---|
| Clear | Slow yellow breath | Yellow |
| Cloudy | Steady dim yellow plus green | Grey |
| Rain | Fast green pulse | Blue |
| Snow | Slow green pulse | White-ish |
| Storm | Random red flicker with occasional bright flashes, faint green | Dark purple |
| Offline | Slow blink of the second red LED | To be decided |

The prototype only has yellow, green and red LEDs, so some states are told apart by pattern rather than colour. The ring will use real colours.
