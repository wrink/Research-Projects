#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

#include "disk.h"
#include "sfs.h"

int test_mount_and_format(struct sfs_disk* disk) {
        int ret, error = 0;
        printf("\n-------------------------------------------\n");;
        printf("# Formatting disk...\n");
        ret = sfs_format(disk);
        if(ret == -1) {
                printf("ERROR: format failed\n");
                error=1;
        }
        printf("# Mounting disk...\n");
        ret = sfs_mount(disk, NULL);
        if(ret == -1) {
                printf("ERROR: mount failed\n");
                error = 1;
        }
        sfs_print_super(&disk->super);
        if(error) {
                printf("# test_mount_and_format FAILED\n");
        }
        else {
                printf("# test_mount_and_format PASSED\n");
        }
        return error;
}

int test_open_new_and_write(struct sfs_disk* disk) {
        int ret, error = 0;
        printf("\n-------------------------------------------\n");
        printf("# Creating file...\n");
        int fd = sfs_open(disk, "file1", 1);
        if(fd < 0) {
                printf("ERROR: creating file failed\n");
                error = 1;
        }
        char* string = "This string is 29 bytes long"; // including \0
        printf("# Writing '%s' to file...\n", string);
        ret = sfs_write(disk, fd, string, strlen(string)+1);
        if(ret == -1) {
                printf("ERROR: write failed\n");
                error = 1;
        }
        if(fd >= 0) {
                sfs_close(disk, fd);
        }
        if(error) {
                printf("# test_open_new_and_write FAILED\n");
        }
        else {
                printf("# test_open_new_and_write PASSED\n");
        }
        return error;
}

int test_open_new_write_close_open_read(struct sfs_disk* disk) {
        int ret, error = 0;
        char* string2 = (char*) malloc(128);
        printf("\n-------------------------------------------\n");
        printf("# Creating file...\n");
        int fd = sfs_open(disk, "file2", 1);
        if(fd < 0) {
                printf("ERROR: creating file failed\n");
                error = 1;
        }
        char* string = "This string is 29 bytes long"; // including \0
        printf("# Writing '%s' to file...\n", string);
        ret = sfs_write(disk, fd, string, strlen(string)+1);
        if(ret == -1) {
                printf("ERROR: write failed\n");
                error = 1;
        }
        ret = sfs_close(disk, fd);
        if(ret == -1) {
                printf("ERROR: close failed\n");
                error = 1;
        }
        fd = sfs_open(disk, "file2", 0);
        if(fd < 0) {
                printf("ERROR: open existing file failed\n");
                error = 1;
        }
        ret = sfs_read(disk, fd, string2, 29);
        if(ret == -1) {
                printf("ERROR: read existing file failed\n");
                error = 1;
        }
        else {
                printf("Read string: '%s'\n", string2);
                if(strncmp(string2, string, 29) != 0) {
                        printf("ERROR: Read doesn't match write!\n");
                        error = 1;
                }
        }
        if(fd >= 0) {
                sfs_close(disk, fd);
        }
        if(error) {
                printf("# test_open_new_write_close_open_read FAILED\n");
        }
        else {
                printf("# test_open_new_write_close_open_read PASSED\n");
        }
        return error;
}


int test_open_write_variable(struct sfs_disk* disk, char* filename, int wsize, int create) {
        int ret;
        printf("\n-------------------------------------------\n");
        char* string = (char*) malloc(wsize);
        memset(string, 'a', wsize-1); // fill string with the a's
        string[wsize-1] = '\0'; // add termination character at end
        int fd = sfs_open(disk, filename, create);
        ret = sfs_write(disk, fd, string, strlen(string)+1);
        if(fd >= 0) {
                sfs_close(disk, fd);
        }
        if(ret == -1) {
                printf("# test_open_write_variable %s FAILED\n", filename);
        }
        else {
                printf("# test_open_write_variable %s PASSED\n", filename);
        }
}

int test_open_write_seek_read(struct sfs_disk* disk)
{
        int error;
        char* string = (char*) malloc(13);
        printf("\n-------------------------------------------\n");
        int fd = sfs_open(disk, "wrseekrd", 1);
        printf("Writing 'test message'...\n");
        sfs_write(disk, fd, "test message", 12);
        sfs_seek(disk, fd, 0, SEEK_SET);
        sfs_read(disk, fd, string, 12);
        string[13] = '\0';
        printf("Read '%s'.\n", string);
        sfs_close(disk, fd);
        error = strncmp(string, "test message", 12);
        if(error != 0) {
                printf("# test_open_write_seek_read FAILED\n");
        }
        else {
                printf("# test_open_write_seek_read PASSED\n");
        }
        return error;
}

int main(int argc, char *argv[])
{
        struct sfs_disk disk;
        disk.data = (char *) malloc(SFS_NUM_BLOCKS*SFS_BLOCK_SIZE);
        if(disk.data == NULL) {
                printf("Error allocating disk memory!\n");
                return -1;
        }

        // mount and format SFS
        test_mount_and_format(&disk);

        // try to open a new file and write to it
        test_open_new_and_write(&disk); // passes in provided code
        // try to write in a new file, seek back to start, and read
        test_open_write_seek_read(&disk); // needs seek to work
        // try to open a new file, write to it, reopen it, and read
        test_open_new_write_close_open_read(&disk); // needs directory support
        // try to write a new short file
        test_open_write_variable(&disk, "shortfile", 16, 1);
        // try to reopen and write to short file again
        test_open_write_variable(&disk, "shortfile", 32, 0);
        // try to write a new long file
        test_open_write_variable(&disk, "longfile", 400, 1);

        // list the directory. This will only work once we have support for multiple files.
        sfs_ls_dir(&disk, &disk.root_dir_inode);

        return 0;
}
