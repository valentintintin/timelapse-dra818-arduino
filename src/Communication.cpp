#include <Wire.h>
#include "Communication.h"
#include "Utils.h"

COMMAND Communication::command;
uint8_t Communication::data;
uint16_t Communication::response[];

void Communication::begin(uint8_t i2cAddress) {
    DPRINT("I2C addr 0x");
    DPRINTLN(i2cAddress, HEX);

    Wire.begin(i2cAddress);

    Wire.onReceive(Communication::receiveData);
    Wire.onRequest(Communication::sendData);
}

void Communication::receiveData(int byteCount) {
    command = (COMMAND) Wire.read();

    data = 0;
    if (byteCount > 1) {
        data = Wire.read();
    }
}

void Communication::sendData() {
    byte buffer[2];
    buffer[0] = response[command] & 0xFF;
    buffer[1] = (response[command] & 0xFF00) >> 8;
    Wire.write(buffer, 2);
}

COMMAND Communication::getCommand() {
    return command;
}

uint8_t Communication::getData() {
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
