#include "headers.h"

void saveProcess(pid_t pid, char *name)
{
    char strPID[10] = "";
    sprintf(strPID, "%d", pid);

    char text[4096] = "";
    strcpy(text, strPID);
    strcpy(text + strlen(text), ":");
    strcpy(text + strlen(text), name);
    strcpy(text + strlen(text), "\n");

    char shellprocessespath[4096] = "";
    strcpy(shellprocessespath, home);
    strcpy(shellprocessespath + strlen(home), "/shellprocesses");
    int file = open(shellprocessespath, O_WRONLY);
    lseek(file, 0, SEEK_END);
    write(file, text, strlen(text));
    close(file);
}

void removeProcess(pid_t pid)
{
    char strPID[10] = "";
    sprintf(strPID, "%d", pid);
    strPID[strlen(strPID)] = ':';

    char shellprocessespath[4096] = "";
    strcpy(shellprocessespath, home);
    strcpy(shellprocessespath + strlen(home), "/shellprocesses");
    int file = open(shellprocessespath, O_RDONLY);
    char text[4096] = "";
    read(file, text, 4096);
    close(file);

    int nextPos = -1, currentPos = 0, startPos = 0;
    ;
    bool found = false;
    for (currentPos = 0; currentPos < strlen(text); currentPos++)
    {
        if (found && text[currentPos] == '\n')
        {
            nextPos = currentPos;
            break;
        }
        if (strncmp(text + currentPos, strPID, strlen(strPID)) == 0)
        {
            found = true;
            startPos = currentPos;
        }
    }
    file = open(shellprocessespath, O_WRONLY | O_TRUNC);
    truncate(shellprocessespath, 0);
    lseek(file, 0, SEEK_SET);
    write(file, text, startPos);
    write(file, text + nextPos + 1, strlen(text + nextPos + 1));
    close(file);
}

char *getProcessName(pid_t pid)
{
    char strPID[10] = "";
    sprintf(strPID, "%d", pid);
    strPID[strlen(strPID)] = ':';

    char shellprocessespath[4096] = "";
    strcpy(shellprocessespath, home);
    strcpy(shellprocessespath + strlen(home), "/shellprocesses");
    int file = open(shellprocessespath, O_RDONLY);
    char text[4096] = "";
    read(file, text, 4096);
    close(file);
    int endPos = -1, currentPos = 0, startPos = 0;
    bool found = false;
    for (currentPos = 0; currentPos < strlen(text); currentPos++)
    {
        if (found && text[currentPos] == ':')
        {
            startPos = currentPos + 1;
        }
        if (found && text[currentPos] == '\n')
        {
            endPos = currentPos;
            break;
        }
        if (strncmp(text + currentPos, strPID, strlen(strPID)) == 0)
        {
            found = true;
        }
    }
    char *processName = malloc(sizeof(char) * 4096);
    memset(processName, '\0', 4096);
    strncpy(processName, text + startPos, endPos - startPos);
    return processName;
}

void checkBackgroundCompletion()
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (WIFEXITED(status))
        {
            printf("%s exited normally (%d)\n", getProcessName(pid), pid);
            removeProcess(pid);
        }
        else if (WIFSIGNALED(status))
        {
            printf("%s exited abnormally (%d)\n", getProcessName(pid), pid);
            removeProcess(pid);
        }
    }
}

void executeCommand(char *command, char delimiter)
{
    char spaceDelimiters[9] = " \n\t\r\b\f\0";
    char *token = strtok(command, spaceDelimiters);
    if (token == NULL)
    {
        return;
    }

    char *function = malloc(sizeof(char) * 4096);
    strcpy(function, token);
    function[strlen(token)] = '\0';

    char *arguments[strlen(command)];
    int argCount = 0;

    while (token != NULL)
    {
        // printf("\tToken: %s\n", token);
        token = strtok(NULL, spaceDelimiters);
        if (token != NULL)
        {
            arguments[argCount++] = token;
            // free(token);
        }
    }

    int successfulRun = 0;
    if (strcmp(function, "warp") == 0)
    {
        successfulRun = warp(argCount, arguments);
    }
    else if (strcmp(function, "peek") == 0)
    {
        successfulRun = peek(argCount, arguments);
    }
    else if (strcmp(function, "seek") == 0)
    {
        successfulRun = seek(argCount, arguments);
    }
    else if (strcmp(function, "pastevents") == 0)
    {
        successfulRun = pastevents(argCount, arguments);
    }
    else if (strcmp(function, "proc") == 0)
    {
        successfulRun = proc(argCount, arguments);
    }
    else
    {
        pid_t childPID = fork();
        if (childPID == 0)
        {
            // child Process
            char *args[argCount + 2];
            args[0] = function;
            for (int i = 0; i < argCount; i++)
            {
                args[i + 1] = arguments[i];
            }
            args[argCount + 1] = NULL;
            execvp(args[0], args);
            printError();
            exit(EXIT_FAILURE);
        }
        else if (childPID > 0)
        {
            // parent Prcess
            if (delimiter == '&')
            {
                // background
                printf("%d\n", childPID);
                saveProcess(childPID, function);
            }
            else
            {
                // foreground
                time_t startTime = time(NULL);
                wait(NULL);
                time_t endTime = time(NULL);
                int executionTime = endTime - startTime;
                char lastCommand[4096];
                if (executionTime > 2)
                {
                    char *timeStr = malloc(sizeof(char) * 10);
                    sprintf(timeStr, "%d", executionTime);
                    strcpy(lastCommand, " ");
                    strcpy(lastCommand + 1, getComamndName(command));
                    strcpy(lastCommand + strlen(lastCommand), " : ");
                    strcpy(lastCommand + strlen(lastCommand), timeStr);
                    strcpy(lastCommand + strlen(lastCommand), "s");
                    free(timeStr);
                }
                else
                {
                    lastCommand[0] = '\0';
                }
                saveLastCommand(lastCommand);
            }
        }
        else
        {
            printf("Failed to execute command fork;\n");
        }
    }
    // for (int i = 0; i < argCount; i++)
    // {
    //     free(arguments[i]);
    // }
    free(function);
}

char *getComamndName(char *command)
{
    char spaceDelimiters[9] = " \n\t\r\b\f\0";
    char *funcName = strtok(command, spaceDelimiters);
    return funcName;
}

void saveLastCommand(char *command)
{
    char *text = malloc(sizeof(char) * 4096);
    memset(text, '\0', 4096);
    char shellprofilepath[4096] = "";
    strcpy(shellprofilepath, home);
    strcpy(shellprofilepath + strlen(home), "/shellprofile");
    int file = open(shellprofilepath, O_RDONLY);
    read(file, text, 4096);
    int pos = 0;
    for (pos = 0; pos < strlen(text); pos++)
    {
        if (strncmp(text + pos, "$LASTCOMMAND=", 13) == 0)
        {
            break;
        }
    }
    char *lastpath = retriveLastPath();
    truncate(shellprofilepath, pos + 13);
    close(file);
    file = open(shellprofilepath, O_WRONLY);
    lseek(file, pos + 13, SEEK_SET);
    write(file, command, strlen(command));
    write(file, "\n$LASTPATH=", 11);
    write(file, lastpath, strlen(lastpath));
    free(lastpath);
    free(text);
    close(file);
}

char *getLastCommand()
{
    // char *text = malloc(sizeof(char) * 4096);
    char text[4096] = "";

    char shellprofilepath[4096] = "";
    strcpy(shellprofilepath, home);
    strcpy(shellprofilepath + strlen(home), "/shellprofile");
    int file = open(shellprofilepath, O_RDONLY);
    read(file, text, 4096);
    int pos = 0, startPos = 0;

    for (pos = 0; pos < strlen(text); pos++)
    {
        if (strncmp(text + pos, "$LASTCOMMAND=", 13) == 0)
        {
            startPos = pos;
        }
        if (strncmp(text + pos, "$LASTPATH=", 10) == 0)
        {
            break;
        }
    }
    char *lastCommand = malloc(sizeof(char) * 4096);
    for (int i = startPos + 13; i < pos - 1; i++)
    {
        lastCommand[i - startPos - 13] = text[i];
    }
    lastCommand[pos - startPos - 14] = '\0';
    close(file);
    saveLastCommand("");
    return lastCommand;
}

char *modifyInput(char *input)
{
    char *newInput = malloc(sizeof(char) * 4096);
    memset(newInput, '\0', sizeof(char) * 4096);
    int startPos = 0;
    int currentPos = 0;

    char spaceDelimiters[9] = " \n\t\r\b\f\0";

    for (currentPos = 0; currentPos < strlen(input) + 1; currentPos++)
    {
        if (input[currentPos] == ';' || input[currentPos] == '&' || input[currentPos] == '\n')
        {
            char command[4096] = "";
            strncpy(command, input + startPos, currentPos - startPos);
            char newCommand[4096] = "";
            startPos = currentPos + 1;

            char *token = strtok(command, spaceDelimiters);
            if (token == NULL)
            {
                continue;
            }
            // printf("token: .%s.\n", token);

            if (token == NULL)
            {
                break;
            }

            strcpy(newCommand, token);
            while (token != NULL)
            {
                token = strtok(NULL, spaceDelimiters);
                if (token != NULL)
                {
                    newCommand[strlen(newCommand)] = ' ';
                    strncpy(newCommand + strlen(newCommand), token, strlen(token));
                }
            }

            if ((strncmp(newCommand, "pastevents execute ", 19) == 0))
            {
                int i = 0;
                for (i = 19; i < strlen(newCommand); i++)
                {
                    if (newCommand[i] == '\n')
                    {
                        break;
                    }
                }
                char countStr[1024] = "";
                strncpy(countStr, newCommand + 19, i - 19);
                strcpy(newCommand, getFromHistory(atoi(countStr), true));
            }

            if (input[currentPos] != '\n')
            {
                newCommand[strlen(newCommand)] = input[currentPos];
            }
            strncpy(newInput + strlen(newInput), newCommand, strlen(newCommand));
            if (input[currentPos] != '\n')
            {
                newInput[strlen(newInput)] = ' ';
            }
        }
    }
    return newInput;
}

void printError()
{
    perror(RED_COLOR "ERROR" LIGHT_RED_COLOR);
    printf(RESET_COLOR);
}

void printWarning(char *message)
{
    printf(MAGENTA_COLOR "WARNING: " LIGHT_MAGENTA_COLOR);
    printf("%s\n", message);
    printf(RESET_COLOR);
}

void printErrorMsg(char *message)
{
    printf(RED_COLOR "ERROR: " LIGHT_RED_COLOR);
    printf("%s\n", message);
    printf(RESET_COLOR);
}
