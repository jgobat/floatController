#include "Arduino.h"
#include <SPI.h>
#include <SD.h>
#include <printf.h>

int
printDirectory(int argc, char **argv)
{
    File root = SD.open("/");
    File entry;

    while(1) {
        if (!(entry = root.openNextFile()))
            break;
        
        printf("%-12s\n", entry.name());
        entry.close();
    }
    root.close();
}