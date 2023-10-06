#ifndef _MYTARUTILS_H
#define _MYTARUTILS_H  1

#include "mytar.h"
#include "tar.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <utime.h>

void printFileNameFromHeader(PosixHeader *header);

void getFileNameFromHeader(PosixHeader *header, char *name);

void handleBadTarFile();

int isValidHeader(PosixHeader *header, int strict_mode);

int isEndBlock(PosixHeader *header);

int isDescendent(
    PosixHeader *header,
    char **paths, 
    int pathsLength
);

#endif
