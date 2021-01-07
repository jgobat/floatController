#include "Arduino.h"
#include "pindefs.h"
#include <printf.h>

typedef struct {
    int pwrctl;
    HardwareSerial port;
} SensorPort;

SensorPort ports[] = {
    { TEENSY_PWRCTL2, Serial2 },
    { TEENSY_PWRCTL3, Serial3 },
    { TEENSY_PWRCTL5, Serial5 },
    { TEENSY_PWRCTL6, Serial6 }
};

void
initPorts(void)
{
    int i;

    for (i = 0 ; i < 4 ; i++) {
        pinMode(ports[i].pwrctl, OUTPUT);
        digitalWrite(ports[i].pwrctl, LOW);
    }
}