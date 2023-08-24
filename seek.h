#ifndef __SEEK_H_
#define __SEEK_H_

int seek(int argc, char *argv[]);

char *searchItem(char *search, char *dir, char *originalDir, int *count, bool fFlag, bool dFlag);

#endif