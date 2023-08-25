#include "headers.h"

void setup()
{
    getcwd(home, 4096);
    home[strlen(home)] = '\0';

    char *text = malloc(sizeof(char) * 4096);

    strncpy(text, "$HOME=", 6);
    strcpy(text + 6, home);
    strcpy(text + strlen(text), "\n$PID=");
    char *pidStr = malloc(sizeof(char) * 10);

    pid_t pid = getpid();
    sprintf(pidStr, "%d", pid);
    strcpy(text + strlen(text), pidStr);

    strcpy(text + strlen(text), "\n$LASTCOMMAND=");
    strcpy(text + strlen(text), "\n$LASTPATH=OLDPWD");

    int file = open("shellprofile", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    truncate("shellprofile", 0);
    write(file, text, strlen(text));
    close(file);

    file = open("shellhistory", O_CREAT | O_WRONLY, 0666);
    write(file, "$COUNT=", 7);
    close(file);

    file = open("shellprocesses", O_CREAT | O_TRUNC, 0666);
    truncate("shellprocesses", 0);
    close(file);

    free(text);
    free(pidStr);

    return;
}
