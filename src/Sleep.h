#ifndef SSTV_TIMELAPSE_SLEEP_H
#define SSTV_TIMELAPSE_SLEEP_H

#include <Arduino.h>
#include <RtcDS3231.h>
#include <Wire.h>

class Sleep {

public:
    void init(byte wakeUpButtonPin, byte wakeUpRtcPin, RtcDS3231<TwoWire> *rtc, bool canUseForEver = false);
    void sleepForever();
    void sleepForTime(uint8_t second);

    void setWakeupMinute(byte minute) { wakeUpMinute = minute; }

    void setWakeupHour(byte hour) { wakeUpHour = hour - 1; }

    static Sleep *getInstance();

    byte wakeUpButtonPin;
    RtcDS3231<TwoWire> *rtc;

private:
    Sleep();

    static void wakeUpRtc();

    static void wakeUpButton();

    static Sleep *instance = nullptr;

    byte wakeUpHour;
    byte wakeUpMinute;
    bool canUseForever;
    byte wakeUpRtcPin;
};


#endif //SSTV_TIMELAPSE_SLEEP_H
