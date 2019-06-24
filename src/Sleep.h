#ifndef SSTV_TIMELAPSE_SLEEP_H
#define SSTV_TIMELAPSE_SLEEP_H

#include <Arduino.h>
#include <RtcDS3231.h>
#include <Wire.h>

class Sleep {

public:
    static void begin(byte wakeUpButtonPin, byte wakeUpRtcPin);

    static void sleepForever();

    static void sleepForTime(uint8_t second);

    static void setWakeupMinute(byte minute);

    static void setWakeupHour(byte hour);

    static byte wakeUpButtonPin;
    static RtcDS3231<TwoWire> rtc;

private:
    static void wakeUpRtc();
    static void wakeUpButton();

    static byte wakeUpHour;
    static byte wakeUpMinute;
    static byte wakeUpRtcPin;
};


#endif //SSTV_TIMELAPSE_SLEEP_H
