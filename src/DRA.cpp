#include "DRA.h"
#include "Utils.h"

DRA818 *DRA::dra;
SoftwareSerial *DRA::serial;
byte DRA::pttPin;
byte DRA::activePin;
bool DRA::activeState = false;
bool DRA::txState = false;

bool DRA::begin(byte rxDra, byte txDra, byte pttPin, byte activePin,
                float txFreq, bool deactiveAfter, char loop) {
    DRA::pttPin = pttPin;
    DRA::activePin = activePin;
    DRA::activeState = false;
    DRA::txState = false;

    delete dra;
    serial = new SoftwareSerial(rxDra, txDra);
    serial->begin(9600);

    pinMode(pttPin, OUTPUT);
    pinMode(activePin, OUTPUT);

    digitalWrite(pttPin, HIGH);
    active();

    char i = loop;
    do {
        i--;
        DPRINTLN("DRA begin ...");
        if (!(dra = DRA818::configure(serial, DRA818_VHF, txFreq, txFreq, 0, 0, 0, 0, DRA818_12K5, false, false, false,
                                      &Serial))) {
            DPRINTLN("DRA failed");
            blink(5);
        }
    } while (dra == nullptr && i > 0);

    if (isDraDetected()) {
        if (deactiveAfter) {
            deactive();
        }

        DPRINTLN("DRA OK");

        return true;
    }

    DPRINTLN("No DRA");

    return false;
}

void DRA::tx() {
    if (isDraDetected()) {
        if (!activeState) {
            active();
        }

        digitalWrite(pttPin, LOW);
        delay(TIME_TOGGLE_PTT);

        txState = true;
        DPRINTLN("DRA TX");
    }
}

void DRA::stopTx(bool deactiveAfter) {
    if (isDraDetected()) {
        delay(100);

        digitalWrite(pttPin, HIGH);
        delay(TIME_TOGGLE_PTT);

        txState = false;
        DPRINTLN("DRA stop TX");

        if (deactiveAfter) {
            deactive();
        }
    }
}

bool DRA::isActive() {
    return activeState;
}

bool DRA::isTx() {
    return txState;
}

void DRA::active() {
    digitalWrite(activePin, HIGH);
    delay(TIME_TOGGLE_ACTIVE);

    activeState = true;
    DPRINTLN("DRA active");
}

void DRA::deactive() {
    digitalWrite(activePin, LOW);

    activeState = false;
    DPRINTLN("DRA inactive");
}

bool DRA::isDraDetected() {
    return dra != nullptr;
}

void DRA::setFreq(float freq, bool deactiveAfter) {
    if (isDraDetected()) {
        if (!isActive()) {
            active();
        }

        delay(100);

        dra->group(0, freq, freq, 0, 0, 0);

        DPRINT("DRA freq ");
        DPRINTLN(freq);

        delay(100);

        if (deactiveAfter) {
            deactive();
        }
    }
}
