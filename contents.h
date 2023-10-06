#include "mytarutils.h"

#define PERMISSIONS_WIDTH 10
#define OWNER_GROUP_WIDTH 10
#define SIZE_WIDTH 14
#define MTIME_WIDTH 16

void listContentsForTarFile(
    char *tarfile, 
    char **paths, 
    int pathsLength, 
    int verbose,
    int strict_mode
);
