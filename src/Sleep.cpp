#include <time.h>
#include <LowPower.h>
#include "Sleep.h"
#include "Utils.h"

RtcDS3231<TwoWire> Sleep::rtc(Wire);
byte Sleep::wakeUpButtonPin;
byte Sleep::wakeUpHour;
byte Sleep::wakeUpMinute;
byte Sleep::wakeUpRtcPin;

void Sleep::begin(byte wakeUpButtonPin, byte wakeUpRtcPin) {
    Sleep::wakeUpButtonPin = wakeUpButtonPin;
    Sleep::wakeUpRtcPin = wakeUpRtcPin;

    pinMode(wakeUpButtonPin, INPUT_PULLUP);
    pinMode(wakeUpRtcPin, INPUT_PULLUP);

    DPRINT("wakeUpButtonPin: ");
    DPRINTLN(digitalRead(wakeUpButtonPin));

    Sleep::rtc.Begin();
    set_zone(ONE_HOUR);
    Sleep::rtc.Enable32kHzPin(false);
    Sleep::rtc.SetSquareWavePin(DS3231SquareWavePin_ModeAlarmOne);
    Sleep::rtc.LatchAlarmsTriggeredFlags();

    time_t now = Sleep::rtc.GetTime();
    DPRINT("RTC: ");
    DPRINTLN(ctime(&now));
}

void Sleep::wakeUpRtc() {
    DPRINTLN("wakeUpRtc");
}

void Sleep::wakeUpButton() {
    DPRINTLN("wakeUpButton");
}

void Sleep::sleepForever() {
    DPRINTLN("sleepForever ");

    DS3231AlarmOne alarm(0, wakeUpHour, wakeUpMinute, 0, DS3231AlarmOneControl_HoursMinutesSecondsMatch);
    Sleep::rtc.SetAlarmOne(alarm);

    DPRINT("Alarm1: ");
    DPRINT(alarm.Hour());
    DPRINT(":");
    DPRINTLN(alarm.Minute());

    // wakeup security
    time_t now = Sleep::rtc.GetTime();
    now += 60 * 15;
    tm *nowTm = gmtime(&now);
    DS3231AlarmTwo alarm2(0, nowTm->tm_hour, nowTm->tm_min, DS3231AlarmTwoControl_HoursMinutesMatch);
    Sleep::rtc.SetAlarmTwo(alarm2);

    DPRINT("Alarm2: ");
    DPRINT(alarm2.Hour());
    DPRINT(":");
    DPRINTLN(alarm2.Minute());

    if (!digitalRead(wakeUpButtonPin)) {

        attachInterrupt(digitalPinToInterrupt(wakeUpButtonPin), Sleep::wakeUpButton, RISING);
        attachInterrupt(digitalPinToInterrupt(wakeUpRtcPin), Sleep::wakeUpRtc, FALLING);

        LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

        detachInterrupt(digitalPinToInterrupt(wakeUpButtonPin));
        detachInterrupt(digitalPinToInterrupt(wakeUpRtcPin));
    } else {
        while (!Sleep::rtc.LatchAlarmsTriggeredFlags()) {
            now = Sleep::rtc.GetTime();
            DPRINT("RTC: ");
            DPRINTLN(ctime(&now));
            delay(10000);
        }
        wakeUpRtc();
    }

    blink(2);
}

void Sleep::sleepForTime(uint8_t seconds) {
    DPRINT("sleepForTime: ");
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
