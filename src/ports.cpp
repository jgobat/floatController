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

int
portTerminal(int argc, char **argv)
{
    int which;
    long baud;
    int  i, o;
    boolean addLF = false;

    if (argc < 3) {
        printf("Invalid. Usage: term portN baud [addlf]\n");
        return 1;
    }
    which = atoi(argv[1]);
    if (which < 1 || which > 4) {
        printf("Invalid sensor port (must be 1-4)\n.");
        return 1;
    }
    baud = atol(argv[2]);
    which --;

    if (argc == 4 && strcmp(argv[3], "addlf") == 0)
        addLF = true;

    digitalWrite(ports[which].pwrctl, 1);
    ports[which].port.begin(baud);
    while(1) {
        o = Serial.read();
        if (o == 0x04)
            break;
        else if (o > -1)
            ports[which].port.write(o);
        
        if (o == 13 && addLF)
            ports[which].port.write(10);

        i = ports[which].port.read();
        if (i > -1)
            Serial.write(i);        
    }
    ports[which].port.end();
    digitalWrite(ports[which].pwrctl, 0);

    return 0;
}