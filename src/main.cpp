
#include "Arduino.h"
#include <SimpleSerialShell.h>
#include "pindefs.h"
#include <printf.h>
#include <SD.h>


extern void initINA233(void);
extern int debugINA233(int argc, char **argv);
extern int actuatorInit(void);
extern int actuatorCmd(int argc, char **argv);
extern int printDirectory(int argc, char **argv);

void
_putchar(char c)
{
  Serial.print(c);
}

void 
setup()
{
  Serial.begin(9600);
  shell.attach(Serial);

  shell.addCommand(F("pwr"), debugINA233);
  shell.addCommand(F("motor"), actuatorCmd);
  shell.addCommand(F("dir"), printDirectory);

  initINA233();
  actuatorInit();

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