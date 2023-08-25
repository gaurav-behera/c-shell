#include "headers.h"

int warp(int argc, char *argv[])
{
    int statusCode = 0;
    if (argc == 0)
    {
        printf(WHITE_COLOR "%s\n", home);
            printf("%s", RESET_COLOR);

        chdir(home);
    }
    for (int i = 0; i < argc; i++)
    {
        char pwd[4096];
        getcwd(pwd, 4096);
        char *newpath = getAbsolutePath(pwd, argv[i]);

        if (strncmp(newpath, "OLDPWD", 6) == 0)
        {
            printWarning("OLDPWD not set");
        }
        else if (chdir(newpath) == 0 && newpath != NULL)
        {
            printf(WHITE_COLOR "%s\n", newpath);
            printf("%s", RESET_COLOR);

            savePath(pwd);
            statusCode = 1;
        }
        else
        {
            printError();
            statusCode = 0;
            free(newpath);
            break;
        }
        // free(newpath);
    }
    return statusCode;
}
