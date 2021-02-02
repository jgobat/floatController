#include <stdio.h>

int
testCmd(int argc, char **argv)
{
    FILE *fp;

    fp = fopen("test.txt", "w");
    fprintf(fp, "test output to test.txt\n");
    fclose(fp);
    printf("done\n");
    
    return 0;
}