#include <Wire.h>
#include "Communication.h"
#include "Utils.h"

Communication *Communication::instance;

void Communication::init(int addr) {
    DPRINT(F("I2C addr 0x"));
    DPRINTLN(addr, HEX);
    Wire.begin(addr);

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
