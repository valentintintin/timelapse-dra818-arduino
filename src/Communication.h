#ifndef SSTV_TIMELAPSE_COMMUNICATION_H
#define SSTV_TIMELAPSE_COMMUNICATION_H

#include <Arduino.h>
#include "Engine.h"

class Communication {

public:
    static Communication *getInstance();

    void seti2cAddress(uint8_t addr);

    void begin();

    COMMAND getCommand();

    uint16_t getData();

    void setResponse(COMMAND commandTarget, uint16_t value);

    void resetCommand();

    bool hasCommand();

private:
    static void receiveData(int bytes);
    static void sendData();
    static Communication *instance = nullptr;

    Communication();

    uint8_t i2cAddress;
    COMMAND command;
    uint16_t data;
    uint16_t response[16];
};

#endif //SSTV_TIMELAPSE_COMMUNICATION_H
