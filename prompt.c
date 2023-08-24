#include "headers.h"

void prompt()
{
    // getting Username and System Name
    char userName[4096] = "";
    char systemName[4096] = "";

    if ((gethostname(systemName, 4096) != 0) || (getlogin_r(userName, 4096) != 0))
    {
        perror(WHITE_COLOR "Error in retriving details!\n");
        exit(0);
    }

    char *pwd = malloc(sizeof(char)*4096);
    getcwd(pwd, 4096);

    char *lastCommand = getLastCommand();
    char *relPath = getRelativePathHome(pwd);

    printf(LIGHT_GREEN_COLOR "<");
    printf(LIGHT_BLUE_COLOR "%s", userName);
    printf(GREEN_COLOR "@");
    printf(CYAN_COLOR "%s", systemName);
    printf(LIGHT_GREEN_COLOR ":");
    printf(GREEN_COLOR "%s", relPath);
    printf(GRAY_COLOR "%s", lastCommand);
    printf(LIGHT_GREEN_COLOR "> " RESET_COLOR);

    free(lastCommand);
    free(relPath);
    free(pwd);

    return;
}
