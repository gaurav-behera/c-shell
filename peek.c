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
            printWarning("Invalid Flags");
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

    struct stat dirDetails;
    stat(pwd, &dirDetails);
    printf(WHITE_COLOR"total %ld\n" RESET_COLOR, dirDetails.st_blocks);

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
            printf(WHITE_COLOR);
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

            printf(WHITE_COLOR " %3hu", itemDetials.st_nlink);

            printf(WHITE_COLOR " %15s %7s", getpwuid(itemDetials.st_uid)->pw_name, getgrgid(itemDetials.st_gid)->gr_name);

            printf(WHITE_COLOR " %6ld", itemDetials.st_size);

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
            printf(WHITE_COLOR " %15s  ", time_buffer);
        }

        if (S_ISDIR(itemDetials.st_mode))
        {
            printf(BLUE_COLOR "%s" RESET_COLOR, itemList[i]->d_name);
        }
        else if (S_ISLNK(itemDetials.st_mode))
        {
            printf(CYAN_COLOR "%s" RESET_COLOR, itemList[i]->d_name);
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
