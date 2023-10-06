#include "mytarutils.h"

void printFileNameFromHeader(PosixHeader *header) {
    char name[PATH_MAX_DEF];
    getFileNameFromHeader(header, name);
    printf("%s\n", name);
}

void getFileNameFromHeader(PosixHeader *header, char *name) {
    int len;

    len = 0;
    if (strlen(header->prefix) > 0) {
        strcpy(name, header->prefix);
        len += strlen(header->prefix);
        strcat(name, "/");
        len += 1;
        strncat(name, header->name, TNAMELEN);
        len += TNAMELEN;
    } else {
        strncpy(name, header->name, TNAMELEN);
        len += strlen(header->name) > TNAMELEN ? 
            TNAMELEN : 
            strlen(header->name);
    }

    name[len] = '\0';
}

void handleBadTarFile() {
    fprintf(stderr, "Malformed header found.  Bailing.\n");
    exit(1);
}

int isValidHeader(PosixHeader *header, int strict_mode) {
    long chksum = strtol(header->chksum, NULL, OCTAL_VAL);
    unsigned int check = 0;
    int i;

    /* set initial value to empty spaces */
    memset(header->chksum, ' ', TCHKSUMLEN); 

    for(i = 0; i < BLOCK_SIZE; i++){
        check += (unsigned char) ((char *) header)[i];
    }
    snprintf(header->chksum, sizeof(header->chksum), "%07o", check);

    if (check != chksum) {
        return 0;
    }

    if (strncmp(header->magic, TMAGIC, TMAGLEN - 1) != 0) {
        /* make sure magic is ustar */
        return 0;
    }

    if (strict_mode && strncmp(header->version, TVERSION, TVERSLEN) != 0) {
        /* make sure version is 00 in strict mode */
        return 0;
    }

    if (strict_mode && strcmp(header->magic, TMAGIC) != 0) {
        /* make sure magic is null terminated in strict mode */
        return 0;
    }

    if (
        header->typeflag != SYMTYPE && 
        header->typeflag != DIRTYPE && 
        header->typeflag != REGTYPE
    ) {
        return 0;
    }

    return 1;
}

int isEndBlock(PosixHeader *header) {
    int i;
    char *block = (char *) header;

    for (i = 0; i < BLOCK_SIZE; i++) {
        if (block[i] != '\0') {
            return 0;
        }
    }

    return 1;
}

/* check if filename is decendent of specified paths */
int isDescendent(
    PosixHeader *header,
    char **paths, 
    int pathsLength
) {
    int valid_header = 0;
    char filename[PATH_MAX_DEF];
    int i;

    if (pathsLength == 0) {
        return 1;
    }

    getFileNameFromHeader(header, filename);

    for (i = 0; i < pathsLength; i++) {
        if (strstr(filename, paths[i]) == filename) {
            valid_header = 1;
            break;
        }
    }

    return valid_header;
}
