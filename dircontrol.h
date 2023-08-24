#ifndef __DIRCONTROL_H_
#define __DIRCONTROL_H_

char *getRelativePathHome(char *pwd);
char *getRelativePath(char *pwd, char *base);

char *getAbsolutePath(char *pwd, char *path);

char *getParentPath(char *path);
void savePath(char *path);
char *retriveLastPath();

#endif