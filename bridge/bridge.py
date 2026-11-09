import json
import sys
import time
import serial

from weatherClient import fetchWeatherCode

configPath = "bridge/config.json"
baudRate = 9600
loopSleepSeconds = 0.2

def loadConfig(path):
    with open(path, 'r', encoding='utf-8') as configFile:
        return json.load(configFile)

def sendWeatherCode(port, weatherCode):
    port.write(f'W,{weatherCode}\n'.encode('ascii'))
    port.flush()

def printBoardOutput(port):
    # show anything board has printed so state changes are visible
    while port.in_waiting > 0:
        line = port.readline().decode('ascii', errors='replace').rstrip()
        if line:
            print(f'board: {line}')

def run(config):
    latitude = config['latitude']
    longitude = config['longitude']
    pollIntervalSeconds = config['pollIntervalSeconds']

    with serial.Serial(config['serialPort'], baudRate, timeout=1) as port:
        # wait for bootloader and startup test
        print(f"Waiting {config['boardResetWaitSeconds']}s for board to reset.")
        time.sleep(config['boardResetWaitSeconds'])
        printBoardOutput(port)

        nextPollTime = time.monotonic()
        while True:
            if time.monotonic() >= nextPollTime:
                weatherCode = fetchWeatherCode(latitude, longitude)
                if weatherCode is None:
                    print('Fetch failed. Sending nothing.')
                else:
                    sendWeatherCode(port, weatherCode)
                    print(f'Sent code {weatherCode}')
                nextPollTime = time.monotonic() + pollIntervalSeconds

            printBoardOutput(port)
            time.sleep(loopSleepSeconds)

def main():
    try:
        config = loadConfig(configPath)
        run(config)
    except FileNotFoundError:
        print(f'Could not find {configPath}. Copy config.example.json and fill it in.')
        sys.exit(1)
    except KeyError as error:
        print(f'Missing setting in {configPath}: {error}')
        sys.exit(1)
    except serial.SerialException as error:
        print(f'Serial problem: {error}')
        sys.exit(1)
    except KeyboardInterrupt:
        print('Stopped.')

if __name__ == '__main__':
    main()