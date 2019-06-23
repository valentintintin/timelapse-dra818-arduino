#ifndef SSTV_TIMELAPSE_SLEEP_H
#define SSTV_TIMELAPSE_SLEEP_H

#include <Arduino.h>
#include <RtcDS3231.h>
#include <Wire.h>

class Sleep {

public:
    static Sleep *getInstance();

    void setPins(byte wakeUpButtonPin, byte wakeUpRtcPin);

    void begin();
    void sleepForever();
    void sleepForTime(uint8_t second);

    void setWakeupMinute(byte minute);

    void setWakeupHour(byte hour);

    byte wakeUpButtonPin{};
    RtcDS3231<TwoWire> *rtc;

private:
    static void wakeUpRtc();
    static void wakeUpButton();

    static Sleep *instance = nullptr;

    Sleep();

    byte wakeUpHour;
    byte wakeUpMinute;
    byte wakeUpRtcPin;
};


#endif //SSTV_TIMELAPSE_SLEEP_H
