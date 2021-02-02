#include "Arduino.h"
#include <EEPROM.h>
#include "eevars.h"
#include <printf.h>

typedef struct {
    const char *name;
    float deflt;
} eeVariable;

eeVariable eeVar[] = {
    { "id", 0 },
    { "min1", 100 },
    { "max1", 3500 },
    { "min3", 100 },
    { "max3", 3500 },
    { "min4", 100 },
    { "max4", 3500 },
    { "curr_max", 3.3 },
};

typedef struct {
    const char *name;
    const char *deflt;
} eeStringVariable;

eeStringVariable eeStringVar[] = {
    { "password", "abc123"       },
    { "userid",   "float"        },
    { "phone",    "881600000000" },
    { "wifi",     "password"     },
    { "ap",       "WiFiAP"       } 
};

float f_ee[eeNUM_VALUES];
char s_ee[eeNUM_STRING_VALUES][16];

#define FLOAT_BASE 0
#define STRING_BASE 512

void
eeSaveVar(int i)
{
    EEPROM.put(FLOAT_BASE + i*4, f_ee[i]);
}

void
eeSaveStringVar(int i)
{
    eeprom_write_block(s_ee[i], (void *) (STRING_BASE + i*16), 16);
}

int
eeSearch(char *name)
{
    int i;

    for (i = 0 ; i < eeNUM_VALUES ; i++) {
        if (strcmp(name, eeVar[i].name) == 0) {
            return i;
        }    
    }

    return -1;
}

int
eeStringSearch(char *name)
{
    int i;

    for (i = 0 ; i < eeNUM_STRING_VALUES ; i++) {
        if (strcmp(name, eeStringVar[i].name) == 0)
            return i;
    }

    return -1;
}

char *
eeGetString(char *name)
{
    int idx;

    idx = eeStringSearch(name);
    if (idx > -1)
        return s_ee[idx];

    return NULL;    
}

extern "C" float eeGetFloat(char *);

float
eeGetFloat(char *name)
{
    int idx;

    idx = eeSearch(name);
    if (idx > -1)
        return f_ee[idx];

    return NAN;    
}

int
eeParseFloatVar(int argc, char **argv)
{
    int   i;
    
    if ((i = eeSearch(argv[0] + 1)) < 0)
        return 1;

    if (argc == 1)
        printf("%s[%d] = %f\n", argv[0] + 1, i, f_ee[i]);
    else {
        f_ee[i] = atof(argv[1]);
        eeSaveVar(i);    
    }

    return 0;
}

int
eeParseStringVar(int argc, char **argv)
{
    int   i;

    if ((i = eeStringSearch(argv[0] + 1)) < 0)
        return 1;

    if (argc == 1)
        printf("%s[%d] = %s\n", argv[0] + 1, i, s_ee[i]);
    else {
        bzero(s_ee[i], 16);
        strncpy(s_ee[i], argv[1], 15);
        eeSaveStringVar(i);
    }

    return 0;
}

int
eeParseVar(int argc, char **argv)
{
    if ((argc != 1 && argc != 2) || (argv[0][0] != '_' && argv[0][0] != '$')) 
        return 1;

    if (argv[0][0] == '_')
        return eeParseFloatVar(argc, argv);
    else if (argv[0][0] == '$')
        return eeParseStringVar(argc, argv);

    return 1;
}


int
eeReset(int argc, char **argv)
{
    int i;

    for (i = 0 ; i < eeNUM_VALUES ; i++) {
        f_ee[i] = eeVar[i].deflt;
        eeSaveVar(i);
    }
    for (i = 0 ; i < eeNUM_STRING_VALUES ; i++) {
        strncpy(s_ee[i], eeStringVar[i].deflt, 15);
        s_ee[i][15] = 0;
        eeSaveStringVar(i);
    }

    return 0;
}

int
eeDump(int argc, char **argv)
{
    int i;

    for (i = 0 ; i < eeNUM_VALUES ; i++) {
        printf("%s[%d] = %f\n", eeVar[i].name, i, f_ee[i]);
    }    
    for (i = 0 ; i < eeNUM_STRING_VALUES ; i++) {
        printf("%s[%d] = %s\n", eeStringVar[i].name, i, s_ee[i]);
    }
    return 0;
}

void
eeInit(void)
{
    int i;

    for (i = 0 ; i < eeNUM_VALUES ; i++) {
        eeprom_read_block(&(f_ee[i]), (void *) (FLOAT_BASE + i*4), 4);
    }    
    for (i = 0 ; i < eeNUM_STRING_VALUES ; i++) {
        eeprom_read_block(s_ee[i], (void *) (STRING_BASE + i*16), 16);
        s_ee[i][15] = 0;
    }
}