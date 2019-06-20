#ifndef SSTV_TIMELAPSE_SLEEP_H
#define SSTV_TIMELAPSE_SLEEP_H

#include <Arduino.h>
#include <RtcDS3231.h>
#include <Wire.h>

class Sleep {

public:
    void init(byte wakeUpButtonPin, byte wakeUpRtcPin, RtcDS3231<TwoWire> *rtc, bool canUseForEver = false);

    void sleepForever();

    void sleepForeverOrAlarmRtc();

    void sleepForTime(uint8_t second);

    void sleepForAlarmRtc();

    void setAlarmRtc();

    static Sleep *getInstance();

    byte wakeUpHour;
    byte wakeUpMinute;
    byte wakeUpButtonPin;
    RtcDS3231<TwoWire> *rtc;

private:
    Sleep();

    static void wakeUpRtc();

    static void wakeUpButton();

    static Sleep *instance = nullptr;

    bool canUseForever;
    byte wakeUpRtcPin;
};


#endif //SSTV_TIMELAPSE_SLEEP_H
