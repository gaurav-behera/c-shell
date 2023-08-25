#include "headers.h"

int peek(int argc, char *argv[])
{
    int statusCode = 0;

    bool aFlag = false, lFlag = false;
    int argi = 0;
    for (argi = 0; argi < argc; argi++)
    {
        if ((strcmp(argv[argi], "-la") == 0) || (strcmp(argv[argi], "-al") == 0))
        {
            aFlag = true;
            lFlag = true;
        }
        else if (strcmp(argv[argi], "-a") == 0)
        {
            aFlag = true;
        }
        else if (strcmp(argv[argi], "-l") == 0)
        {
            lFlag = true;
        }
        else if (argv[argi][0] == '-')
        {
            if (strlen(argv[argi]) == 1)
            {
                printErrorMsg("No such file or directory");
                return statusCode;
            }
            else
            {
                printWarning("Invalid Flags");
            }
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
        strcpy(pwd, getAbsolutePath(pwd, argv[argi++]));
    }
    if (argi < argc)
    {
        printWarning("Extra Arguments");
    }

    struct dirent **itemList;
    int itemsCount = scandir(pwd, &itemList, NULL, alphasort);
    if (itemsCount == -1)
    {
        printError();
        return statusCode;
    }
    int pwdLength = strlen(pwd);
    char *itemPath = malloc(sizeof(char) * 4096);
    strcpy(itemPath, pwd);
    strcpy(itemPath + pwdLength, "/");

    // struct stat dirDetails;
    // stat(pwd, &dirDetails);
    // printf(WHITE_COLOR "total %ld\n" RESET_COLOR, dirDetails.st_blocks);

    int spacing[6] = {0};
    spacing[0] = 9;
    int totalBlockSize = 0;
    for (int i = 0; i < itemsCount; i++)
    {
        strcpy(itemPath + pwdLength + 1, itemList[i]->d_name);
        struct stat itemDetials;
        lstat(itemPath, &itemDetials);

        totalBlockSize += itemDetials.st_blocks;

        char hardLinksStr[1024] = "";
        sprintf(hardLinksStr, "%d", itemDetials.st_nlink);
        spacing[1] = spacing[1] < strlen(hardLinksStr) ? strlen(hardLinksStr) : spacing[1];

        char *owner = getpwuid(itemDetials.st_uid)->pw_name;
        char *group = getgrgid(itemDetials.st_gid)->gr_name;
        spacing[2] = spacing[2] < strlen(owner) ? strlen(owner) : spacing[2];
        spacing[3] = spacing[3] < strlen(group) ? strlen(group) : spacing[3];

        char sizeStr[1024] = "";
        sprintf(sizeStr, "%ld", itemDetials.st_size);
        spacing[4] = spacing[4] < strlen(sizeStr) ? strlen(sizeStr) : spacing[4];

        char lastModified[1024];
        time_t six_months_ago = time(NULL) - (6 * 30 * 24 * 60 * 60);

        if (itemDetials.st_mtime < six_months_ago)
        {
            strftime(lastModified, sizeof(lastModified), "%H:%M", localtime(&(itemDetials.st_mtime)));
        }
        else
        {
            strftime(lastModified, sizeof(lastModified), "%h %d %H:%M", localtime(&(itemDetials.st_mtime)));
        }
        spacing[5] = spacing[5] < strlen(lastModified) ? strlen(lastModified) : spacing[5];
    }
    if (lFlag == true)
    {
        printf(WHITE_COLOR"Total %d\n", totalBlockSize);
    }
    for (int i = 0; i < itemsCount; i++)
    {
        strcpy(itemPath + pwdLength + 1, itemList[i]->d_name);
        struct stat itemDetials;
        lstat(itemPath, &itemDetials);
        if (aFlag == false && (itemList[i]->d_name)[0] == '.')
        {
            continue;
        }

            printf(WHITE_COLOR);
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

            printf(WHITE_COLOR " %*hu ", spacing[1], itemDetials.st_nlink);

            printf(WHITE_COLOR "%*s %*s ", spacing[2], getpwuid(itemDetials.st_uid)->pw_name, spacing[3], getgrgid(itemDetials.st_gid)->gr_name);

            printf(WHITE_COLOR "%*ld ", spacing[4], itemDetials.st_size);

            char lastModified[1024];
            strftime(lastModified, sizeof(lastModified), "%h %d %H:%M", localtime(&(itemDetials.st_mtime)));
            printf(WHITE_COLOR "%*s ", spacing[5], lastModified);
        }

        if (S_ISDIR(itemDetials.st_mode))
        {
            printf(BLUE_COLOR "%s" RESET_COLOR, itemList[i]->d_name);
        }
        else if (S_ISLNK(itemDetials.st_mode))
        {
            printf(CYAN_COLOR "%s" RESET_COLOR, itemList[i]->d_name);
            if (lFlag == true)
            {
                char readLinkPath[4096] = "";
                readlink(itemPath, readLinkPath, 4096);
                printf(CYAN_COLOR " -> %s" RESET_COLOR, readLinkPath);
            }
        }
        else if (S_ISREG(itemDetials.st_mode))
        {
            if (itemDetials.st_mode & S_IXUSR)
            {
                printf(GREEN_COLOR "%s" RESET_COLOR, itemList[i]->d_name);
            }
            else
            {
                printf(WHITE_COLOR "%s" RESET_COLOR, itemList[i]->d_name);
            }
        }
        else
        {
            printf(WHITE_COLOR "%s" RESET_COLOR, itemList[i]->d_name);
        }
        printf("\n");
        free(itemList[i]);
    }

    free(pwd);
    free(itemPath);
    free(itemList);
    return statusCode;
}
