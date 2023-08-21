#ifndef __COMMANDS_H
#define __COMMANDS_H

extern char home[4096];

void setup();

char *getRelativePath(char *pwd);
char *getAbsolutePath(char *pwd, char *path);
char *getRealPath(char *path, char *newpath);
int executeCommand(char *command);
int executeCommand_working(char *command);
void saveInput(char *path);

void executeInForeground(char *command);
void executeInBackground(char *command);
void checkBackgroundCompletion();

char *getParentPath(char *path);
void savePath(char *path);

int warp(int argc, char *argv[]);
int peek(int argc, char *argv[]);
int seek(int argc, char *argv[]);
int pastevents(int argc, char *argv[]);

#endif