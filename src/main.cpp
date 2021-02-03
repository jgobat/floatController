
#include "Arduino.h"
#include <SdFat.h>
#include <SimpleSerialShell.h>
#include "pindefs.h"
#include <printf.h>
#include <TimeLib.h>
#include <errno.h>
#include <malloc.h>
#include <opts.h>
extern "C" {
#include "expressions.h"
}

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
extern "C" float  eeGetFloat(char *);
extern char  *eeGetString(char *);

extern int portTerminal(int argc, char **argv);

extern int catCmd(int argc, char **argv);
extern int cdCmd(int argc, char **argv);
extern int mkdirCmd(int argc, char **argv);
extern int rmCmd(int argc, char **argv);
extern int renameCmd(int argc, char **argv);
extern int cpCmd(int argc, char **argv);

extern "C" int vi_main(int argc, char **argv);
extern "C" int testCmd(int argc, char **argv);
extern "C" Code initLexer(char *line);
extern "C" double EvalCode(Code);

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

    while(ms < timeout && console -> available() == 0) {
        // yield();
    }

    return console -> available();
}

int
_getchar(void)
{
    while(console -> available() == 0) {
        // yield();
    }

    return console -> read();
}

#define NFILES 5
#define FILE0 10
#define FILEN 14

SdFile files[NFILES];
int filesOpen[NFILES] = {0,0,0,0,0};
char *filesName[NFILES] = {NULL, NULL, NULL, NULL, NULL};

int 
_read(int fd, char *ptr, int n)
{
  if (fd == 0) {
    int i;
    for (i = 0 ; i < n ; i++)
      ptr[i] = _getchar();

    return n;  
  }

  if (fd < FILE0 || fd > FILEN || !filesOpen[fd - FILE0])
    return -1;

  return files[fd - FILE0].read(ptr, n);

}
 
int
_write(int fd, char *ptr, int n)
{
  if (fd == 1 || fd == 2) {
    int i;
    // loop through putchar so that we get \n -> \r\n translation
    // rather than console -> write(ptr, n) which would be more
    // efficient
    for (i = 0 ; i < n ; i++) {
      _putchar(ptr[i]);
    }
    return n;
  }

  if (fd < FILE0 || fd > FILEN || !filesOpen[fd - FILE0])
    return -1;

  return files[fd - FILE0].write(ptr, n);
}

int
_open(const char *name, int flags, int mode)
{
  int i;

  for (i = 0 ; i < NFILES ; i++) {
    if (!filesOpen[i])
      break; 
  }

  if (i == NFILES) {
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

  return i + FILE0;
}

int
_close(int fd)
{
  // what kind of error is trying to close stdin, stdout, stderr?
  if (fd >= 0 && fd <= 2)
    return 0;

  if (fd >= FILE0 && fd <= FILEN && filesOpen[fd - FILE0]) {
    files[fd - FILE0].close();
    filesOpen[fd - FILE0] = 0;
    free(filesName[fd - FILE0]);
    filesName[fd - FILE0] = NULL;
  }
  return 0;
}

int
_stat(const char *nam, struct stat *st)
{
  SdFile f;
  int i;
  int closeIt = 1;

  for (i = 0 ; i < NFILES ; i++) {
    if (filesOpen[i] && strcmp(filesName[i], nam) == 0) {
      f = files[i];
      closeIt = 0;
      break;
    }
  }    

  if (i == NFILES) {
    if (!f.open(nam, O_RDONLY)) {
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

int
lsofCmd(int argc, char **argv)
{
  int i;

  for (i = 0 ; i < NFILES ; i++)
    if (filesOpen[i])
      printf("%s\n", filesName[i]);

  return 0;
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
memCmd(int argc, char **argv)
{
  struct mallinfo mi;

  mi = mallinfo();
  if (getBooleanOpt(argc, argv, "-v"))
    printf("arena=%ld,ord=%ld,uord=%ld,ford=%ld\n", mi.arena, mi.ordblks, mi.uordblks, mi.fordblks);
  else
    printf("%ld\n", mi.uordblks);

  return 0;
}



int 
verCmd(int argc, char **argv)
{
  printf("%s %s\n", __DATE__, __TIME__);
  if (getBooleanOpt(argc, argv, "-v")) {
    uint32_t num;
    __disable_irq();
    kinetis_hsrun_disable();
		FTFL_FSTAT = FTFL_FSTAT_RDCOLERR | FTFL_FSTAT_ACCERR | FTFL_FSTAT_FPVIOL;
		*(uint32_t *)&FTFL_FCCOB3 = 0x41070000;
		FTFL_FSTAT = FTFL_FSTAT_CCIF;
		while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF)) ; // wait
		num = *(uint32_t *)&FTFL_FCCOBB;
    kinetis_hsrun_enable();
    __enable_irq();
    printf("sn: %08x\n", num);
  }
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
ifCmd(int argc, char **argv)
{
  char line[128];
  Code code;
  float x;

  if (argc < 3 || (code = initLexer(argv[1])) == NULL)
    return 1;

  x = EvalCode(code);
  FreeCode(code);
  if (x) {
    reconstituteLine(argc -2, argv + 2, line, 128);
    shell.execute(line);
  }
  else {
    return 1;
  }

  return 0; // do we care about the return value of the command 
            // or do we not want to confuse a possible trailing else
}

int
elseCmd(int argc, char **argv)
{
  char line[128];

  if (shell.lastErrNo()) {
    reconstituteLine(argc - 1, argv + 1, line, 128);
    shell.execute(line);
  }

  return shell.lastErrNo();
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
  return vi_main(argc, argv);
}

int
delayCmd(int argc, char **argv)
{
  long ms;

  if (argc < 2 || (ms = atol(argv[1])) < 0)
    return 1;

  delay(ms);
  return 0;
}

int
reset(int argc, char **argv)
{
  __asm__ volatile("bkpt");
  __builtin_unreachable();
  return 0; // silence warning
}


void 
setup()
{
  Serial1.begin(9600);
  Serial.begin(9600);
  setConsole(&Serial1, NULL); // use Serial1 when not on USB

  shell.attach(*console);
  shell.addFallback(eeParseVar);   // need it for _ and $ as commands (variable sets and reads)
  shell.addStrings(eeGetString);   // need it for $ arguments
  shell.addFloats(eeGetFloat);     // need it for _ arguments
  shell.addRedirector(setConsole); // need it for redirected commands
  shell.addSD(&sd); // need it for globbed arguments
  shell.addCommand(F("pwr"),  debugINA233, true, true, NULL);
  shell.addCommand(F("gpio"), gpioCmd, false, true, NULL);
  shell.addCommand(F("mem"), memCmd, false, true, F("mem [-v]"));
  shell.addCommand(F("ver"), verCmd, false, true, F("ver [-v]"));

  shell.addCommand(F("motor"), actuatorCmd, true, true, NULL);
  shell.addCommand(F("move"),  actuatorMove, false, true, F("move motorN target"));
  shell.addCommand(F("pos"),   actuatorReadPosition, true, true, F("pos motorN"));

  shell.addCommand(F("lsof"),  lsofCmd, false, true, NULL);
  shell.addCommand(F("vi"),    viCmd, true, true, NULL);
  shell.addCommand(F("cat"),   catCmd, true, true, F("cat filespec [> dest]"));
  shell.addCommand(F("cd"),    cdCmd, true, true, NULL);
  shell.addCommand(F("mkdir"), mkdirCmd, true, true, NULL);
  shell.addCommand(F("rm"),    rmCmd, true, true, NULL);
  shell.addCommand(F("ren"),   renameCmd, true, true, NULL);
  shell.addCommand(F("cp"),    cpCmd, true, true, NULL);
  // dir does it's own globbing so that dir *.dat isn't limited by MAXARGS
  shell.addCommand(F("dir"),   printDirectory, false, true, F("dir filespec"));

  shell.addCommand(F("clock"), clockCmd, false, true, F("clock [YYYY-mm-dd HH:MM:SS]"));

  shell.addCommand(F("defaults"), eeReset, false, true, NULL);
  shell.addCommand(F("dump"), eeDump, false, true, NULL);

  shell.addCommand(F("reset"), reset, false, true, NULL);

  shell.addCommand(F("term"), portTerminal, false, true, F("term port baud [addlf]"));
  shell.addCommand(F("script"), executeScript, true, true, F("script filename"));
  shell.addCommand(F("print"), printCmd, true, true, F("print arg1 arg2 arg3..."));
  shell.addCommand(F("time"),  timeCmd, false, true, F("time cmd args"));
  shell.addCommand(F("repeat"), repeatCmd, false, true, F("repeat N cmd args")); // call this loop?
  shell.addCommand(F("test"), testCmd, true, true, NULL);
  shell.addCommand(F("delay"), delayCmd, false, true, F("delay ms"));

  shell.addCommand(F("else"), elseCmd, false, true, F("else cmd args"));
  shell.addCommand(F("if"), ifCmd, false, false, F("if expression cmd args"));

  // waitfor "expression"
  // sleep, usleep, 
  // if "expression" command args
  // _var "expression"
  // else following any command that execs if previous command "fails" (returns 1)
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
    // printf("> ");  console -> flush();
  } 
}
