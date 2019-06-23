#include <Wire.h>
#include "Communication.h"
#include "Utils.h"

Communication *Communication::instance;

void Communication::begin() {
    DPRINT(F("I2C addr 0x"));
    DPRINTLN(i2cAddress, HEX);

    Wire.begin(i2cAddress);

    Wire.onReceive(Communication::receiveData);
    Wire.onRequest(Communication::sendData);
}

void Communication::receiveData(int byteCount) {
    Communication *communication = Communication::getInstance();
    communication->command = (COMMAND) Wire.read();

    communication->data = 0;
    if (byteCount > 1) {
        communication->data = Wire.read();
        if (byteCount > 2) {
            communication->data += (uint8_t) Wire.read() << 8;
        }
    }
}

void Communication::sendData() {
    Communication *communication = Communication::getInstance();
    Wire.write((byte *) &communication->response[communication->command], 2);
}

Communication *Communication::getInstance() {
    if (Communication::instance == nullptr) Communication::instance = new Communication;
    return Communication::instance;
}

Communication::Communication() = default;

COMMAND Communication::getCommand() {
    return command;
}

uint16_t Communication::getData() {
    return data;
}

void Communication::setResponse(COMMAND commandTarget, uint16_t value) {
    response[commandTarget] = value;
}

void Communication::resetCommand() {
    command = NOTHING;
}

bool Communication::hasCommand() {
    return command != NOTHING;
}

void Communication::seti2cAddress(uint8_t addr) {
    i2cAddress = addr;
}
