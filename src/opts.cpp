#include <string.h>
#include <math.h>
#include <stdlib.h>

int 
getBooleanOpt(int argc, char **argv, const char *flag)
{
    int i;

    for (i = 0 ; i < argc ; i++) {
        if (strcmp(argv[i], flag) == 0) {
            argv[i][0] = '\0';
            return 1;
        }
    }
    return 0;
}

int
getFloatOpt(int argc, char **argv, const char *flag, float *val)
{
    int i;

    *val = NAN;
    for (i = 0 ; i < argc - 1 ; i++) {
        if (strcmp(argv[i], flag) == 0 && argv[i+1][0] != '\0') {
            *val = atof(argv[i+1]);
            argv[i][0] = '\0';
            argv[i+1][0] = '\0';
            return 1;
        }
    }
    return 0;
}

char *
getStringOpt(int argc, char **argv, const char *flag)
{
    int i;

    for (i = 0 ;i < argc - 1 ; i++) {
        if (strcmp(argv[i], flag) == 0 && argv[i+1][0] != '\0') {
            argv[i][0] = '\0';
            return argv[i+1];
        }
    }
    return NULL;
}
