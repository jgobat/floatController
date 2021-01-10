#include "Arduino.h"
#include <SPI.h>
#include <SdFat.h>
#include <printf.h>
#include <fnmatch.h>

extern SdFat sd;
extern void __putchar(char c);

int
catCmd(int argc, char **argv)
{
    SdFile f;
    char   *buff;
    int     n,i,j,done = 0;

    buff = (char *) malloc(sizeof(char) * 1024);

    for (i = 1 ; i < argc ; i++) {
        if (!f.open(argv[i]))
            continue;

        while((n = f.read(buff, 1024)) > 0) {
            for (j = 0 ; j < n ; j++)
                _putchar(buff[j]);
        }    
        done ++;
        f.close();
    }
    free(buff);
    return done > 0 ? 0 : 1;
}

int
cdCmd(int argc, char **argv)
{
    if (argc > 1) {
        sd.chdir(argv[1]);
        return 0;
    }    
    return 1;
}

int 
rmCmd(int argc, char **argv)
{
    int i;

    for (i = 1 ; i < argc ; i++)
        sd.remove(argv[i]);

    return 0;
}

int
mkdirCmd(int argc, char **argv)
{
    int i;

    for (i = 1 ; i < argc ; i++) {
        sd.mkdir(argv[i], false);
    }
    return 0;
}

int
renameCmd(int argc, char **argv)
{
    if (argc == 3)
        return sd.rename(argv[1], argv[2]);

    return 1;    
}

int
cpCmd(int argc, char **argv)
{
    uint8_t *buff;
    SdFile in, out;
    int    n;

    if (argc != 3)
        return 1;

    if (!in.open(argv[1]))
        return 1;
    if (!out.open(argv[1])) {
        in.close();
        return 1;
    }
    buff = (uint8_t *) malloc(sizeof(char) * 1024);

    while((n = in.read(buff, 1024)) > 0)
        out.write(buff, n);
        
    in.close();
    out.close();
    free(buff);
    return 0;
}
int
printDirectory(int argc, char **argv)
{
    SdFile root;
    SdFile entry;
    char   buff[16];
    boolean match;
    int     i;
    uint16_t date, tim;
    uint64_t size;
    char cwd[16];

    sd.vol()->cwd(cwd, 16);

    if (!root.open(cwd)) {
        printf("root open fail\n");
        return 1;
    }    

    while (entry.openNext(&root, O_RDONLY)) {   
        entry.getName(buff, 16);  
        match = false;

        if (argc == 1)
            match = true;
        else {
            for (i = 1 ; i < argc ; i++) {
                if (fnmatch(argv[i], buff, 0) == 0) {
                    match = true;
                    break;
                }
            }
        }
        if (match) {
            entry.getModifyDateTime(&date, &tim);
            size = entry.fileSize();

            printf("%04d-%02d-%02d %02d:%02d %9llu %s\n", 
                    1980 + (date >> 9), (date >> 5) & 15, (date & 31),
                    (tim >> 11), (tim >> 5) & 63, 
                    size, buff);
        }
        entry.close();
    }
    root.close();
    // sd.ls(&Serial, "/", LS_DATE | LS_SIZE);
    return 0;
}