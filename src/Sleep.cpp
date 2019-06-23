#include <time.h>
#include <LowPower.h>
#include "Sleep.h"
#include "Utils.h"

Sleep *Sleep::instance;

Sleep *Sleep::getInstance() {
    if (Sleep::instance == nullptr) Sleep::instance = new Sleep;
    return Sleep::instance;
}

Sleep::Sleep() : rtc(new RtcDS3231<TwoWire>(Wire)) {
}

void Sleep::begin() {
    pinMode(wakeUpButtonPin, INPUT_PULLUP);
    pinMode(wakeUpRtcPin, INPUT_PULLUP);

    DPRINT(F("wakeUpButtonPin: "));
    DPRINTLN(digitalRead(wakeUpButtonPin));

    rtc->Begin();
    set_zone(ONE_HOUR);
    rtc->Enable32kHzPin(false);
    rtc->SetSquareWavePin(DS3231SquareWavePin_ModeAlarmOne);
    rtc->LatchAlarmsTriggeredFlags();

    time_t now = rtc->GetTime();
    DPRINT(F("RTC: "));
    DPRINTLN(ctime(&now));
}

void Sleep::wakeUpRtc() {
    DPRINTLN(F("wakeUpRtc"));
}

void Sleep::wakeUpButton() {
    DPRINTLN(F("wakeUpButton"));
}

void Sleep::sleepForever() {
    DPRINTLN(F("sleepForever "));

    DS3231AlarmOne alarm(0, wakeUpHour, wakeUpMinute, 0, DS3231AlarmOneControl_HoursMinutesSecondsMatch);
    rtc->SetAlarmOne(alarm);

    DPRINT(F("Alarm1: "));
    DPRINT(alarm.Hour());
    DPRINT(F(":"));
    DPRINTLN(alarm.Minute());

    // wakeup security
    time_t now = rtc->GetTime();
    now += 60 * 15;
    tm *nowTm = gmtime(&now);
    DS3231AlarmTwo alarm2(0, nowTm->tm_hour, nowTm->tm_min, DS3231AlarmTwoControl_HoursMinutesMatch);
    rtc->SetAlarmTwo(alarm2);

    DPRINT(F("Alarm2: "));
    DPRINT(alarm2.Hour());
    DPRINT(F(":"));
    DPRINTLN(alarm2.Minute());

    if (!digitalRead(wakeUpButtonPin)) {

        attachInterrupt(digitalPinToInterrupt(wakeUpButtonPin), Sleep::wakeUpButton, RISING);
        attachInterrupt(digitalPinToInterrupt(wakeUpRtcPin), Sleep::wakeUpRtc, FALLING);

        LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

        detachInterrupt(digitalPinToInterrupt(wakeUpButtonPin));
        detachInterrupt(digitalPinToInterrupt(wakeUpRtcPin));
    } else {
        while (!rtc->LatchAlarmsTriggeredFlags()) {
            now = rtc->GetTime();
            DPRINT(F("RTC: "));
            DPRINTLN(ctime(&now));
            delay(10000);
        }
        wakeUpRtc();
    }

    blink(2);
}

void Sleep::sleepForTime(uint8_t seconds) {
    DPRINT(F("sleepForTime: "));
    DPRINTLN(seconds);

    for (uint8_t i = 0; i < seconds / 8; i++) {
        if (!digitalRead(wakeUpButtonPin)) {
            LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
        } else {
            delay(8000);
        }
    }

    blink(2);
}

void Sleep::setWakeupMinute(byte minute) {
    wakeUpMinute = minute;
}

void Sleep::setWakeupHour(byte hour) {
    wakeUpHour = hour - 1;
}

void Sleep::setPins(byte wakeUpButtonPin, byte wakeUpRtcPin) {
    this->wakeUpButtonPin = wakeUpButtonPin;
    this->wakeUpRtcPin = wakeUpRtcPin;
}
