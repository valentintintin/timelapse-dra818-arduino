#ifndef SSTV_TIMELAPSE_ENGINE_H
#define SSTV_TIMELAPSE_ENGINE_H

#include <Arduino.h>

class Engine;

enum COMMAND {
    NOTHING,
    GET_VOLTAGE,
    GET_TEMPERATURE,
    GET_DRA,
    GET_BUTTON,
    SET_WAKEUP_HOUR,
    SET_WAKEUP_MINUTE,
    SLEEP,
    SET_PTT_ON,
    SET_PTT_OFF,
    SET_FREQ_APRS,
    SET_FREQ_SSTV
};

#include "Communication.h"
#include "DRA.h"
#include "Sleep.h"

class Engine {

public:
    enum STATE {
        PI_STOPPED, PI_STOPPING, PI_RUNNING
    };

    static void begin(byte raspberryPin, float freqAprs, float freqSstv,
                      byte rxDra, byte txDra, byte pttPin, byte activePin,
                      int i2cAddress, byte wakeUpButtonPin, byte wakeUpRtcPin);

    static void loop();

    static void computeCommand();

private:
    static void startPi();

    static void restartPi();
    static void stopPi();

    static byte raspberryPin;
    static STATE state;
    static float freqAprs;
    static float freqSstv;
};


#endif //SSTV_TIMELAPSE_ENGINE_H
