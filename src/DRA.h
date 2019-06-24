#ifndef ARDUINO_TRACKER_APRS_DRA818_DRA_H
#define ARDUINO_TRACKER_APRS_DRA818_DRA_H

#include <Arduino.h>
#include <DRA818.h>
#include <SoftwareSerial.h>

#define TIME_TOGGLE_ACTIVE 1500
#define TIME_TOGGLE_PTT 500

class DRA {
public:
    static bool begin(byte rxDra, byte txDra, byte pttPin, byte activePin,
                      float txFreq = 144.600, bool deactiveAfter = true, char loop = 1
    );

    static void tx();

    static void stopTx(bool deactiveAfter = true);

    static bool isTx();

    static bool isActive();

    static bool isDraDetected();

    static void setFreq(float freq, bool deactiveAfter = false);

    static DRA818 *dra;

private:
    static void active();

    static void deactive();

    static SoftwareSerial *serial;
    static byte pttPin;
    static byte activePin;
    static bool activeState;
    static bool txState;
};


#endif //ARDUINO_TRACKER_APRS_DRA818_DRA_H
