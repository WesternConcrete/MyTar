#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define USAGE "mytar [ctxvS]f tarfile [ path [ ... ] ]\n"

void handleUsageError(char *msg) {
    fprintf(stderr, "Usage: %s\n", msg);
    exit(1);
}

void parseArgs(
    int argc, 
    char *argv[],
    int *verbose,
    int *create_mode,
    int *table_of_contents_mode,
    int *extract_mode,
    int *strict_mode
) {
    char *options;

    if (argc < 2) {
        handleUsageError("no arguments");
    }

    options = argv[1];

    *verbose = strchr(options, 'v') != NULL ? 1 : 0;
    *create_mode = strchr(options, 'c') != NULL ? 1 : 0;
    *table_of_contents_mode = strchr(options, 't') != NULL ? 1 : 0;
    *extract_mode = strchr(options, 'x') != NULL ? 1 : 0;
    *strict_mode = strchr(options, 'S') != NULL ? 1 : 0;

    /* f is required */
    if (strchr(options, 'f') == NULL) {
        handleUsageError("no F");
    }

    /* one of 'c' 't' or 'x' is required to be present */
    if (!*table_of_contents_mode && !*create_mode && !*extract_mode) {
        handleUsageError("no C, T, or X");
    }
}

char *getTarfileFromArgs(int argc, char *argv[]) {
    char *tarfile;
    if (argc < 3) {
        handleUsageError("f switch with no filename");
    }
    tarfile = (char *) calloc(strlen(argv[2]) + 1, sizeof(char));
    strcpy(tarfile, argv[2]);
    return tarfile;
}

char **getPathsFromArgs(int argc, char *argv[], int *numPaths) {
    int i;
    char **paths;

    if (argc < 4) {
        *numPaths = 0;
        return NULL;
    }

    *numPaths = argc - 3;
    paths = (char **) calloc(*numPaths, sizeof(char *));

    for (i = 3; i < argc; i++) {
        int len = strlen(argv[i]);
        char *path = (char *) calloc(len + 1, sizeof(char));
        strcpy(path, argv[i]);
        paths[i - 3] = path;
    }

    return paths;
}
