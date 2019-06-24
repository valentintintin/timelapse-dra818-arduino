#ifndef SSTV_TIMELAPSE_COMMUNICATION_H
#define SSTV_TIMELAPSE_COMMUNICATION_H

#include <Arduino.h>
#include "Engine.h"

class Communication {

public:
    static void begin(uint8_t i2cAddress);

    static COMMAND getCommand();

    static uint8_t getData();

    static void setResponse(COMMAND commandTarget, uint16_t value);

    static void resetCommand();

    static bool hasCommand();

private:
    static void receiveData(int bytes);
    static void sendData();

    static COMMAND command;
    static uint8_t data;
    static uint16_t response[12];
};

#endif //SSTV_TIMELAPSE_COMMUNICATION_H
