
#include "Arduino.h"
#include <SimpleSerialShell.h>
#include "pindefs.h"
#include <printf.h>
#include <SD.h>
#include <TimeLib.h>

extern void initINA233(void);
extern int  debugINA233(int argc, char **argv);
extern int  actuatorInit(void);
extern int  actuatorCmd(int argc, char **argv);
extern int  actuatorReadPosition(int argc, char **argv);
extern int  printDirectory(int argc, char **argv);
extern void initPorts(void);
extern int  eeParseVar(int argc, char **argv);
extern void eeInit(void);
extern int eeDump(int argc, char **argv);
extern int eeReset(int argc, char **argv);

void
_putchar(char c)
{
  Serial.print(c);
}

int 
gpioCmd(int argc, char **argv)
{
    int pin;
    int state;
    
    if (argc != 3) {
        printf("Invalid request. Usage: gpio PIN STATE\n");
        return 1;
    }

    pin = atoi(argv[1]);
    state = atoi(argv[2]);

    digitalWrite(pin, state);
    return 0;
}

int
clockCmd(int argc, char **argv)
{
  time_t t;
  int yr, mo, da;
  int hr, mi, se;
  int n1, n2;

  if (argc == 1) {
    t = Teensy3Clock.get();
    printf("%ld %04d-%02d-%02dT%02d:%02d:%02d\n", t, year(), month(), day(), hour(), minute(), second());
  }
  else if (argc == 3) {
    n1 = sscanf(argv[1], "%04d-%02d-%02d", &yr, &mo, &da);
    n2 = sscanf(argv[2], "%02d:%02d:%02d", &hr, &mi, &se);
    if (n1 == 3 && n2 == 3) {
      setTime(hr, mi, se, da, mo, yr);
      Teensy3Clock.set(now());
    }  
  }  
  return 0;
}

time_t
getTeensy3Time(void)
{
  return Teensy3Clock.get();
}

void 
setup()
{
  Serial.begin(9600);
  shell.attach(Serial);
  shell.addFallback(eeParseVar);
  shell.addCommand(F("pwr"), debugINA233);
  shell.addCommand(F("motor"), actuatorCmd);
  shell.addCommand(F("dir"), printDirectory);
  shell.addCommand(F("pos"), actuatorReadPosition);
  shell.addCommand(F("gpio"), gpioCmd);
  shell.addCommand(F("clock"), clockCmd);
  shell.addCommand(F("defaults"), eeReset);
  shell.addCommand(F("dump"), eeDump);

  setSyncProvider(getTeensy3Time);
  
  eeInit();
  initINA233();
  actuatorInit();
  initPorts();

  if (!SD.begin(BUILTIN_SDCARD)) {
    printf("filesystem init failure\n");
  }
  printf("> ");
} 

void loop()
{

  if (shell.executeIfInput()) {
    printf("> ");  Serial.flush();
  } 

}