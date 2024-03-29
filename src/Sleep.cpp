#include <time.h>
#include <LowPower.h>
#include "Sleep.h"
#include "Utils.h"

RtcDS3231<TwoWire> Sleep::rtc(Wire);
byte Sleep::wakeUpButtonPin;
byte Sleep::wakeUpHour = 255;
byte Sleep::wakeUpMinute = 255;
byte Sleep::wakeUpRtcPin;

void Sleep::begin(byte wakeUpButtonPin, byte wakeUpRtcPin) {
    Sleep::wakeUpButtonPin = wakeUpButtonPin;
    Sleep::wakeUpRtcPin = wakeUpRtcPin;

    pinMode(wakeUpButtonPin, INPUT_PULLUP);
    pinMode(wakeUpRtcPin, INPUT_PULLUP);

    DPRINT("wakeUpButtonPin: ");
    DPRINTLN(digitalRead(wakeUpButtonPin));

    rtc.Begin();
    set_zone(ONE_HOUR);

#ifdef DEBUG
    printCurrentTime();
#endif

    if (!rtc.GetIsRunning() || !rtc.IsDateTimeValid()) {
        DPRINTLN("RTC issue or time invalid !");
        blink(10);
    }

    rtc.Enable32kHzPin(false);
    rtc.SetSquareWavePin(DS3231SquareWavePin_ModeAlarmOne);
    rtc.LatchAlarmsTriggeredFlags();
}

void Sleep::wakeUpRtc() {
    DPRINTLN("wakeUpRtc");
}

void Sleep::wakeUpButton() {
    DPRINTLN("wakeUpButton");
}

void Sleep::sleepForever() {
    DPRINTLN("sleepForever ");

    rtc.LatchAlarmsTriggeredFlags();

    if (wakeUpHour != 255 && wakeUpMinute != 255) {
        time_t now = rtc.GetTime();
        DS3231AlarmOne alarm(0, wakeUpHour, wakeUpMinute, gmtime(&now)->tm_sec,
                             DS3231AlarmOneControl_HoursMinutesSecondsMatch);
        rtc.SetAlarmOne(alarm);
        wakeUpHour = 255;
        wakeUpMinute = 255;
    }

#ifdef DEBUG
    DS3231AlarmOne alarm = rtc.GetAlarmOne();

    DPRINT("Alarm1: ");
    DPRINT(alarm.Hour());
    DPRINT(":");
    DPRINT(alarm.Minute());
    DPRINT(":");
    DPRINTLN(alarm.Second());
#endif

#ifdef USE_WAKEUP_SECURITY
    setSecurityInterrupt(900); // 60s * 15 min
#endif

#ifdef DEBUG
    DS3231AlarmTwo alarm2 = rtc.GetAlarmTwo();

    DPRINT("Alarm2: ");
    DPRINT(alarm2.Hour());
    DPRINT(":");
    DPRINTLN(alarm2.Minute());
#endif

#ifdef DEBUG
    Serial.flush();
#endif

    if (!digitalRead(wakeUpButtonPin)) {

        attachInterrupt(digitalPinToInterrupt(wakeUpButtonPin), Sleep::wakeUpButton, RISING);
        attachInterrupt(digitalPinToInterrupt(wakeUpRtcPin), Sleep::wakeUpRtc, FALLING);

        LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

        detachInterrupt(digitalPinToInterrupt(wakeUpButtonPin));
        detachInterrupt(digitalPinToInterrupt(wakeUpRtcPin));

        rtc.LatchAlarmsTriggeredFlags();
    } else {
        while (!rtc.LatchAlarmsTriggeredFlags()) {
#ifdef DEBUG
            printCurrentTime();
            delay(10000);
#else
            delay(1000);
#endif
        }
        wakeUpRtc();
    }

    blink(2);
}

void Sleep::sleepForTime(uint8_t seconds) {
    DPRINT("sleepForTime: ");
    DPRINTLN(seconds);

#ifdef DEBUG
    Serial.flush();
#endif

    for (uint8_t i = 0; i < seconds / 4; i++) {
        if (!digitalRead(wakeUpButtonPin)) {
            LowPower.idle(SLEEP_4S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
        } else {
            delay(4000);
        }
    }

    DPRINTLN("Timeout");
    blink(2);
}

void Sleep::setWakeupMinute(byte minute) {
    wakeUpMinute = minute;
}

void Sleep::setWakeupHour(byte hour) {
    wakeUpHour = hour - 1;
}

void Sleep::setSecurityInterrupt(uint16_t secondsFromNow) {
    rtc.LatchAlarmsTriggeredFlags();

    time_t now = rtc.GetTime();
    now += secondsFromNow;
    tm *nowTm = gmtime(&now);
    DS3231AlarmTwo alarm2(0, nowTm->tm_hour, nowTm->tm_min, DS3231AlarmTwoControl_HoursMinutesMatch);
    rtc.SetAlarmTwo(alarm2);

    DPRINT("Alarm2: ");
    DPRINT(alarm2.Hour());
    DPRINT(":");
    DPRINTLN(alarm2.Minute());
}

void Sleep::printCurrentTime() {
    time_t now = rtc.GetTime();
    DPRINT("RTC: ");
    DPRINTLN(ctime(&now));
}