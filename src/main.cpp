
#include "Arduino.h"
#include <SdFat.h>
#include <SimpleSerialShell.h>
#include "pindefs.h"
#include <printf.h>
#include <TimeLib.h>
#include <errno.h>

extern void initINA233(void);
extern int  debugINA233(int argc, char **argv);
extern long readINA233(float *, float *, float *, float *);

extern int  actuatorInit(void);
extern int  actuatorCmd(int argc, char **argv);
extern int  actuatorReadPosition(int argc, char **argv);
extern int  actuatorMove(int argc, char **argv);

extern int  printDirectory(int argc, char **argv);

extern void initPorts(void);

extern int    eeParseVar(int argc, char **argv);
extern void   eeInit(void);
extern int    eeDump(int argc, char **argv);
extern int    eeReset(int argc, char **argv);
extern float  eeGetFloat(char *);
extern char  *eeGetString(char *);

extern int portTerminal(int argc, char **argv);

extern int catCmd(int argc, char **argv);
extern int cdCmd(int argc, char **argv);
extern int mkdirCmd(int argc, char **argv);
extern int rmCmd(int argc, char **argv);
extern int renameCmd(int argc, char **argv);
extern int cpCmd(int argc, char **argv);

extern "C" int vi_main(int argc, char **argv);

SdFat sd;

Stream * console;
SdFile * consoleRedirect;

extern "C" {

void
_putchar(char c)
{
  if (consoleRedirect)
    consoleRedirect -> write(c);
  else { 
    if (c == '\n')
      console -> print('\r');
    
    console -> print(c);
  }
}

int
_waitchar(long timeout)
{
    elapsedMillis ms = 0;

    while(ms < timeout && Serial.available() == 0) {
        // yield();
    }

    return Serial.available();
}

int
_getchar(void)
{
    while(Serial.available() == 0) {
        // yield();
    }

    return Serial.read();
}

SdFile files[5];
int filesOpen[5] = {0,0,0,0,0};
char *filesName[5] = {NULL, NULL, NULL, NULL, NULL};

int
_readFile(int fd, char *ptr, int n)
{
  if (!filesOpen[fd])
    return -1;

  return files[fd].read(ptr, n);

}

int
_writeFile(int fd, char *ptr, int n)
{
  if (!filesOpen[fd])
    return -1;

  return files[fd].write(ptr, n);
}

int
_openFile(const char *name, int flags, int mode)
{
  int i;

  for (i = 0 ; i < 5 ; i++) {
    if (!filesOpen[i])
      break; 
  }

  if (i == 5) {
    errno = ENFILE;
    return -1;
  }
    

  if (!files[i].open(name, flags)) {
    errno = ENOENT;
    return -1;
  }

  if (filesName[i]) free(filesName[i]);
  filesName[i] = strdup(name);
  filesOpen[i] = 1;

  return i;
}

int
_closeFile(int fd)
{
  if (filesOpen[fd]) {
    files[fd].close();
    filesOpen[fd] = 0;
    free(filesName[fd]);
    filesName[fd] = NULL;
  }
  return 0;
}

int
_statFile(const char *nam, struct stat *st)
{
  SdFile f;
  int i;
  int closeIt = 1;

  for (i = 0 ; i < 5 ; i++) {
    if (filesOpen[i] && strcmp(filesName[i], nam) == 0) {
      f = files[i];
      closeIt = 0;
      break;
    }
  }    

  if (i == 5) {
    if (!f.open(nam, O_RDONLY)) {
      printf("could not open %s\n", nam);
      errno = EINVAL;
      return -1;
    }
  }

  st -> st_size = f.fileSize();
  if (f.isDir())
    st -> st_mode = S_IFDIR;
  else  
    st -> st_mode = S_IFREG;

  if (closeIt) f.close();

  return 0;
}

}

Stream *
setConsole(Stream *con, SdFile *fil)
{
  Stream * prev = console;
  console = con;
  consoleRedirect = fil;
  return prev;
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
  else {
      printf("Invalid. Usage: clock [YYYY-mm-dd HH:MM:SS]\n");
      return 1;
  }

  return 0;
}

time_t
getTeensy3Time(void)
{
  return Teensy3Clock.get();
}

int
executeScript(int argc, char **argv)
{
  SdFile  f;
  int     i;
  char    line[80];

  for (i = 1 ; i < argc ; i++) {
      if (!f.open(argv[i], O_RDONLY))
        continue;

      while(f.fgets(line, 80) > 0) {
        shell.execute(line);
      }

      f.close();
  }    

  return 0;
}

void
reconstituteLine(int argc, char **argv, char *buff, int max)
{
  int i, len;
  buff[0] = 0;

  len =  0;
  for(i = 0 ; i < argc ; i++) {
    strncat(buff, argv[i], max - len);
    strcat(buff, " ");
    len += strlen(buff);
  }  
  return;
}

int
timeCmd(int argc, char **argv)
{
  float av_mW;
  long  ms;
  char line[128];

  if (argc < 2)
    return 1;

  reconstituteLine(argc - 1, argv + 1, line, 128);
  readINA233(NULL, NULL, NULL, &av_mW);
  shell.execute(line);
  ms = readINA233(NULL, NULL, NULL, &av_mW);
  printf("elapsed ms = %lu, pwr = %f\n", ms, av_mW);
  return 0;
}

int
repeatCmd(int argc, char **argv)
{
  int i;
  int n;
  char line[128];

  if (argc < 3)
    return 1;

  n = atoi(argv[1]);
  reconstituteLine(argc - 2, argv + 2, line, 128);
  for (i = 0 ; i < n ; i++)
    shell.execute(line);

  return 0;  
}
int
printCmd(int argc, char **argv)
{
  int i;

  for (i = 1 ; i < argc ; i++)
    printf("%s\n", argv[i]);

  return 0;
}

int
viCmd(int argc, char **argv)
{
  /* struct stat st;
  int ret;

  ret = _statFile("foo.c", &st);
  printf("ret = %d, %ld\n", ret, st.st_size);

  return ret; */
  return vi_main(argc, argv); 
}

int
reset(int argc, char **argv)
{
  _reboot_Teensyduino_();
  return 0; // silence warning
}


void 
setup()
{
  Serial.begin(9600);
  setConsole(&Serial, NULL); // use Serial1 when not on USB

  shell.attach(*console);
  shell.addFallback(eeParseVar);   // need it for _ and $ as commands (variable sets and reads)
  shell.addStrings(eeGetString);   // need it for $ arguments
  shell.addFloats(eeGetFloat);     // need it for _ arguments
  shell.addRedirector(setConsole); // need it for redirected commands
  shell.addSD(&sd); // need it for globbed arguments
  shell.addCommand(F("pwr"),  debugINA233, true, NULL);
  shell.addCommand(F("gpio"), gpioCmd, false, NULL);

  shell.addCommand(F("motor"), actuatorCmd, true, NULL);
  shell.addCommand(F("move"),  actuatorMove, false, F("move motorN target"));
  shell.addCommand(F("pos"),   actuatorReadPosition, true, F("pos motorN"));

  shell.addCommand(F("vi"),    viCmd, true, NULL);
  shell.addCommand(F("cat"),   catCmd, true, F("cat filespec [> dest]"));
  shell.addCommand(F("cd"),    cdCmd, true, NULL);
  shell.addCommand(F("mkdir"), mkdirCmd, true, NULL);
  shell.addCommand(F("rm"),    rmCmd, true, NULL);
  shell.addCommand(F("ren"),   renameCmd, true, NULL);
  shell.addCommand(F("cp"),    cpCmd, true, NULL);
  // dir does it's own globbing so that dir *.dat isn't limited by MAXARGS
  shell.addCommand(F("dir"),   printDirectory, false, F("dir filespec"));

  shell.addCommand(F("clock"), clockCmd, false, F("clock [YYYY-mm-dd HH:MM:SS]"));

  shell.addCommand(F("defaults"), eeReset, false, NULL);
  shell.addCommand(F("dump"), eeDump, false, NULL);

  shell.addCommand(F("reset"), reset, false, NULL);

  shell.addCommand(F("term"), portTerminal, false, F("term port baud [addlf]"));
  shell.addCommand(F("script"), executeScript, true, F("script filename"));
  shell.addCommand(F("print"), printCmd, true, NULL);
  shell.addCommand(F("time"),  timeCmd, false, NULL);
  shell.addCommand(F("repeat"), repeatCmd, false, NULL); // call this loop?

  // waitfor "expression"
  // delay, sleep, usleep, 
  // if "expression" command args
  // _var "expression"
  // expressions, including ! to evaluate return value of shell command? or set _retval?

  setSyncProvider(getTeensy3Time);
  
  eeInit();
  initINA233();
  actuatorInit();
  initPorts();

  if (!sd.begin(SdioConfig(FIFO_SDIO))) {
    printf("filesystem init failure\n");
  }
  else {
    // printf("wd = %s\n", sd.vol()->cwd(name, 16));
  }
  printf("starting ...\n");
  printf("> ");
} 

void loop()
{
  if (shell.executeIfInput()) {
    printf("> ");  Serial.flush();
  } 
}
