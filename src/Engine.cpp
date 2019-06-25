#include "Engine.h"
#include "Utils.h"

float Engine::freqSstv;
float Engine::freqAprs;
Engine::STATE Engine::state = PI_STOPPED;
byte Engine::raspberryPin;

void Engine::begin(byte raspberryPin, float freqAprs, float freqSstv,
                   byte rxDra, byte txDra, byte pttPin, byte activePin,
                   int i2cAddress, byte wakeUpButtonPin, byte wakeUpRtcPin) {
    Engine::raspberryPin = raspberryPin;
    Engine::freqAprs = freqAprs;
    Engine::freqSstv = freqSstv;
    pinMode(raspberryPin, OUTPUT);
    Communication::begin(i2cAddress);
    Sleep::begin(wakeUpButtonPin, wakeUpRtcPin);
    DRA::begin(rxDra, txDra, pttPin, activePin);
}

void Engine::startPi() {
    DPRINTLN("startPi");
    digitalWrite(raspberryPin, HIGH);
    state = PI_RUNNING;
#ifdef USE_WAKEUP_SECURITY
    Sleep::setSecurityInterrupt(300); // 60s * 5min
    attachInterrupt(digitalPinToInterrupt(Sleep::wakeUpRtcPin), Engine::restartPi, FALLING);
#endif
}

void Engine::restartPi() {
    stopPi();
    delay(100);
    startPi();
}

void Engine::stopPi() {
    DPRINTLN("stopPi");
    digitalWrite(raspberryPin, LOW);
    state = PI_STOPPED;
#ifdef USE_WAKEUP_SECURITY
    detachInterrupt(digitalPinToInterrupt(Sleep::wakeUpRtcPin));
#endif
}

void Engine::loop() {
    switch (state) {
        case PI_STOPPING:
            DPRINTLN("PI_STOPPING");
            Sleep::sleepForTime(16);
            stopPi();
            Sleep::sleepForever();
            break;
        case PI_STOPPED:
            DPRINTLN("PI_STOPPED");
            startPi();
            break;
        case PI_RUNNING:
            computeCommand();
            break;
    }
}

void Engine::computeCommand() {
    if (Communication::hasCommand()) {
        COMMAND command = Communication::getCommand();
        uint8_t data = Communication::getData();
        uint16_t response = 0;

        DPRINT("I2C CMD 0x");
        DPRINT(Communication::getCommand());
        DPRINT(" ");

        switch (command) {
            case GET_VOLTAGE:
                DPRINT("GET_VOLTAGE ");
                response = readVccAtmega();
                break;
            case GET_TEMPERATURE:
                DPRINT("GET_TEMPERATURE ");
                response = (uint16_t) (Sleep::rtc.GetTemperature() * 100);
                break;
            case GET_DRA:
                DPRINT("GET_DRA ");
                response = DRA::isDraDetected();
                break;
            case GET_BUTTON:
                DPRINT("GET_BUTTON ");
                response = digitalRead(Sleep::wakeUpButtonPin);
                break;
            case SET_WAKEUP_HOUR:
                DPRINT("SET_WAKEUP_HOUR");
                response = 1;
                Sleep::setWakeupHour(data);
                break;
            case SET_WAKEUP_MINUTE:
                DPRINT("SET_WAKEUP_MINUTE");
                response = 1;
                Sleep::setWakeupMinute(data);
                break;
            case SLEEP:
                DPRINT("SLEEP ");
                response = 1;
                DRA::stopTx();
                state = PI_STOPPING;
                break;
            case SET_PTT_OFF:
                DPRINT("SET_PTT_OFF ");
                response = 1;
                DRA::stopTx(false);
                break;
            case SET_PTT_ON:
                DPRINT("SET_PTT_ON ");
                response = 1;
                DRA::tx();
                break;
            case SET_FREQ_APRS:
                DPRINT("SET_FREQ_APRS ");
                response = 1;
                DRA::setFreq(freqAprs);
                break;
            case SET_FREQ_SSTV:
                DPRINT("SET_FREQ_SSTV ");
                response = 1;
                DRA::setFreq(freqSstv);
                break;
            default:
                DPRINT("unknown ");
                break;
        }

        DPRINT(data);
        DPRINT(" ");
        DPRINTLN(response);

        Communication::setResponse(command, response);
        Communication::resetCommand();
    }
}