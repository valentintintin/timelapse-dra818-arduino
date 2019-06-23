#ifndef ARDUINO_TRACKER_APRS_DRA818_DRA_H
#define ARDUINO_TRACKER_APRS_DRA818_DRA_H

#include <Arduino.h>
#include <DRA818.h>
#include <SoftwareSerial.h>

#define TIME_TOGGLE_ACTIVE 1500
#define TIME_TOGGLE_PTT 500

class DRA {
public:
    DRA(byte rxDra, byte txDra, byte pttPin, byte activePin);

    virtual ~DRA();

    bool begin(float txFreq = 144.600, bool deactiveAfter = true, char loop = 1);

    void tx();

    void stopTx(bool deactiveAfter = true);

    bool isTx();

    bool isActive();

    bool isDraDetected();

    void setFreq(float freq, bool deactiveAfter = false);

    DRA818 *dra;

private:
    void active();

    void deactive();

    SoftwareSerial *serial;

    byte pttPin;
    byte activePin;

    bool activeState = false;
    bool txState = false;
};


#endif //ARDUINO_TRACKER_APRS_DRA818_DRA_H
