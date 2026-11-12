# Serial protocol

**Prototype only.** This is the link between the laptop bridge and the Uno. The final ESP32-C3 device fetches weather itself, so this protocol goes away with the bridge.

## Link

- USB serial, 9600 baud, 8 data bits, no parity, 1 stop bit (the Arduino default).
- Direction: laptop to board carries weather messages. The board also prints diagnostic text back, which is not part of the protocol.

## Message format

```
W,<wmoCode>\n
```

- `W,` followed by one to three decimal digits, then a newline. Example: `W,61`.
- A carriage return (`\r`) is ignored, so both `\n` and `\r\n` line endings work.
- The firmware accepts values 0 to 999. Mapping a value to a weather state is a separate step, described in [weatherMapping.md](weatherMapping.md).

## What is accepted and ignored

| Input | Result |
|---|---|
| `W,0`, `W,61`, `W,095` | Accepted |
| `W,4` | Accepted, and maps to cloudy as an unknown code |
| `W,` | Ignored (no digits) |
| `W,1234` | Ignored (more than three digits) |
| `W,6x`, `W, 61`, `W,-1` | Ignored (non-digit character) |
| `w,61` | Ignored (the letter is case-sensitive) |
| A line longer than 15 characters | Discarded entirely, up to the next newline |

Malformed input is dropped silently and never changes the state.

## Watchdog and startup

- The board starts in the **offline** state.
- Each valid message resets a timer. If no valid message arrives within `offlineTimeoutMs` (30 minutes, set in `config.h`), the board returns to offline.
- The bridge sends a code every 10 minutes. If a fetch fails it sends nothing, so three missed fetches in a row bring the offline state.

## Opening the port resets the board

Opening the serial port toggles the Uno's reset line. The bootloader then runs for about a second, followed by the startup test (roughly 3 seconds). Bytes sent in the first moments can be lost, so the bridge waits (`boardResetWaitSeconds`, default 4) before its first message.

## Board output

The board prints a label for each LED during the startup test, a ready message, `Recieved code: <n>` for each valid message, and `State: <name>` on each state change. This is for debugging, and the bridge simply echoes it.
