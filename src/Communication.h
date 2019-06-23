#ifndef SSTV_TIMELAPSE_COMMUNICATION_H
#define SSTV_TIMELAPSE_COMMUNICATION_H

#include <Arduino.h>
#include "Engine.h"

class Communication {

public:
    void init(int addr);

    COMMAND getCommand() { return command; };

    uint16_t getData() { return data; };

    void setResponse(COMMAND commandTarget, uint16_t value) { response[commandTarget] = value; }

    void resetCommand() { command = NOTHING; }

    bool hasCommand() { return command != NOTHING; }

    static Communication *getInstance();

private:
    Communication();

    static void receiveData(int bytes);

    static void sendData();

    static Communication *instance = nullptr;

    COMMAND command;
    uint16_t data;
    uint16_t response[16];
};

#endif //SSTV_TIMELAPSE_COMMUNICATION_H
