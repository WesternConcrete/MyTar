#include "contents.h"

void printPermissions(PosixHeader *header) {
    long mode = strtol(header->mode, NULL, OCTAL_VAL);

    if (header->typeflag == DIRTYPE) {
        printf("d");
    } else if (header->typeflag == SYMTYPE) {
        printf("l");
    } else {
        printf("-");
    }

    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x " : "- ");
}

void printOwnerGroup(PosixHeader *header) {
    int owner_group_len;
    char *owner_group;

    /* print owner name */
    owner_group_len = strlen(header->uname) + strlen(header->gname) + 2;
    owner_group = (char *) calloc(owner_group_len, sizeof(char));
    strcpy(owner_group, header->uname);
    strcat(owner_group, "/");
    strcat(owner_group, header->gname);
    printf("%.17s ", owner_group);
    free(owner_group);
}

void printSizeFromHeader(PosixHeader *header) {
    long size = strtol(header->size, NULL, OCTAL_VAL);
    printf("%14ld ", size);
}

void printStatTime(PosixHeader *header) {
    long t_long = strtol(header->mtime, NULL, OCTAL_VAL);
    struct tm *time_info;
    char time_buffer[MTIME_WIDTH + 1]; /* save space for null */
    time_t rawtime;

    rawtime = (time_t) t_long;
    time_info = localtime(&rawtime);
    strftime(time_buffer, MTIME_WIDTH + 1, "%Y-%m-%d %H:%M", time_info);
    printf("%s ", time_buffer);
}

void listContentsForTarFile(
    char *tarfile, 
    char **paths, 
    int pathsLength, 
    int verbose,
    int strict_mode
) {
    int fd;
    size_t bytes_read;
    size_t r;
    PosixHeader *header;

    if ((fd = open(tarfile, O_RDONLY)) < 0) {
        handleBadTarFile();
    }

    header = (PosixHeader *) calloc(1, sizeof(PosixHeader));

    bytes_read = 0;
    while ((r = read(fd, (char *) header, BLOCK_SIZE)) == BLOCK_SIZE) {
        long size;
        int num_content_blocks;

        bytes_read += r;

        if (isEndBlock(header)) {
            /* check if the second end block follows */
            if (
                read(fd, (char *) header, BLOCK_SIZE) == BLOCK_SIZE &&
                isEndBlock(header)
            ) {
                break;
            } else {
                free(header);
                handleBadTarFile();
            }
        }

        if (!isValidHeader(header, strict_mode)) {
            free(header);
            handleBadTarFile();
        }

        size = strtol(header->size, NULL, OCTAL_VAL);
        num_content_blocks = size == 0 ? 0 : (size / BLOCK_SIZE) + 1;

        /* if there is content in the file, skip to the next header */
        if (num_content_blocks) {
            lseek(fd, num_content_blocks * BLOCK_SIZE, SEEK_CUR);
        }

        /* only show contents from specified files */
        if (!isDescendent(header, paths, pathsLength)) {
            continue;
        }

        if (verbose) {
            printPermissions(header);
            printOwnerGroup(header);
            printSizeFromHeader(header);
            printStatTime(header);
        }

        printFileNameFromHeader(header);
    }

    free(header);

    /* check that a valid number of bytes where in the tar file */
    if (bytes_read > 0 && bytes_read % BLOCK_SIZE != 0) {
        handleBadTarFile();
    }
}
