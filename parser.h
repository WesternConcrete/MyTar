void parseArgs(
    int argc, 
    char *argv[],
    int *verbose,
    int *create_mode,
    int *table_of_contents_mode,
    int *extract_mode,
    int *strict_mode
);

char *getTarfileFromArgs(int argc, char *argv[]);

char **getPathsFromArgs(int argc, char *argv[], int *numPaths);
