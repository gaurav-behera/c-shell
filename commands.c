#include "headers.h"

void setup()
{
    getcwd(home, 4096);
    home[strlen(home)] = '\0';
    printf("Home: %s\n", home);
    char *text = malloc(sizeof(char) * 4096);

    strncpy(text, "$HOME=", 6);
    strcpy(text + 6, home);
    strcpy(text + strlen(text), "\n$PID=");
    char *pidStr = malloc(sizeof(char) * 10);

    pid_t pid = getpid();
    sprintf(pidStr, "%d", pid);
    strcpy(text + strlen(text), pidStr);

    strcpy(text + strlen(text), "\n$LASTCOMMAND=");
    strcpy(text + strlen(text), "\n$LASTPATH=");

    truncate("shellprofile", 0);
    int file = open("shellprofile", O_CREAT | O_WRONLY, 0777);
    write(file, text, strlen(text));
    close(file);

    file = open("shellhistory", O_CREAT | O_WRONLY, 0777);
    write(file, "$COUNT=", 7);
    close(file);
}

char *getRelativePathHome(char *pwd)
{
    if (strcmp(pwd, home) == 0)
    {
        return "~";
    }
    if (strncmp(pwd, home, strlen(home)) == 0)
    {
        char *relPath = malloc(sizeof(char) * 4096);
        strcpy(relPath, "~");
        strcpy(relPath + 1, pwd + strlen(home));
        return relPath;
    }
    else
    {
        return pwd;
    }
}

char *getRelativePath(char *pwd, char *base)
{
    if (strcmp(pwd, base) == 0)
    {
        return "./";
    }
    if (strncmp(pwd, base, strlen(base)) == 0)
    {
        char *relPath = malloc(sizeof(char) * 4096);
        strcpy(relPath, ".");
        strcpy(relPath + 1, pwd + strlen(base));
        return relPath;
    }
    else
    {
        return getRelativePathHome(pwd);
    }
}

char *getParentPath(char *path)
{
    int endPos = -1;
    for (int i = 0; i < strlen(path); i++)
    {
        if (path[i] == '/')
        {
            endPos = i;
        }
    }
    if (endPos == -1)
    {
        return "/";
    }
    char *parentPath = malloc(sizeof(char) * 4096);
    strncpy(parentPath, path, endPos);
    return parentPath;
}

void savePath(char *path)
{
    char *text = malloc(sizeof(char) * 4096);
    char shellprofilepath[4096];
    strcpy(shellprofilepath, home);
    strcpy(shellprofilepath + strlen(home), "/shellprofile");
    int file = open(shellprofilepath, O_RDONLY);
    read(file, text, 4096);
    int pos = 0;
    for (pos = 0; pos < strlen(text); pos++)
    {
        if (strncmp(text + pos, "$LASTPATH=", 10) == 0)
        {
            break;
        }
    }
    file = open(shellprofilepath, O_WRONLY);
    lseek(file, pos + 10, SEEK_SET);
    write(file, path, strlen(path));
    write(file, "\0", 1);
    close(file);
}

pid_t getShellPID()
{
    char *text = malloc(sizeof(char) * 4096);
    char shellprofilepath[4096];
    strcpy(shellprofilepath, home);
    strcpy(shellprofilepath + strlen(home), "/shellprofile");
    int file = open(shellprofilepath, O_RDONLY);
    read(file, text, 4096);
    int pos = 0, startPos = 0;
    for (pos = 0; pos < strlen(text); pos++)
    {
        if (strncmp(text + pos, "$PID=", 5) == 0)
        {
            startPos = pos;
        }
        if (text[pos] == '\n')
        {
            break;
        }
    }
    char *pidStr = malloc(sizeof(char) * 10);
    for (int i = startPos + 5; i < pos - 1; i++)
    {
        pidStr[i - startPos - 5] = text[i];
    }
    close(file);
    pid_t pid = atoi(pidStr);
    return pid;
}

char *retriveLastPath()
{
    char *text = malloc(sizeof(char) * 4096);
    char shellprofilepath[4096];
    strcpy(shellprofilepath, home);
    strcpy(shellprofilepath + strlen(home), "/shellprofile");
    int file = open(shellprofilepath, O_RDONLY);
    read(file, text, 4096);
    int pos = 0;
    for (pos = 0; pos < strlen(text); pos++)
    {
        if (strncmp(text + pos, "$LASTPATH=", 10) == 0)
        {
            break;
        }
    }
    char *lastPath = malloc(sizeof(char) * 4096);
    close(file);
    strcpy(lastPath, text + pos + 10);
    return lastPath;
}

void saveLastCommand(char * command){
    char *text = malloc(sizeof(char) * 4096);
    char shellprofilepath[4096];
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
    truncate(shellprofilepath, pos+13);
    close(file);
    file = open(shellprofilepath, O_WRONLY);
    lseek(file, pos + 13, SEEK_SET);
    write(file, command, strlen(command));
    write(file, "\n$LASTPATH=", 11);
    write(file, lastpath, strlen(lastpath));
    close(file);
}

char *getLastCommand()
{
    char *text = malloc(sizeof(char) * 4096);
    char shellprofilepath[4096];
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
    for (int i = startPos + 13; i < pos-1; i++)
    {
        lastCommand[i - startPos - 13] = text[i];
    }
    lastCommand[pos-startPos-14] = '\0';
    close(file);
    saveLastCommand("");
    return lastCommand;
}

char *modifyInput(char *input)
{
    char *newInput = malloc(sizeof(char) * 4096);
    int startPos = 0;
    int currentPos = 0;

    char spaceDelimiters[9] = " \n\t\r\b\f\0";

    for (currentPos = 0; currentPos < strlen(input) + 1; currentPos++)
    {
        if (input[currentPos] == ';' || input[currentPos] == '&' || input[currentPos] == '\n')
        {
            char *command = malloc(sizeof(char) * 4096);
            strncpy(command, input + startPos, currentPos - startPos);
            char *newCommand = malloc(sizeof(char) * 4096);
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
                newCommand = getFromHistory(atoi(newCommand + 19));
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

    if (strcmp(input, getFromHistory(1)) == 0)
    {
        return;
    }

    char *text = malloc(sizeof(char) * 4096);
    char shellhistorypath[4096];
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
    char *countStr = malloc(sizeof(char) * 3);
    strncpy(countStr, text + 7, pos - 7);
    int commandCount = atoi(countStr);
    // printf("Command Count: %d\n", commandCount);

    close(file);

    if (commandCount < 15)
    {
        commandCount++;
        sprintf(countStr, "%d", commandCount);
    }
    file = open(shellhistorypath, O_WRONLY);

    lseek(file, 7, SEEK_SET);
    write(file, countStr, strlen(countStr));
    write(file, "\n", 1);
    write(file, input, strlen(input));
    write(file, "\n", 1);
    int startPos = pos + 1;
    int currCount = 1;
    for (int i = pos + 1; i < strlen(text) && currCount < commandCount; i++)
    {
        if (text[i] == '\n')
        {
            write(file, text + startPos, i - startPos);
            write(file, "\n", 1);
            startPos = i + 1;
            currCount++;
        }
    }
    close(file);
}

char *getFromHistory(int position)
{
    char *text = malloc(sizeof(char) * 4096);
    char shellhistorypath[4096];
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
    char *countStr = malloc(sizeof(char) * 3);
    strncpy(countStr, text + 7, pos - 7);
    int commandCount = atoi(countStr);

    char *newCommand = malloc(sizeof(char) * 4096);
    if (position < 1 || position > commandCount)
    {
        printf("Invlid argument pos = %d\n", position);
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
    // printf("Comamd: %s\n", newCommand);
    return newCommand;
}

char *getAbsolutePath(char *pwd, char *path)
{
    char *newpath = malloc(sizeof(char) * 4096);
    strcpy(newpath, pwd);
    char *dir = strtok(path, "/");
    bool flag = 1;
    if (strcmp(dir, "-") == 0)
    {
        return retriveLastPath();
    }
    if (strcmp(dir, "~") == 0)
    {
        strcpy(newpath, home);
    }
    if (path[0] == '/')
    {
        strcpy(newpath, "");
    }

    while (dir != NULL && strlen(dir) > 0)
    {
        if (strcmp(dir, ".") == 0)
        {
        }
        else if (strcmp(dir, "..") == 0)
        {
            newpath = getParentPath(newpath);
        }
        else
        {
            if (!flag || (strcmp(dir, "~") != 0))
            {
                strcat(newpath, "/");
                strcat(newpath, dir);
            }
        }
        flag = 0;
        dir = strtok(NULL, "/");
    }
    // printf("New path: %s\n", newpath);
    return newpath;
}

char *getRealPath(char *path, char *newpath)
{
    newpath = realpath(path, newpath);
    return newpath;
}

char *getComamndName(char *command)
{
    char spaceDelimiters[9] = " \n\t\r\b\f\0";
    char *funcName = strtok(command, spaceDelimiters);
    return funcName;
}

void executeInForeground(char *command)
{
    pid_t childPID = fork();
    if (childPID == 0)
    {
        // child Process
        executeCommand(command);
    }
    else if (childPID > 0)
    {
        // parent Prcess
        time_t startTime = time(NULL);
        wait(NULL);
        time_t endTime = time(NULL);
        int executionTime = endTime - startTime;
        char *lastCommand = malloc(sizeof(char) * 4096);
        if (executionTime > 2)
        {
            char * timeStr = malloc(sizeof(char)*10);
            sprintf(timeStr, "%d", executionTime);
            strcpy(lastCommand, " ");
            strcpy(lastCommand+1, getComamndName(command));
            strcpy(lastCommand+strlen(lastCommand), " : ");
            strcpy(lastCommand+strlen(lastCommand), timeStr);
            strcpy(lastCommand+strlen(lastCommand), "s");
        }
        else
        {
            lastCommand = "";
        }
        saveLastCommand(lastCommand);
    }
    else
    {
        printf("Failed to execute command;");
    }
}

void executeInBackground(char *command)
{
    pid_t childPID = fork();
    if (childPID == 0)
    {
        // child Process
        executeCommand(command);
    }
    else if (childPID > 0)
    {
        // parent Prcess
        // wait(NULL);
        printf("%d started\n", childPID);
    }
    else
    {
        printf("Failed to execute command;");
    }
}

void checkBackgroundCompletion()
{
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        printf("%d completed\n", pid);
    }
}

int executeCommand(char *command)
{
    char spaceDelimiters[9] = " \n\t\r\b\f\0";
    char *token = strtok(command, spaceDelimiters);

    char function[strlen(token) + 1];
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
        char *args[argCount + 2];
        args[0] = function;
        for (int i = 0; i < argCount; i++)
        {
            args[i + 1] = arguments[i];
        }
        args[argCount + 1] = NULL;
        execvp(args[0], args);
        perror("Failed to execute command");
        return 0;
    }

    return successfulRun;
}

int executeCommand_working(char *command)
{
    printf("Command: %s\n", command);

    char spaceDelimiters[9] = " \n\t\r\b\f\0";
    char *token = strtok(command, spaceDelimiters);

    char function[strlen(token) + 1];
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
        }
    }

    printf("Function: %s\n", function);
    printf("Argument Count: %d\n", argCount);
    printf("Arguments: ");
    for (int i = 0; i < argCount; i++)
    {
        printf("%s, ", arguments[i]);
    }
    printf("\n");

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
    else
    {
        return 0;
    }
    return successfulRun;
}

char *searchItem(char *search, char *dir, char *originalDir, int *count, bool fFlag, bool dFlag)
{
    char *retValue = malloc(sizeof(char) * 4096);
    struct dirent **itemList;
    int itemsCount = scandir(dir, &itemList, NULL, alphasort);

    int dirLength = strlen(dir);
    for (int i = 0; i < itemsCount; i++)
    {
        char *newPath = malloc(sizeof(char) * 4096);
        strcpy(newPath, dir);
        strcpy(newPath + dirLength, "/");
        strcpy(newPath + dirLength + 1, itemList[i]->d_name);
        if ((strcmp(itemList[i]->d_name, ".") == 0) || (strcmp(itemList[i]->d_name, "..") == 0))
        {
            continue;
        }
        if (itemList[i]->d_type == DT_DIR)
        {
            if ((dFlag == true) && (strncmp(itemList[i]->d_name, search, strlen(search)) == 0))
            {
                printf(BLUE_COLOR "%s\n" RESET_COLOR, getRelativePath(newPath, originalDir));
                (*count)++;
                strcpy(retValue, newPath);
            }
            char *newRet = searchItem(search, newPath, originalDir, count, fFlag, dFlag);
            if (strlen(newRet) != 0)
            {
                strcpy(retValue, newRet);
            }
        }
        else
        {
            if ((fFlag == true) && (strncmp(itemList[i]->d_name, search, strlen(search)) == 0))
            {
                printf(GREEN_COLOR "%s\n" RESET_COLOR, getRelativePath(newPath, originalDir));
                (*count)++;
                strcpy(retValue, newPath);
            }
        }
        free(newPath);
    }
    return retValue;
}

int warp(int argc, char *argv[])
{
    int statusCode = 0;
    if (argc == 0)
    {
        chdir(home);
    }
    for (int i = 0; i < argc; i++)
    {
        char pwd[4096];
        getcwd(pwd, 4096);
        printf("Current Directory: %s\n", getRelativePathHome(pwd));

        char *newpath = getAbsolutePath(pwd, argv[i]);
        // char * newpath =malloc(sizeof(char)*4096);
        // newpath = getRealPath(argv[i], newpath);

        printf("New Path = %s\n", newpath);
        if (chdir(newpath) == 0 && newpath != NULL)
        {
            savePath(pwd);
            printf("changed\n");
            statusCode = 1;
        }
        else
        {
            printf("Failed to change directory\n");
            statusCode = 0;
            break;
            // chdir(pwd);
        }
    }
    return statusCode;
}

int peek(int argc, char *argv[])
{
    int statusCode = 0;

    bool aFlag = false, lFlag = false;
    int argi = 0;
    for (argi = 0; argi < argc; argi++)
    {
        if ((strncmp(argv[argi], "-la", 3) == 0) || (strncmp(argv[argi], "-al", 3) == 0))
        {
            aFlag = true;
            lFlag = true;
        }
        else if (strncmp(argv[argi], "-a", 2) == 0)
        {
            aFlag = true;
        }
        else if (strncmp(argv[argi], "-l", 2) == 0)
        {
            lFlag = true;
        }
        else
        {
            break;
        }
    }

    char *pwd = malloc(sizeof(char) * 4096);
    getcwd(pwd, 4096);
    if (argi < argc)
    {
        strcpy(pwd, getAbsolutePath(pwd, argv[argi]));
    }

    struct dirent **itemList;
    int itemsCount = scandir(pwd, &itemList, NULL, alphasort);
    if (itemsCount == -1)
    {
        perror("Failed to scan directory");
        return statusCode;
    }
    int pwdLength = strlen(pwd);
    char *itemPath = malloc(sizeof(char) * 4096);
    strcpy(itemPath, pwd);
    strcpy(itemPath + pwdLength, "/");
    // printf("l: %d, a: %d\n", lFlag, aFlag);
    struct stat dirDetails;
    stat(pwd, &dirDetails);
    printf("total %d\n", dirDetails.st_blocks);

    for (int i = 0; i < itemsCount; i++)
    {
        strcpy(itemPath + pwdLength + 1, itemList[i]->d_name);
        struct stat itemDetials;
        lstat(itemPath, &itemDetials);
        if (aFlag == false && (itemList[i]->d_name)[0] == '.')
        {
            continue;
        }

        if (lFlag == true)
        {
            printf((S_ISDIR(itemDetials.st_mode)) ? "d" : "-");
            printf((itemDetials.st_mode & S_IRUSR) ? "r" : "-");
            printf((itemDetials.st_mode & S_IWUSR) ? "w" : "-");
            printf((itemDetials.st_mode & S_IXUSR) ? "x" : "-");
            printf((itemDetials.st_mode & S_IRGRP) ? "r" : "-");
            printf((itemDetials.st_mode & S_IWGRP) ? "w" : "-");
            printf((itemDetials.st_mode & S_IXGRP) ? "x" : "-");
            printf((itemDetials.st_mode & S_IROTH) ? "r" : "-");
            printf((itemDetials.st_mode & S_IWOTH) ? "w" : "-");
            printf((itemDetials.st_mode & S_IXOTH) ? "x" : "-");

            printf(" %3hu", itemDetials.st_nlink);

            printf(" %15s %7s", getpwuid(itemDetials.st_uid)->pw_name, getgrgid(itemDetials.st_gid)->gr_name);

            printf(" %6ld", itemDetials.st_size);

            char time_buffer[1024];
            time_t six_months_ago = time(NULL) - (6 * 30 * 24 * 60 * 60);

            if (itemDetials.st_mtime < six_months_ago)
            {
                strftime(time_buffer, sizeof(time_buffer), "%H:%M", localtime(&(itemDetials.st_mtime)));
            }
            else
            {
                strftime(time_buffer, sizeof(time_buffer), "%h %d %H:%M", localtime(&(itemDetials.st_mtime)));
            }
            printf(" %15s  ", time_buffer);
        }

        if (S_ISDIR(itemDetials.st_mode))
        {
            printf(DARK_BLUE "%s" RESET_COLOR, itemList[i]->d_name);
        }
        else if (S_ISLNK(itemDetials.st_mode))
        {
            printf(DARK_CYAN "%s" RESET_COLOR, itemList[i]->d_name);
        }
        else if (S_ISREG(itemDetials.st_mode))
        {
            if (itemDetials.st_mode & S_IXUSR)
            {
                printf(DARK_GREEN "%s" RESET_COLOR, itemList[i]->d_name);
            }
            else
            {
                printf(WHITE_COLOR "%s" RESET_COLOR, itemList[i]->d_name);
            }
        }
        else
        {
            printf(GRAY_COLOR "%s" RESET_COLOR, itemList[i]->d_name);
        }
        printf("\n");

        // free(itemList[i]);
    }

    return statusCode;
}

int seek(int argc, char *argv[])
{
    int statusCode = 0;

    bool dFlag = false, eFlag = false, fFlag = false;
    bool noFlagsGiven = true;
    int argi = 0;
    for (argi = 0; argi < argc; argi++)
    {
        if ((strlen(argv[argi]) == 4) && ((strncmp(argv[argi], "-def", 4) == 0) || (strncmp(argv[argi], "-dfe", 4) == 0) || (strncmp(argv[argi], "-fde", 4) == 0) || (strncmp(argv[argi], "-fed", 4) == 0) || (strncmp(argv[argi], "-efd", 4) == 0) || (strncmp(argv[argi], "-edf", 4) == 0)))
        {
            noFlagsGiven = false;
            dFlag = true;
            eFlag = true;
            fFlag = true;
        }
        else if ((strlen(argv[argi]) == 3) && ((strncmp(argv[argi], "-de", 3) == 0) || (strncmp(argv[argi], "-ed", 3) == 0)))
        {
            noFlagsGiven = false;
            dFlag = true;
            eFlag = true;
        }
        else if ((strlen(argv[argi]) == 3) && ((strncmp(argv[argi], "-df", 3) == 0) || (strncmp(argv[argi], "-fd", 3) == 0)))
        {
            noFlagsGiven = false;
            dFlag = true;
            fFlag = true;
        }
        else if ((strlen(argv[argi]) == 3) && ((strncmp(argv[argi], "-fe", 3) == 0) || (strncmp(argv[argi], "-ef", 3) == 0)))
        {
            noFlagsGiven = false;
            fFlag = true;
            eFlag = true;
        }
        else if (strncmp(argv[argi], "-d", 2) == 0)
        {
            noFlagsGiven = false;
            dFlag = true;
        }
        else if (strncmp(argv[argi], "-e", 2) == 0)
        {
            // noFlagsGiven = false;
            eFlag = true;
        }
        else if (strncmp(argv[argi], "-f", 2) == 0)
        {
            noFlagsGiven = false;
            fFlag = true;
        }
        else
        {
            break;
        }
    }

    if (dFlag == true && fFlag == true)
    {
        printf("Invalid Flags");
        return statusCode;
    }
    if (noFlagsGiven == true)
    {
        dFlag = true;
        fFlag = true;
    }

    char *search = malloc(sizeof(char) * 4096);
    if (argi < argc)
    {
        strcpy(search, argv[argi++]);
    }
    else
    {
        printf("Missing Arguments");
    }

    char *pwd = malloc(sizeof(char) * 4096);
    getcwd(pwd, 4096);
    if (argi < argc)
    {
        strcpy(pwd, getAbsolutePath(pwd, argv[argi]));
    }

    // printf("Flags <d,e,f>: %d %d %d\n", dFlag, eFlag, fFlag);
    // printf("Search Str: %s \n", search);
    // printf("Directory: %s \n", pwd);

    int *count = malloc(sizeof(int));
    (*count) = 0;
    char *foundItem = searchItem(search, pwd, pwd, count, fFlag, dFlag);
    // printf("count: %d\n", *count);
    // printf("ret: %s\n", foundItem);

    if (*count == 0)
    {
        printf("No match found!\n");
    }

    if ((eFlag == true) && (*count == 1))
    {
        if (fFlag == true)
        {
            char *text = malloc(sizeof(char) * 4096);
            int file = open(foundItem, O_RDONLY);
            int bytesRead = 0;
            while ((bytesRead = read(file, text, 4095)) > 0)
            {
                text[bytesRead] = '\0';
                printf("%s\n", text);
            }
        }
        if (dFlag == true)
        {
            chdir(foundItem);
        }
    }

    return statusCode;
}

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

int pastevents(int argc, char *argv[])
{
    int statusCode = 0;
    char *text = malloc(sizeof(char) * 4096);
    char shellhistorypath[4096];
    strcpy(shellhistorypath, home);
    strcpy(shellhistorypath + strlen(home), "/shellhistory");

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
        // printf("%s", text + pos + 1);
        printHistory(text + pos + 1);
        close(file);
    }
    else if ((argc == 1) && (strcmp(argv[0], "purge") == 0))
    {
        int file = open(shellhistorypath, O_WRONLY | O_TRUNC);
        if (truncate(shellhistorypath, 0) == -1)
        {
            printf("Error, failed to truncate\n");
            return statusCode;
        }
        close(file);
        file = open("shellhistory", O_CREAT | O_WRONLY, 0777);
        write(file, "$COUNT=0", 8);
        close(file);
    }
    else
    {
        printf("Invalid Arguments\n");
        return statusCode;
    }

    return statusCode;
}

int proc(int argc, char *argv[])
{
    int statusCode = 0;
    pid_t processPID;
    if (argc == 0)
    {
        pid_t shellPID = getShellPID();
        processPID = shellPID;
    }
    else if (argc > 1)
    {
        printf("Invalid Arguments!\n");
        return statusCode;
    }
    else
    {
        processPID = atoi(argv[0]);
    }

    printf("pid : %d\n", processPID);
    char *processPIDStr = malloc(sizeof(char) * 10);
    sprintf(processPIDStr, "%d", processPID);

    char *procStatusFilePath = malloc(sizeof(char) * 4096);
    strcpy(procStatusFilePath, "/proc/");
    strcpy(procStatusFilePath + strlen(procStatusFilePath), processPIDStr);
    strcpy(procStatusFilePath + strlen(procStatusFilePath), "/status");

    char *text = malloc(sizeof(char) * 4096);
    // printf("path: %s\n", procStatusFilePath);
    int procStatusFile = open(procStatusFilePath, O_RDONLY);
    if (procStatusFile < 0)
    {
        printf("Error retriving details\n");
    }
    read(procStatusFile, text, 4096);

    char *statePtr = strstr(text, "State:");
    char processState[3];
    for (int i = 6; i < strlen(statePtr); i++)
    {
        if ((statePtr[i] == 'R') || (statePtr[i] == 'S') || (statePtr[i] == 'Z'))
        {
            processState[0] = statePtr[i];
            if (statePtr[i + 1] == '+')
            {
                processState[1] = '+';
                processState[2] = '\0';
            }
            else
            {
                processState[1] = '\0';
            }
            break;
        }
        if (statePtr[i] == '\n')
        {
            break;
        }
    }
    printf("Process State : %s\n", processState);

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
    printf("Process Group : %s\n", processGroup);

    char *vmSizePtr = strstr(text, "VmSize:");
    char vmSize[20];
    j = 0;
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
    printf("Virtual memory : %s\n", vmSize);

    char *exePath = malloc(sizeof(char) * 4096);
    strcpy(exePath, "/proc/");
    strcpy(exePath + strlen(exePath), processPIDStr);
    strcpy(exePath + strlen(exePath), "/exe");
    char *readLinkPath = malloc(sizeof(char) * 4096);
    if (readlink(exePath, readLinkPath, 4096) == -1)
    {
        printf("Error retriving executable path!\n");
    }
    else
    {
        printf("Executable path : %s\n", getRelativePathHome(readLinkPath));
    }

    return statusCode;
}
