#include "Arduino.h"
#include "pindefs.h"
#include <printf.h>

static int actA[] = { TEENSY_ACT1CTLA, TEENSY_ACT2CTLA, TEENSY_ACT3CTLA, TEENSY_ACT4CTLA, TEENSY_ACT5CTLA };
static int actB[] = { TEENSY_ACT1CTLB, TEENSY_ACT2CTLB, TEENSY_ACT3CTLB, TEENSY_ACT4CTLB, TEENSY_ACT5CTLB };

// A=0, B=0 => stop
// A=1, B=0 => forward
// A=0, B=1 => reverse
// A=1, B=1 => brake

#define FULL 4096

int
actuatorState(int which, uint16_t A, uint16_t B)
{
    if (which < 0 || which > 4)
        return 1;

    analogWrite(actA[which], A);
    analogWrite(actB[which], B);
    return 0;
}

int
actuatorCmd(int argc, char **argv)
{
    int which;
    int pwm_A, pwm_B;

    if (argc != 3 && argc != 4) {
        printf("Invalid request. Usage: motor N [fwd|rev|brake|off] [PWM_A PWM_B]\n"); Serial.flush();
        return 1;
    }

    if ((which = atoi(argv[1])) < 0 || which > 4) {
        printf("Invalid actuator number %d (must be 0-4)\n", which);
        return 1;
    }
        
    if (argc == 3) {    
        if (strcmp(argv[2], "fwd") == 0 || strcmp(argv[2], "on") == 0) {
            actuatorState(which, FULL, 0);
            return 0;
        }
        else if (strcmp(argv[2], "rev") == 0)  {
            actuatorState(which, 0, FULL);
            return 0;
        }
        else if (strcmp(argv[2], "brake") == 0) {
            actuatorState(which, FULL, FULL);
            return 0;
        }
        else if (strcmp(argv[2], "off") == 0 || strcmp(argv[2], "stop") == 0) {
            actuatorState(which, 0, 0);
            return 0;
        }
        printf("Invalid state. Must be one of fwd|on, rev, brake, off|stop\n");
        return 1; 
    }
    else if (argc == 4) {
        pwm_A = atoi(argv[2]);
        pwm_B = atoi(argv[3]);
        if (pwm_A < 0 || pwm_B < 0 || (pwm_A > 0 && pwm_B > 0)) {
            printf("invalid actuation state\n");
            return 1;
        }
        actuatorState(which, pwm_A, pwm_B);
        return 0;
    }   
    return 1;
}

// TB67H451 U13 actuator 1 motor driver current limit is set by 
// 0.1 ohm R30. Iout = 0.1 (fixed gain) * Vref (DAC output) / 0.1 (R30)
// So Iout = Vdac with a max of 3.3A

void
actuator1CurrentLimit(float limit) // limit in A
{
    float x;

    x = (limit/3.3) * 4095.0;
    if (x < 0)
        x = 0;
    else if (x > 4095)
        x = 4095;
    
    analogWrite(TEENSY_ACT1VCURR, (int) x);
}

int
actuator3Position(void)
{
    return analogRead(TEENSY_ACT3FB);
}

int
actuator4Position(void)
{
    return analogRead(TEENSY_ACT4FB);
}

// A10 and A11 ACTVFBx signals are not mapped to a specific
// actuator connector - they have their own 4-pin connector
// with 3.3V reference and ground

int
actuatorPosition(int *fb1, int *fb2)
{
    int v1, v2;

    v1 = analogRead(TEENSY_ACTVFB1);
    v2 = analogRead(TEENSY_ACTVFB2);

    if (fb1)
        *fb1 = v1;
    if (fb2)
        *fb2 = v2;

    return (v1 + v2) / 2;
}

int
actuatorInit(void)
{
    int i;

    for (i = 0 ; i < 5 ; i++) {
        pinMode(actA[i], OUTPUT);
        pinMode(actB[i], OUTPUT);
        digitalWrite(actA[i], LOW);
        digitalWrite(actB[i], LOW);
        analogWriteFrequency(actA[i], 4000);
        analogWriteFrequency(actB[i], 4000);
    }
    analogWriteResolution(12);
    analogReadResolution(12);
    actuator1CurrentLimit(1.0);

    return 0;
}