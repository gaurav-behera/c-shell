#include "headers.h"

void printHistory(char *text)
{
    for (int i = 0; i < strlen(text); i++)
    {
        if (text[i] == '\n')
        {
            text[i] = '\0';
            printHistory(text + i + 1);
            break;
        }
    }
    if (strlen(text) > 0)
    {
        printf("%s\n", text);
    }
}

void saveInput(char *input)
{
    if (input[strlen(input) - 1] == '\n')
    {
        input[strlen(input) - 1] = '\0';
    }

    if (strlen(input) == 0)
    {
        return;
    }

    for (int i = 0; i < strlen(input); i++)
    {
        if (strncmp(input + i, "pastevents", 10) == 0)
        {
            return;
        }
    }

    char *recentInput = getFromHistory(1, false);

    if (strcmp(input, recentInput) == 0)
    {
        return;
    }

    char *text = malloc(sizeof(char) * 4096);
    // memset(text, '\0', 4096);
    char shellhistorypath[4096] = "";
    strcpy(shellhistorypath, home);
    strcpy(shellhistorypath + strlen(home), "/shellhistory");
    int file = open(shellhistorypath, O_RDONLY);
    read(file, text, 4096);
    int pos = 0;
    for (pos = 7; pos < strlen(text); pos++)
    {
        if (text[pos] == '\n')
        {
            break;
        }
    }
    char countStr[3] = "";
    strncpy(countStr, text + 7, pos - 7);
    int commandCount = atoi(countStr);
    // printf("Command Count: %d\n", commandCount);

    close(file);

    if (commandCount < 15)
    {
        commandCount++;
        sprintf(countStr, "%d", commandCount);
    }
    file = open(shellhistorypath, O_WRONLY | O_TRUNC);
    truncate(shellhistorypath, 7);
    lseek(file, 0, SEEK_SET);
    write(file, "$COUNT=", 7);
    write(file, countStr, strlen(countStr));
    write(file, "\n", 1);
    write(file, input, strlen(input));
    write(file, "\n", 1);
    int startPos = pos + 1;
    int currCount = 1;
    for (int i = pos + 1; (i < strlen(text)) && (currCount < commandCount); i++)
    {
        if (text[i] == '\n')
        {
            write(file, text + startPos, i - startPos);
            write(file, "\n", 1);
            startPos = i + 1;
            currCount++;
        }
    }
    free(text);
    close(file);
}

char *getFromHistory(int position, bool showError)
{
    char text[4096] = "";
    char shellhistorypath[4096] = "";
    strcpy(shellhistorypath, home);
    strcpy(shellhistorypath + strlen(home), "/shellhistory");
    int file = open(shellhistorypath, O_RDONLY);
    read(file, text, 4096);
    int pos = 0;
    for (pos = 7; pos < strlen(text); pos++)
    {
        if (text[pos] == '\n')
        {
            break;
        }
    }
    char countStr[3] = "";
    strncpy(countStr, text + 7, pos - 7);
    int commandCount = atoi(countStr);

    char *newCommand = malloc(sizeof(char) * 4096);
    memset(newCommand, '\0', 4096);
    if ((showError == true) && (position < 1 || position > commandCount))
    {
        // printf("Invlid argument pos = %d\n", position);
        printErrorMsg("Invalid index for pastevents");
        return newCommand;
    }

    int currCount = 0;
    int startPos = pos + 1;
    int endPos = pos + 1;
    for (endPos = pos + 1; endPos < strlen(text); endPos++)
    {
        if (text[endPos] == '\n')
        {
            currCount++;
            if (currCount == position)
            {
                break;
            }
            startPos = endPos + 1;
        }
    }
    strncpy(newCommand, text + startPos, endPos - startPos);
    strncpy(newCommand + strlen(newCommand), "\0", 1);
    return newCommand;
}

int pastevents(int argc, char *argv[])
{
    int statusCode = 0;
    char text[4096] = "";
    char shellhistorypath[4096] = "";
    strcpy(shellhistorypath, home);
    strcpy(shellhistorypath + strlen(home), "/shellhistory");

    if (argc > 1)
    {
        printWarning("Extra Arguments");
    }
    
    if (argc == 0)
    {
        // print 15 commands
        int file = open(shellhistorypath, O_RDONLY);
        read(file, text, 4096);
        int pos = 0;
        for (pos = 7; pos < strlen(text); pos++)
        {
            if (text[pos] == '\n')
            {
                break;
            }
        }

        printHistory(text + pos + 1);
        close(file);
    }
    else if ((argc == 1) && (strcmp(argv[0], "purge") == 0))
    {
        int file = open(shellhistorypath, O_WRONLY | O_TRUNC);
        if (truncate(shellhistorypath, 0) == -1)
        {
            printError();
            return statusCode;
        }
        close(file);
        file = open(shellhistorypath, O_WRONLY, 0666);
        write(file, "$COUNT=0\n", 9);
        close(file);
    }
    else
    {
        printErrorMsg("Invalid arguments");
    }

    return statusCode;
}
