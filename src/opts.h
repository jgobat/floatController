#ifndef _OPTS_H
#define _OPTS_H

int getBooleanOpt(int argc, char **argv, const char *opt);
int getFloatOpt(int argc, char **argv, const char *opt, float *val);
char  *getStringOpt(int argc, char **argv, const char *opt);

#endif // _OPTS_H