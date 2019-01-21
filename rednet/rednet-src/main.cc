
#include "urednet.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void start_screen()
{
    fprintf(stderr, "*************************************************\n");
    fprintf(stderr, "*                                               *\n");
    fprintf(stderr, "*                   Rednet                      *\n");
    fprintf(stderr, "*                                               *\n");
    fprintf(stderr, "*************************************************\n");
    fprintf(stderr, "Start ------->>>> %s\n", REDNET_VERSION);
}

void end_screen()
{
    fprintf(stderr, "Exit\n");
}

int main(int argc, char const *argv[])
{
    start_screen();
    const char *filename = NULL;
    if (argc >= 2)
        filename = argv[1];
    rednet::startup(filename);
    end_screen();
    return 0;
}
