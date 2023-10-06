#include "conformer.h"
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
#include <math.h>
#include "mytar.h"

void createTarfileFromPaths(
    char *tarfile, 
    char **paths, 
    int pathsLength, 
    int verbose
);
