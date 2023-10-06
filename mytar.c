#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "explorer.h"
#include "contents.h"
#include "create.h"
#include "extract.h"

int main(int argc, char *argv[]) {
    int verbose;
    int create_mode;
    int table_of_contents_mode;
    int extract_mode;
    int strict_mode;
    char *tarfile;
    char **paths;
    int numOfArgumentPaths = 0;
    Stack *allPathsStack;
    int i;

    /* get option values */
    parseArgs(
        argc, 
        argv,
        &verbose,
        &create_mode,
        &table_of_contents_mode,
        &extract_mode,
        &strict_mode
    );

    /* get tarfile and any paths */
    tarfile = getTarfileFromArgs(argc, argv);

    numOfArgumentPaths = 0;
    paths = getPathsFromArgs(argc, argv, &numOfArgumentPaths);

    if (table_of_contents_mode) {
        listContentsForTarFile(
            tarfile, 
            paths, 
            numOfArgumentPaths, 
            verbose,
            strict_mode
        );
    }

    if (create_mode) {
        allPathsStack = explorePaths(paths, numOfArgumentPaths);
        createTarfileFromPaths(
            tarfile, 
            allPathsStack->array, 
            allPathsStack->top + 1, 
            verbose
        );

        /* free stack and with that, the paths */
        freeStackArray(allPathsStack);
        freeStack(allPathsStack);
    }

    if (extract_mode) {
        extractTarFile(
            tarfile, 
            paths, 
            numOfArgumentPaths, 
            verbose, 
            strict_mode
        );
    }

    if (table_of_contents_mode || extract_mode) {
        /* free paths */
        for (i = 0; i < numOfArgumentPaths; i++) {
            free(paths[i]);
        } 
    }

    /* free tarfile */
    free(tarfile);

    /* free paths */
    free(paths);

    return 0;
}
