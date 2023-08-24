#ifndef __PASTEVENTS_H_
#define __PASTEVENTS_H_

int pastevents(int argc, char *argv[]);

void saveInput(char *path);
char *getFromHistory(int position, bool showErrors);
void printHistory(char *text);

#endif