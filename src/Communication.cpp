#include <Wire.h>
#include "Communication.h"

Communication *Communication::instance;

void Communication::init(int addr, Engine *engine) {
    this->engine = engine;

    Wire.begin(addr);

    Wire.onReceive(Communication::receiveData);
    Wire.onRequest(Communication::sendData);
}

void Communication::receiveData(int byteCount) {
    Communication *communication = Communication::getInstance();
    communication->command = (COMMAND) Wire.read();

    if (byteCount > 1) {
        communication->data = Wire.read();
    }
}

void Communication::sendData() {
    Communication *communication = Communication::getInstance();

    Wire.write(communication->engine->computeCommand(communication->command, communication->data));
}

Communication *Communication::getInstance() {
    if (Communication::instance == nullptr) Communication::instance = new Communication;
    return Communication::instance;
}

Communication::Communication() = default;
