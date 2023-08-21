#include "headers.h"

void setup()
{
    getcwd(home, 4096);
    home[strlen(home)] = '\0';
    printf("Home: %s\n", home);
    char *path = malloc(sizeof(char) * (5 + strlen(home) + 1 + 9));
    strncpy(path, "$HOME=", 6);
    strcpy(path + 6, home);
    strncpy(path + strlen(home) + 6, "\n$LASTPATH=", 11);
    int file = open("shellprofile", O_CREAT | O_WRONLY, 0777);
    write(file, path, 6 + strlen(home) + 11);
    close(file);
    file = open("shellhistory", O_CREAT | O_WRONLY, 0777);
    write(file, "$COUNT=", 7);
    close(file);
}

char *getRelativePath(char *pwd)
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

char *modifyInput(char *input)
{
    char *newInput = malloc(sizeof(char) * 4096);

    return newInput;
}

void saveInput(char *input)
{
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

    char *text = malloc(sizeof(char) * 4096);
    char shellprofilepath[4096];
    strcpy(shellprofilepath, home);
    strcpy(shellprofilepath + strlen(home), "/shellhistory");
    int file = open(shellprofilepath, O_RDONLY);
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
    file = open(shellprofilepath, O_WRONLY);

    lseek(file, 7, SEEK_SET);
    write(file, countStr, strlen(countStr));
    write(file, "\n", 1);
    write(file, input, strlen(input));
    int startPos = pos + 1;
    int currCount = 1;
    for (int i = pos + 1; i < strlen(text) && currCount < commandCount; i++)
    {
        if (text[i] == '\n')
        {
            write(file, text + startPos, i - startPos);
            write(file, "\n", 1);
            startPos = i + 1;
            ;
            currCount++;
        }
    }
    close(file);
}

char *getFromHistory(int position)
{
    char *text = malloc(sizeof(char) * 4096);
    char shellprofilepath[4096];
    strcpy(shellprofilepath, home);
    strcpy(shellprofilepath + strlen(home), "/shellhistory");
    int file = open(shellprofilepath, O_RDONLY);
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

    if (position < 1 || position > commandCount)
    {
        printf("Invlid argument pos = %d\n", position);
        return NULL;
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
    char *newCommand = malloc(sizeof(char) * 4096);
    strncpy(newCommand, text + startPos, endPos - startPos);
    printf("Comamd: %s\n", newCommand);
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
        wait(NULL);
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
    // printf("Command: %s\n", command);

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

    // printf("Function: %s\n", function);
    // printf("Argument Count: %d\n", argCount);
    // printf("Arguments: ");
    // for (int i = 0; i < argCount; i++)
    // {
    //     printf("%s, ", arguments[i]);
    // }
    // printf("\n");

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

char * searchItem(char *search, char *dir, int *count, bool fFlag, bool dFlag)
{
    char * retValue = malloc(sizeof(char)*4096);
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
                printf(BLUE_COLOR "%s\n" RESET_COLOR, (newPath));
                (*count)++;
                strcpy(retValue, newPath);
            }
            char * newRet = searchItem(search, newPath, count, fFlag, dFlag);
            if (strlen(newRet) != 0)
            {
                strcpy(retValue, newRet);
            }
        }
        else
        {
            if ((fFlag == true) && (strncmp(itemList[i]->d_name, search, strlen(search)) == 0))
            {
                printf(GREEN_COLOR "%s\n" RESET_COLOR, (newPath));
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
        printf("Current Directory: %s\n", getRelativePath(pwd));

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

    printf("Flags <d,e,f>: %d %d %d\n", dFlag, eFlag, fFlag);
    printf("Search Str: %s \n", search);
    printf("Directory: %s \n", pwd);

    int *count = malloc(sizeof(int));
    (*count) = 0;
    char * foundItem = searchItem(search, pwd, count, fFlag, dFlag);
    printf("count: %d\n", *count);
    printf("ret: %s\n", foundItem);
    return statusCode;
}

int pastevents(int argc, char *argv[])
{
    int statusCode = 0;

    char *text = malloc(sizeof(char) * 4096);
    char shellprofilepath[4096];
    strcpy(shellprofilepath, home);
    strcpy(shellprofilepath + strlen(home), "/shellhistory");
    int file = open(shellprofilepath, O_RDONLY);
    read(file, text, 4096);
    int pos = 0;
    for (pos = 7; pos < strlen(text); pos++)
    {
        if (text[pos] == '\n')
        {
            break;
        }
    }
    printf("%s", text + pos + 1);
    close(file);

    return statusCode;
}