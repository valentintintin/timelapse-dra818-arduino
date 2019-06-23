#include "Engine.h"
#include "Utils.h"

Engine::Engine(byte raspberryPin, float freqAprs, float freqSstv,
               byte rxDra, byte txDra, byte pttPin, byte activePin,
               int i2cAddress, byte wakeUpButtonPin, byte wakeUpRtcPin) :
        raspberryPin(raspberryPin), freqAprs(freqAprs), freqSstv(freqSstv),
        dra(new DRA(rxDra, txDra, pttPin, activePin)) {
    pinMode(raspberryPin, OUTPUT);
    Sleep::getInstance()->setPins(wakeUpButtonPin, wakeUpRtcPin);
    Communication::getInstance()->seti2cAddress(i2cAddress);
}

void Engine::begin() {
    Communication::getInstance()->begin();
    Sleep::getInstance()->begin();
    dra->begin();
}

void Engine::startPi() {
    DPRINTLN(F("startPi"));
    digitalWrite(raspberryPin, HIGH);
    state = PI_RUNNING;
}

void Engine::stopPi() {
    DPRINTLN(F("stopPi"));
    digitalWrite(raspberryPin, LOW);
    state = PI_STOPPED;
}

void Engine::loop() {
    switch (state) {
        case PI_STOPPING:
            DPRINTLN(F("PI_STOPPING"));
            Sleep::getInstance()->sleepForTime(16);
            stopPi();
            Sleep::getInstance()->sleepForever();
            break;
        case PI_STOPPED:
            DPRINTLN(F("PI_STOPPED"));
            startPi();
            break;
        case PI_RUNNING:
            computeCommand();
            break;
    }
}

void Engine::computeCommand() {
    Communication *communication = Communication::getInstance();

    if (communication->hasCommand()) {
        COMMAND command = communication->getCommand();
        uint16_t data = communication->getData();
        uint16_t response = 0;

        DPRINT(F("I2C CMD 0x"));
        DPRINT(communication->getCommand());
        DPRINT(F(" "));

        switch (command) {
            case GET_VOLTAGE:
                DPRINT(F("GET_VOLTAGE "));
                response = readVccAtmega();
                break;
            case GET_TEMPERATURE:
                DPRINT(F("GET_TEMPERATURE "));
                response = (uint16_t) (Sleep::getInstance()->rtc->GetTemperature() * 100);
                break;
            case GET_DRA:
                DPRINT(F("GET_DRA "));
                response = dra->isDraDetected();
                break;
            case GET_BUTTON:
                DPRINT(F("GET_BUTTON "));
                response = digitalRead(Sleep::getInstance()->wakeUpButtonPin);
                break;
            case SET_WAKEUP_HOUR:
                DPRINT(F("SET_WAKEUP_HOUR "));
                response = 1;
                Sleep::getInstance()->setWakeupHour(data);
                break;
            case SET_WAKEUP_MINUTE:
                DPRINT(F("SET_WAKEUP_MINUTE "));
                response = 1;
                Sleep::getInstance()->setWakeupMinute(data);
                break;
            case SLEEP:
                DPRINT(F("SLEEP "));
                response = 1;
                dra->stopTx();
                state = PI_STOPPING;
                break;
            case SET_PTT_OFF:
                DPRINT(F("SET_PTT_OFF "));
                response = 1;
                dra->stopTx(false);
                break;
            case SET_PTT_ON:
                DPRINT(F("SET_PTT_ON "));
                response = 1;
                dra->tx();
                break;
            case SET_FREQ_APRS:
                DPRINT(F("SET_FREQ_APRS "));
                response = 1;
                dra->setFreq(freqAprs);
                break;
            case SET_FREQ_SSTV:
                DPRINT(F("SET_FREQ_SSTV "));
                response = 1;
                dra->setFreq(freqSstv);
                break;
            default:
                DPRINT(F("unknown "));
                break;
        }

        DPRINT(data);
        DPRINT(F(" "));
        DPRINTLN(response);

        communication->setResponse(command, response);
        communication->resetCommand();
    }
}