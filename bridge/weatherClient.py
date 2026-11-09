import requests

apiUrl = "https://api.open-meteo.com/v1/forecast"
requestTimeoutSeconds = 10

def fetchWeatherCode(latitude, longitude):
    """returns current WMO weather code, or None"""
    parameters = {
        'latitude': latitude,
        'longitude': longitude,
        'current': 'weather_code',
    }

    try:
        response = requests.get(apiUrl, params=parameters, timeout=requestTimeoutSeconds)
        response.raise_for_status()
        weatherCode = response.json()['current']['weather_code']
    except (requests.RequestException, ValueError, KeyError, TypeError) as e:
        #print(f'error: {e}')
        return None

    # firmware accepts 1-3 digits, reject anything else
    if isinstance(weatherCode, bool) or not isinstance(weatherCode, int):
        return None
    if weatherCode < 0 or weatherCode > 999:
        return None

    return weatherCode