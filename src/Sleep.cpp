#include <time.h>
#include <LowPower.h>
#include "Sleep.h"
#include "Utils.h"

Sleep *Sleep::instance;

Sleep::Sleep() = default;

Sleep *Sleep::getInstance() {
    if (Sleep::instance == nullptr) Sleep::instance = new Sleep;
    return Sleep::instance;
}

void Sleep::init(byte wakeUpButtonPin, byte wakeUpRtcPin, RtcDS3231<TwoWire> *rtc, bool canUseForEver) {
    this->wakeUpButtonPin = wakeUpButtonPin;
    this->wakeUpRtcPin = wakeUpRtcPin;
    this->canUseForever = canUseForEver;
    this->rtc = rtc;

    pinMode(wakeUpButtonPin, INPUT_PULLUP);
    pinMode(wakeUpRtcPin, INPUT_PULLUP);

    rtc->Begin();
    set_zone(ONE_HOUR);
    rtc->Enable32kHzPin(false);
    rtc->SetSquareWavePin(DS3231SquareWavePin_ModeAlarmOne);
    rtc->LatchAlarmsTriggeredFlags();
}

void Sleep::wakeUpRtc() {
    DPRINTLN(F("Wakeup by RTC !"));
//    RTC.alarm(ALARM_1);
}

void Sleep::wakeUpButton() {
    DPRINTLN(F("Wakeup by button !"));
//    RTC.alarm(ALARM_1);
}

void Sleep::sleepForever() {
    if (!digitalRead(wakeUpButtonPin)) {
        attachInterrupt(digitalPinToInterrupt(wakeUpRtcPin), Sleep::wakeUpRtc, FALLING);
        attachInterrupt(digitalPinToInterrupt(wakeUpButtonPin), Sleep::wakeUpButton, RISING);

        LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
                      SPI_OFF, USART0_OFF, TWI_OFF);

        detachInterrupt(0);
        detachInterrupt(1);

        rtc->LatchAlarmsTriggeredFlags();
    }
}

void Sleep::sleepForeverOrAlarmRtc() {
    if (canUseForever) {
        sleepForever();
    } else {
        sleepForAlarmRtc();
    }
}

void Sleep::sleepForTime(uint8_t seconds) {
    if (!digitalRead(wakeUpButtonPin)) {
        for (uint8_t i = 0; i < seconds / 8; i++) {
            LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
                          SPI_OFF, USART0_OFF, TWI_OFF);
        }
    }
}

void Sleep::sleepForAlarmRtc() {
    if (!digitalRead(wakeUpButtonPin)) {
        while (!(rtc->LatchAlarmsTriggeredFlags() & DS3231AlarmFlag_Alarm1)) {
            LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
                          SPI_OFF, USART0_OFF, TWI_OFF);
        }
    }
}

void Sleep::setAlarmRtc() {
    DS3231AlarmOne alarm(0, wakeUpHour, wakeUpMinute, 0, DS3231AlarmOneControl_HoursMinutesSecondsMatch);
    rtc->SetAlarmOne(alarm);
}