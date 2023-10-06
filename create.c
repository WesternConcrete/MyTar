#include "create.h"

/* Insert name into header, and if needed, prefix */
void getNameFromPath(char *path, int isDir, PosixHeader *header) {
    int len = strlen(path);
    int i;

    if (len <= TNAMELEN) {
        /* len is <= 100, don't need prefix */
        strncpy(header->name, path, TNAMELEN);
    } else {
        /* len is > 100, overflow to prefix */
        int temp = -1;
        i = len - 1;

        /* get the last / character that fits in the name space */
        while((len - i) < TNAMELEN) {
            if (path[i] == '/') {
                temp = i;
            }
            i--;
        }

        /* if name len is > 100 than it can't be archived */
        if (temp < 0) {
            printf("Error: name is too long\n");
            return;
        }

        /* copy the name and prefix to the header data */

        /* add 1 to account for the '/' */
        strncpy(header->name, path + temp + 1, TNAMELEN); 
        /* overflow into the prefix */
        strncpy(header->prefix, path, temp);
    }

    /* make sure dir names end in slash */
    if (isDir && header->name[len - 1] != '/') {
        strcat(header->name, "/");
    }
}

/* Create header data from path */
PosixHeader *createHeaderBlock(char *path) {
    PosixHeader *header;
    struct stat info;
    struct passwd *p;
    struct group *grp;
    unsigned int check = 0;
    int i;

    if (lstat(path, &info) != 0) {
        printf("Error: lstat path: %s\n", path);
    }

    header = (PosixHeader *) calloc(1, sizeof(PosixHeader));

    /* NAME */
    getNameFromPath(path, S_ISDIR(info.st_mode), header);

    /* MODE */
    sprintf(header->mode, "%07o", info.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));

    /* UID */
    if (info.st_uid > 07777777) {
        insert_special_character(header->uid, TUIDLEN, (int32_t) info.st_uid);
    } else {
        sprintf(header->uid, "%07o", info.st_uid);
    }

    /* GID */
    if (info.st_gid > 07777777) {
        insert_special_character(header->gid, TGIDLEN, (int32_t) info.st_gid);
    } else {
        sprintf(header->gid, "%07o", info.st_gid);
    }

    /* MTIME */
    sprintf(header->mtime, "%011o", (int) info.st_mtime);

    /* TYPEFLAG + SIZE */
    if (S_ISDIR(info.st_mode)) {
        memset(header->size, '0', TSIZELEN - 1);
        header->typeflag = DIRTYPE;
    } else if (S_ISLNK(info.st_mode)) {
        memset(header->size, '0', TSIZELEN - 1);

        /* LINK VALUE */
        if (readlink(path, header->linkname, TLINKNAMELEN) < 0){
            printf("Could not read link %s\n", path);
        }

        header->typeflag = SYMTYPE;
    } else if (S_ISREG(info.st_mode)) {
        sprintf(header->size, "%011o", (int) info.st_size);
        header->typeflag = REGTYPE;
    }

    /* DEVMAJOR / DEVMINOR */
    if (S_ISCHR(info.st_mode) || S_ISBLK(info.st_mode)) {
        /* get character device major and minor values */
        snprintf(header->devmajor, sizeof(header->devmajor), 
                "%06o ", major(info.st_rdev));
        snprintf(header->devminor, sizeof(header->devminor), 
                "%06o ", minor(info.st_rdev));
    }

    /* VERSION */
    strncpy(header->version, TVERSION, TVERSLEN);

    /* MAGIC */
    strcpy(header->magic, TMAGIC);

    /* USER NAME */
    if ((p = getpwuid(info.st_uid)) == NULL) {
        perror("getpwuid() error");
    }
    strncpy(header->uname, p->pw_name, sizeof(header->uname) - 1);

    /* GROUP NAME */
    if ((grp = getgrgid(info.st_gid)) == NULL) {
        perror("getgrgid() error");
    }
    strncpy(header->gname, grp->gr_name, sizeof(header->gname) - 1);
    
    /* CHECK SUM */

    /* set initial value to empty spaces */
    memset(header->chksum, ' ', TCHKSUMLEN); 

    for(i = 0; i < BLOCK_SIZE; i++) {
        check += (unsigned char) ((char *) header)[i]; /* sum up bytes */
    }
    snprintf(header->chksum, sizeof(header->chksum), "%07o", check);

    return header;
}

/* Read a block with a specified size from a file  */
int read_block(int fd, char *buf, int size){
    int bytes_read = 0, read_count;

    while (
        (bytes_read < size) && 
        ((read_count = read(fd, buf + bytes_read, size - bytes_read)) > 0)
    ){
        bytes_read += read_count;
    }

    return bytes_read;
}

/* Write end of block, padding with \0 if necessary */
int write_end(const int fd, int lastBlockSize, char *nullBlock) {
    if (fd < 0){
        return 0;
    }

    /* pad current block with 0 bytes if needed */
    if (
        lastBlockSize < BLOCK_SIZE && 
        lastBlockSize != 0 &&
        (
            write(fd, nullBlock, BLOCK_SIZE - lastBlockSize) !=
                (BLOCK_SIZE - lastBlockSize)
        )
    ) {
        printf("Error: Unable to pad tar file\n");
        return 0;
    }

    return 1;
}

/* CREATE TARFILE */
void createTarfileFromPaths(
    char *tarfile, 
    char **paths, 
    int pathsLength, 
    int verbose
) {
    int i;
    int out;
    char nullBlock[BLOCK_SIZE];

    /* 
     * set the null block with null values,
     * will use this later on in the function
     */
    memset(nullBlock, '\0', sizeof(nullBlock));

    /* open the out (tar) file */
    if((out = open(tarfile, O_WRONLY | O_CREAT | O_TRUNC, 0744)) < 0) {
        perror(tarfile);
        exit(1);
    }

    /* loop through all the paths */
    for (i = 0; i < pathsLength; i++) {
        PosixHeader *header;
        int fd;
        int r = 0;
        int lastBlockSizeWritten = 0;
        char buf[BLOCK_SIZE];

        fd = open(paths[i], O_RDONLY);

        if (fd < 0) {
            printf("Error: could not open path for reading: %s\n", paths[i]);
            continue;
        }

        if (verbose) {
            printf("%s\n", paths[i]);
        }

        /* get header data from path */
        header = createHeaderBlock(paths[i]);

        /* write header to tar file */
        write(out, (char *) header, BLOCK_SIZE);

        /* read a block from infile, and write to tar file */
        while ((r = read_block(fd, buf, BLOCK_SIZE)) > 0){
            if (write(out, buf, r) != r) {
                printf("Could not write to archive: %s\n", tarfile);
            }
            lastBlockSizeWritten = r;
        }

        /* pad last block with null if needed */
        write_end(out, lastBlockSizeWritten, nullBlock);

        close(fd);
        free(header);
    }

    /* add closing 2 null bytes */
    for(i = 0; i < NUM_CLOSING_BLOCKS; i++) {
        int w;
        if ((w = write(out, nullBlock, BLOCK_SIZE)) != BLOCK_SIZE){
            printf("Error: Unable to close tar file %d\n", w);
        }
    }

    close(out);
}
