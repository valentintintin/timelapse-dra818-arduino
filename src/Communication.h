#ifndef SSTV_TIMELAPSE_COMMUNICATION_H
#define SSTV_TIMELAPSE_COMMUNICATION_H

#include <Arduino.h>
#include "Engine.h"

class Communication {

public:
    void init(int addr, Engine *engine);

    static Communication *getInstance();

private:
    Communication();

    static void receiveData(int bytes);

    static void sendData();

    static Communication *instance = nullptr;

    Engine *engine;
    COMMAND command;
    int data;
};

#endif //SSTV_TIMELAPSE_COMMUNICATION_H
