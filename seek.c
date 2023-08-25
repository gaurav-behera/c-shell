#include "headers.h"

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
                printf(BLUE_COLOR "%s\n", getRelativePath(newPath, originalDir));
            printf("%s", RESET_COLOR);

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
                printf(GREEN_COLOR "%s\n", getRelativePath(newPath, originalDir));
            printf("%s", RESET_COLOR);

                (*count)++;
                strcpy(retValue, newPath);
            }
        }
        free(newPath);
    }
    return retValue;
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
        else if (argv[argi][0] == '-')
        {
            printWarning("Invalid Flags");
        }
        else
        {
            break;
        }
    }

    if (dFlag == true && fFlag == true)
    {
        printErrorMsg("Invalid Flags");
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
        printErrorMsg("Missing Arguments");
        return statusCode;
    }

    char *pwd = malloc(sizeof(char) * 4096);
    getcwd(pwd, 4096);
    if (argi < argc)
    {
        strcpy(pwd, getAbsolutePath(pwd, argv[argi]));
    }

    int *count = malloc(sizeof(int));
    (*count) = 0;
    char *foundItem = searchItem(search, pwd, pwd, count, fFlag, dFlag);

    if (*count == 0)
    {
        printf(MAGENTA_COLOR "No match found!\n");
        printf("%s", RESET_COLOR);
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
            struct stat st;
            if (stat(foundItem, &st) == 0)
            {
                if ((st.st_mode & S_IXUSR) == 0)
                {
                    printWarning("Missing permissions for task!");
                    return statusCode;
                }
                else
                {
                    chdir(foundItem);
                }
            }
            else
            {
                printError();
            }
        }
    }

    return statusCode;
}
