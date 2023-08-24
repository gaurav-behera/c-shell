#include "headers.h"

int proc(int argc, char *argv[])
{
    int statusCode = 0;
    pid_t processPID, processGroupID, tc_pgid;

    if (argc == 0)
    {
        pid_t shellPID = getShellPID();
        processPID = shellPID;
    }
    else if (argc > 1)
    {
        printErrorMsg("Invalid Arguments");
        return statusCode;
    }
    else
    {
        processPID = atoi(argv[0]);
    }

    if ((processGroupID = getpgid(processPID)) == -1)
    {
        printError();
        return statusCode;
    }
    if ((tc_pgid = tcgetpgrp(STDIN_FILENO)) == -1)
    {
        printError();
        return statusCode;
    }

    char *processPIDStr = malloc(sizeof(char) * 10);
    sprintf(processPIDStr, "%d", processPID);

    char *procStatusFilePath = malloc(sizeof(char) * 4096);
    strcpy(procStatusFilePath, "/proc/");
    strcpy(procStatusFilePath + strlen(procStatusFilePath), processPIDStr);
    strcpy(procStatusFilePath + strlen(procStatusFilePath), "/status");

    char *text = malloc(sizeof(char) * 4096);
    int procStatusFile = open(procStatusFilePath, O_RDONLY);
    if (procStatusFile < 0)
    {
        // printError();
        printErrorMsg("No process with given pid");
        return statusCode;
    }
    if (read(procStatusFile, text, 4096) == -1)
    {
        printError();
        return statusCode;
    }
    printf(WHITE_COLOR"pid : %d\n" RESET_COLOR, processPID);

    char *statePtr = strstr(text, "State:");
    char processState[1];
    for (int i = 6; i < strlen(statePtr); i++)
    {
        if ((statePtr[i] == 'R') || (statePtr[i] == 'S') || (statePtr[i] == 'Z') || (statePtr[i] == 'I'))
        {
            processState[0] = statePtr[i];
            break;
        }
        if (statePtr[i] == '\n')
        {
            break;
        }
    }
    printf(WHITE_COLOR"Process State : %s", processState);
    if (tc_pgid == processGroupID)
    {
        printf("+\n" RESET_COLOR);
    }
    else
    {
        printf("\n" RESET_COLOR);
    }

    char *groupPtr = strstr(text, "Tgid:");
    char processGroup[20];
    int j = 0;
    for (int i = 5; i < strlen(groupPtr); i++)
    {
        if (groupPtr[i] >= '0' && groupPtr[i] <= '9')
        {
            processGroup[j++] = groupPtr[i];
        }
        if (groupPtr[i] == '\n')
        {
            processGroup[j++] = '\0';
            break;
        }
    }
    if (strlen(processGroup) == 0)
    {
        printErrorMsg("Unable to extract Pro");
    }
    printf(WHITE_COLOR"Process Group : %d\n"RESET_COLOR, processGroupID);

    char *vmSizePtr = strstr(text, "VmSize:");
    char vmSize[20] = "";
    j = 0;
    if (vmSizePtr == NULL)
    {
        printErrorMsg("Unable to extract VmSize");
        return statusCode;
    }
    
    for (int i = 7; i < strlen(vmSizePtr); i++)
    {
        if (vmSizePtr[i] >= '0' && vmSizePtr[i] <= '9')
        {
            vmSize[j++] = vmSizePtr[i];
        }
        if (vmSizePtr[i] == '\n')
        {
            vmSize[j++] = '\0';
            break;
        }
    }
    if (strlen(vmSize) == 0)
    {
        printErrorMsg("Unable to extract VM Size");
        return statusCode;
    }

    printf(WHITE_COLOR"Virtual memory : %s\n"RESET_COLOR, vmSize);

    char *exePath = malloc(sizeof(char) * 4096);
    strcpy(exePath, "/proc/");
    strcpy(exePath + strlen(exePath), processPIDStr);
    strcpy(exePath + strlen(exePath), "/exe");
    char *readLinkPath = malloc(sizeof(char) * 4096);
    if (readlink(exePath, readLinkPath, 4096) == -1)
    {
        // printf("Error retriving executable path!\n");
        printError();
        return statusCode;
    }
    printf(WHITE_COLOR"Executable path : %s\n" RESET_COLOR, getRelativePathHome(readLinkPath));

    return statusCode;
}

pid_t getShellPID()
{
    char text[4096];
    char shellprofilepath[4096] = "";
    strcpy(shellprofilepath, home);
    strcpy(shellprofilepath + strlen(home), "/shellprofile");
    int file = open(shellprofilepath, O_RDONLY);
    read(file, text, 4096);
    int pos = 0, startPos = 0;
    bool found = false;
    for (pos = 0; pos < strlen(text); pos++)
    {
        if (strncmp(text + pos, "$PID=", 5) == 0)
        {
            startPos = pos;
            found = true;
        }
        if (found == true && text[pos] == '\n')
        {
            break;
        }
    }
    char *pidStr = malloc(sizeof(char) * 10);
    memset(pidStr, '\0', 10);
    strncpy(pidStr, text + startPos + 5, pos - startPos - 5);
    close(file);
    pid_t pid = atoi(pidStr);

    return pid;
}
