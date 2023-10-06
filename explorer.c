#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "explorer.h"
#include "mytar.h"

/*
 * PATH EXPLORER
 *
 * Recursively gets all paths from a given file or directory.
 */

/*
 * Gets the max path length allowed
 */
unsigned int getPathMax() {
    return PATH_MAX_DEF;
}

/*
 * Creates a path that can store up to
 * PATH_MAX_DEF characters
 */
char *createPath() {
    return (char *) malloc(getPathMax() * sizeof(char));
}

/*
 * Retrieve all children from directory
 * one level below
 */
void explorePathHelper(char *path, Stack *allPathsStack) {
    DIR *dp;
    struct dirent *dirp;
    struct stat info;

    if (path == NULL) {
        return;
    }

    /* 
     * get info about path 
     *
     * if there is a problem with the stat
     * then something went wrong => error out
     */
    if (lstat(path, &info) != 0) {
        printf("Error: cannot get path %s\n", path);
        return;
    }

    /* add the path to all paths */
    push(allPathsStack, path);

    /* if it is not a directory, nothing left to explore */
    if (!S_ISDIR(info.st_mode)) {
        return;
    }

    /* open the path directory */
    if ((dp = opendir(path)) == NULL)  {
        if (strcmp(path, "./") == 0) {
            printf("Error: cannot get current directory\n");
        }
    }

    /* loop through directories in DIR */
    while ((dirp = readdir(dp)) != NULL) {
        char *childPath;

        /* Ignore "." and ".." items. */
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
            continue;
        }

        /* check valid length */
        if (strlen(path) + strlen(dirp->d_name) + 1 >= getPathMax()) {
            printf("Error: path too long\n");
        }

        /* child path will be parent dir path + dir name */
        childPath = createPath();
        strcpy(childPath, path);
        /* if / isn't alread at the end, add it */
        strcat(childPath, childPath[strlen(path) - 1] != '/' ? "/" : "");
        strcat(childPath, dirp->d_name);

        if (stat(childPath, &info) != 0) {
            printf("Error: invalid path: %s\n", childPath);
        }

        /* recursively search down */
        explorePathHelper(childPath, allPathsStack);
    }

    /* close directory pointer */
    closedir(dp);
}

/* explore an array of paths and returns all associated paths */
Stack *explorePaths(char **paths, int numPaths) {
    int i;
    Stack* allPathsStack = createStackWithCapcity(1);

    /* traverse all paths from args */
    for (i = 0; i < numPaths; i++) {
        explorePathHelper(paths[i], allPathsStack);
    }

    return allPathsStack;
}
