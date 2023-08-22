#include "headers.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void prompt()
{
    // getting Username and System Name
    char *userName = getlogin();
    char systemName[256];
    int sysNameStatus = gethostname(systemName, 256);

    if (userName == NULL || sysNameStatus != 0)
    {
        printf(WHITE_COLOR "Error in retriving details!\n");
        exit(0);
    }

    char pwd[4096];
    getcwd(pwd, 4096);

    printf(GREEN_COLOR "<");
    printf(LIGHT_BLUE_COLOR "%s", userName);
    printf(LIGHT_GRAY_COLOR "@");
    printf(CYAN_COLOR "%s", systemName);
    printf(GREEN_COLOR ":");
    printf(GREEN_COLOR"%s", getRelativePathHome(pwd));
    printf(GRAY_COLOR"%s", getLastCommand());
    printf(GREEN_COLOR"> " RESET_COLOR);
}
