#ifndef SERIAL_LINK_H
#define SERIAL_LINK_H

// reads newline-temrinated messages from laptop bridge without blocking.
// message format: "W,<wmoCode>" followed by a newlLine e.g. "W,61"

// starts serial port and clears line buffer
void initialiseSerialLink();

// reads any waiting bytes and returns true when a complete, valid message has just arrived, writing the code into wmoCode
bool pollWeatherMessage(int &wmoCode);

#endif