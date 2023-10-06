/*this file holds functions that aid in extracting the tar file
 * and turning it into a file system.*/

#include "extract.h"

/*this function builds a path of directories given a path/directory name.*/
void mkdir_deep(char *path, mode_t file_perms) {
    int i;
    char *new_path;
    struct stat buf;
    for(i = 0; path[i]; i++){
        if(path[i] == '/'){
            /*does not build a directory if the directory exists*/
            new_path = strndup(path, i);
            if(stat(new_path, &buf) == 0){
                free(new_path);
                continue;
            }
            if(mkdir(new_path, file_perms) == -1){
                perror("mkdir");
                exit(1);
            }
            free(new_path);
        }
    }
}

/*returns the permissions of the current directory*/
mode_t currentDirPerms(){
    struct stat buf;
    if(stat(".", &buf) == -1){
        perror("stat");
        exit(1);
    }
    return buf.st_mode;
}

/*returns the access time of the file*/
mode_t getATime(char *filename){
    struct stat buf;
    if(stat(filename, &buf) == -1){
        perror("stat");
        exit(1);
    }
    return buf.st_atime;
}


void extractTarFile(
    char *tarfile,
    char **paths,
    int pathsLength,
    int verbose,
    int strict_mode
) {
    int fd, built_fd = 0;
    PosixHeader *header = (PosixHeader *) calloc(1, sizeof(PosixHeader));
    struct utimbuf *times;
    time_t mtime;
    char filename[PATH_MAX_DEF];
    char *file_data;
     
    if ((fd = open(tarfile, O_RDONLY)) < 0) {
        handleBadTarFile();
    }
    /*checks loops through blocks of tar file*/
    while (read(fd, (char *) header, BLOCK_SIZE) == BLOCK_SIZE) {
        long size;
        int num_content_blocks;

        if (isEndBlock(header)) {
            if (
                read(fd, (char *) header, BLOCK_SIZE) == BLOCK_SIZE &&
                isEndBlock(header)
            ) {
                break;
            } else {
                handleBadTarFile();
            }
        }

        if (!isValidHeader(header, strict_mode)) {
            handleBadTarFile();
        } 

        size = strtol(header->size, NULL, OCTAL_VAL);
        num_content_blocks = size == 0 ? 0 : (size / BLOCK_SIZE) + 1;
         
        getFileNameFromHeader(header, filename);
        /*checks if the current header is a descendent of tar file*/
        if (!isDescendent(header, paths, pathsLength)) {
            lseek(fd, num_content_blocks * BLOCK_SIZE, SEEK_CUR);
            continue;
        }

        
        if(header->typeflag == '5'){
            mkdir_deep(filename, RWXALL);
        } else if (header->typeflag == '0' || header->typeflag == '\0'){
            /* builds parent directories of a file if those directories
            * dont exist*/
            mkdir_deep(filename, RWXALL);
            if(((built_fd = open(
                    filename, O_CREAT|O_WRONLY|O_TRUNC, RWALL)) == -1)){
                perror(filename);
                exit(1);
            }
        } else if (header->typeflag == '2'){
            if(-1 == (symlink(header->linkname, filename))){
                perror("symlink");
                exit(1);
            }
        }

        if(!(times = malloc(sizeof(struct utimbuf)))){
            perror("malloc");
            exit(1);
        }
        /*sets m time of file*/
    mtime = strtol(header->mtime, NULL, OCTAL_VAL);
        times->modtime = mtime;
        if (header->typeflag != '2'){
            times->actime = getATime(filename);
        } else {
            times->actime = mtime;
        }
   
        if(header->typeflag != '2' && utime(filename, times) == -1){
            perror("utime");
            exit(1);
        }
           
        
        if (verbose) {
            printFileNameFromHeader(header);
        }
        
        /*if there is data in the body, write that data to
        * created file*/
        if(built_fd && size){
            file_data = malloc(size * sizeof(char));
            if(-1 == read(fd, file_data, size)){
                perror("read");
                exit(1);
            }
            if(-1 == write(built_fd, file_data, size)){
                perror("write");
                exit(1);
            }
            free(file_data);
            /*skip over block padding*/
            lseek(fd, num_content_blocks * BLOCK_SIZE - size, SEEK_CUR);
            close(built_fd); 
        }
        free(times);
    }
    
    free(header);
}
