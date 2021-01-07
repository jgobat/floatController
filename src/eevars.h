#ifndef _EEVARS_H
#define _EEVARS_H

enum {
    eeSERIAL,
    eeACT1_MIN,
    eeACT1_MAX,
    eeACT3_MIN,
    eeACT3_MAX,
    eeACT4_MIN,
    eeACT4_MAX,
    eeACT1_CURR_MAX,

    eeNUM_VALUES,
};

enum {
    eePASSWORD,
    eeUSERID,
    eePHONE_NUMBER,
    eeWIFI_PASSWORD,
    eeWIFI_AP,

    eeNUM_STRING_VALUES,
};

extern float f_ee[eeNUM_VALUES];
extern char s_ee[eeNUM_STRING_VALUES][16];

#endif // _EEVARS_H