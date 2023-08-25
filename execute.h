#ifndef __EXECUTE_H_
#define __EXECUTE_H_

void saveProcess(pid_t pid, char * name);
void removeProcess(pid_t pid);
char * getProcessName(pid_t pid);

void executeCommand(char *command, char delimiter);
int checkBackgroundCompletion();

char * getComamndName(char *command);
char *getLastCommand();
void saveLastCommand(char * command);

char *modifyInput(char *input);

void printError();
void printWarning(char * message);
void printErrorMsg(char * message);

#endif