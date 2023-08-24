#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

extern char home[4096];

#include "colors.h"

#include "prompt.h"
#include "setup.h"
#include "warp.h"
#include "peek.h"
#include "seek.h"
#include "pastevents.h"
#include "proclore.h"
#include "execute.h"
#include "dircontrol.h"

#endif