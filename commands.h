#ifndef __COMMANDS_H
#define __COMMANDS_H

extern char home[4096];

void setup();

char *modifyInput(char *input);
char *getRelativePathHome(char *pwd);
char *getAbsolutePath(char *pwd, char *path);
char *getRealPath(char *path, char *newpath);
char *getLastCommand();
int executeCommand(char *command);
int executeCommand_working(char *command);
void saveInput(char *path);
char *getFromHistory(int position);
pid_t getShellPID();

void executeInForeground(char *command);
char * getComamndName(char *command);
void executeInBackground(char *command);
void checkBackgroundCompletion();

char *getParentPath(char *path);
void savePath(char *path);

int warp(int argc, char *argv[]);
int peek(int argc, char *argv[]);
int seek(int argc, char *argv[]);
int pastevents(int argc, char *argv[]);
int proc(int argc, char * argv[]);

#endif