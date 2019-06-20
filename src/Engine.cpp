#include "Engine.h"
#include "Utils.h"

Engine::Engine(byte raspberryPin, float freqAprs, float freqSstv, DRA *dra) :
        raspberryPin(raspberryPin), freqAprs(freqAprs), freqSstv(freqSstv), dra(dra) {
    pinMode(raspberryPin, OUTPUT);
}

void
Engine::init(int slaveAddress, byte wakeUpButtonPin, byte wakeUpRtcPin, RtcDS3231<TwoWire> *rtc, bool canUseForEver) {
    Communication::getInstance()->init(slaveAddress, this);
    Sleep::getInstance()->init(wakeUpButtonPin, wakeUpRtcPin, rtc, canUseForEver);
    dra->init();
}

void Engine::startPi() {
    digitalWrite(raspberryPin, HIGH);
    state = PI_RUNNING;
}

void Engine::stopPi() {
    digitalWrite(raspberryPin, LOW);
    state = PI_STOPPED;
}

void Engine::loop() {
    switch (state) {
        case PI_STOPPING:
            Sleep::getInstance()->sleepForTime(30);
            stopPi();
            Sleep::getInstance()->sleepForeverOrAlarmRtc();
            break;
        case PI_STOPPED:
            startPi();
            Sleep::getInstance()->sleepForever();
            break;
        case PI_RUNNING:
            break;
    }
}

int Engine::computeCommand(COMMAND command, int data) {
    switch (command) {
        case GET_VOLTAGE:
            return readVccAtmega() / 1000;
        case GET_TEMPERATURE:
            return Sleep::getInstance()->rtc->GetTemperature();
        case GET_DRA:
            return dra->isDraDetected();
        case GET_BUTTON:
            return digitalRead(Sleep::getInstance()->wakeUpButtonPin);
        case SET_WAKEUP_HOUR:
            Sleep::getInstance()->wakeUpHour = data;
            return true;
        case SET_WAKEUP_MINUTE:
            Sleep::getInstance()->wakeUpMinute = data;
            return true;
        case SLEEP:
            dra->stopTx();
            Sleep::getInstance()->setAlarmRtc();
            state = PI_STOPPING;
            return true;
        case SET_PTT_OFF:
            dra->stopTx();
            return true;
        case SET_PTT_ON:
            dra->tx();
            return true;
        case SET_FREQ_APRS:
            dra->setFreq(freqAprs);
            return true;
        case SET_FREQ_SSTV:
            dra->setFreq(freqSstv);
            return true;
        default:
            return false;
    }
}