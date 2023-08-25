#include "headers.h"

void prompt()
{
    // getting Username and System Name
    char systemName[4096] = "";
    uid_t uid = getuid();
    struct passwd *name = getpwuid(uid);

    if ((gethostname(systemName, 4096) == -1) || (name == NULL))
    {
        printError();
        exit(0);
    }

    char *pwd = malloc(sizeof(char)*4096);
    getcwd(pwd, 4096);

    char *lastCommand = getLastCommand();
    char *relPath = getRelativePathHome(pwd);

    printf(LIGHT_GREEN_COLOR "<");
    printf(LIGHT_BLUE_COLOR "%s", name->pw_name);
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
