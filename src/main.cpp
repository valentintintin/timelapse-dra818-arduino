#include <Arduino.h>
#include <RtcDS3231.h>
#include <Wire.h>

#include "Utils.h"
#include "DRA.h"
#include "Communication.h"
#include "Sleep.h"
#include "Engine.h"

#define DRA_PTT 4
#define RX_DRA 5
#define TX_DRA 6
#define DRA_ACTIVE 7

#define WAKEUP_BUTTON_PIN 2
#define WAKEUP_RTC_PIN 3
#define RASPBERRY_PI_PIN 9

#ifdef TEST
#define FREQ_APRS 144.600
#else
#define FREQ_APRS 144.800
#endif
#define FREQ_SSTV 144.300

#define SLAVE_ADDRESS 0x11

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

#ifdef DEBUG
    Serial.begin(9600);
#endif
    DPRINTLN("Starting ...");

    Engine::begin(RASPBERRY_PI_PIN, FREQ_APRS, FREQ_SSTV,
                  RX_DRA, TX_DRA, DRA_PTT, DRA_ACTIVE,
                  SLAVE_ADDRESS, WAKEUP_BUTTON_PIN, WAKEUP_RTC_PIN
    );

    DPRINTLN("Started !");
}

void loop() {
    Engine::loop();

    delay(100);
}
