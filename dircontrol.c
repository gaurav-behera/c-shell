#include "headers.h"

char *getRelativePathHome(char *pwd)
{
    char *relPath = malloc(sizeof(char) * 4096);
    if (strcmp(pwd, home) == 0)
    {
        strcpy(relPath, "~");
    }
    else if (strncmp(pwd, home, strlen(home)) == 0)
    {
        strcpy(relPath, "~");
        strcpy(relPath + 1, pwd + strlen(home));
    }
    else
    {
        strcpy(relPath, pwd);
    }
    return relPath;
}

char *getRelativePath(char *pwd, char *base)
{
    char *relPath = malloc(sizeof(char) * 4096);
    if (strcmp(pwd, base) == 0)
    {
        strcpy(relPath, "./");
    }
    else if (strncmp(pwd, base, strlen(base)) == 0)
    {
        strcpy(relPath, ".");
        strcpy(relPath + 1, pwd + strlen(base));
    }
    else
    {
        char *relPathHome = getRelativePathHome(pwd);
        strcpy(relPath, relPathHome);
        free(relPathHome);
    }
    return relPath;
}

char *getParentPath(char *path)
{
    char *parentPath = malloc(sizeof(char) * 4096);
    int endPos = -1;
    for (int i = 1; i < strlen(path); i++)
    {
        if (path[i] == '/')
        {
            endPos = i;
        }
    }
    if (endPos == -1)
    {
        strncpy(parentPath, "/\0", 2);
        return parentPath;
    }
    strncpy(parentPath, path, endPos);
    return parentPath;
}

void savePath(char *path)
{
    char text[4096] = "";
    char shellprofilepath[4096] = "";
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
    char text[4096] = "";
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

char *getAbsolutePath(char *pwd, char *path)
{
    char *newpath = malloc(sizeof(char) * 4096);
    strcpy(newpath, pwd);
    char *dir = strtok(path, "/");
    bool flag = 1;
    if (dir == NULL)
    {
        strcpy(newpath, "/");
        return newpath;
    }
    
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
                strcat(newpath + strlen(newpath), "/");
                strcat(newpath + strlen(newpath), dir);
            }
        }
        flag = 0;
        dir = strtok(NULL, "/");
    }
    return newpath;
}
