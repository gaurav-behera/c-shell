#include "headers.h"

int warp(int argc, char *argv[])
{
    int statusCode = 0;
    if (argc == 0)
    {   
        printf(WHITE_COLOR"%s\n"RESET_COLOR, home);
        chdir(home);
    }
    for (int i = 0; i < argc; i++)
    {
        char pwd[4096];
        getcwd(pwd, 4096);
        char *newpath = getAbsolutePath(pwd, argv[i]);

        if (chdir(newpath) == 0 && newpath != NULL)
        {
        printf(WHITE_COLOR"%s\n", newpath);
            savePath(pwd);
            statusCode = 1;
        }
        else
        {
            // printf("Failed to change directory\n");
            // perror(RED_COLOR"ERROR" LIGHT_RED_COLOR);
            // printf(RESET_COLOR);
            printError();
            statusCode = 0;
            free(newpath);
            break;
        }
        // free(newpath);
    }
    return statusCode;
}
