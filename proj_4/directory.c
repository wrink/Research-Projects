#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

#include "disk.h"
#include "sfs.h"


/* Read entry `n` from a directory and store its info into `dir`. Returns 0 on
 * success or -1 on failure */
int sfs_read_dir_entry(struct sfs_disk* disk, struct sfs_inode* dir_inode,
        int n, struct sfs_dir_entry* dir)
{
        int dir_block_index = 0; // TODO: currently only support using the first data block in an inode
        int dir_block = dir_inode->block[dir_block_index];  // FIXME: this should be the actual block ID to read from
        int dir_offset = n; // FIXME: this should be the byte offset inside the block to read from (based on n)

        // find the right offsets and read the inum, file name, and file name length from disk
        // remember, if the strlen field in the dir_entry is 0 that means it is unused
        // return 0 if this is a valid dir entry, or -1 if it is unused.

        disk_read(disk->data, dir_block, dir_offset, dir, SFS_DIR_ENTRY_SIZE);

        if(dir->strlen == 0) return -1;

        return 0;
        //return -1;
}

/* Create a new file entry in a directory based on info in `direntry`. Returns 0
 * index of the entry or -1 on failure */
int sfs_create_dir_entry(struct sfs_disk* disk, struct sfs_inode* dir_inode,
        struct sfs_dir_entry* direntry)
{
        int dir_block_index = 0; // TODO: currently only support using the first data block in an inode
        int n; // FIXME: this should be the index of the new entry
        /* for example n=0 means this is the first entry in the data block
         * and n=7 is the last entry in the data block, since we can fit 8 dir entries per block (128/16)
         * and n=12 would be an entry in the second data block (once we add support for multi-block dirs!)*/
        int dir_block = dir_inode->block[dir_block_index];  // FIXME: this should be the actual block ID to write to
        //int dir_offset; // FIXME: this should be the byte offset inside the block to write to (based on n)

        // Read through the dir_entries currently stored on disk to find an empty one (n)
        // then figure out which block that is and what offset it is within the block

        /* Note: we should implement read_dir_entry first, since this function will need to
         * read through the dir_entries in the dir_inode's data block until it finds an unused one.
         * Once we find a free entry on disk, write the values from direntry into it. */

        struct sfs_dir_entry* frame = malloc(SFS_DIR_ENTRY_SIZE);
        for(n = 0; n < 8; n++) {
                if(sfs_read_dir_entry(disk, dir_inode, n, frame) == -1) {
                        int dir_block_index = n >> 3;
                        int dir_block = dir_inode->block[dir_block_index];
                        int dir_offset = n & 7;

                        disk_write(disk->data, dir_block, n, direntry, SFS_DIR_ENTRY_SIZE);
                        return 0;
                }
        }

        return -1;
        //return -1;
}

/* List out a directory by scanning all of its data blocks for valid dir_entries. */
void sfs_ls_dir(struct sfs_disk* disk, struct sfs_inode* dir_inode)
{
        if(dir_inode->type != 2) {
                printf("ERROR: tried to list a non-directory inode\n");
                return;
        }
        printf("          NAME     TYPE       SIZE         BLOCK LIST\n");
        int max_used_entries = dir_inode->used_blocks * SFS_BLOCK_SIZE / SFS_DIR_ENTRY_SIZE;
        for(int n=0; n < max_used_entries; n++) {
                struct sfs_dir_entry dir;
                int invalid;
                invalid = sfs_read_dir_entry(disk, dir_inode, n, &dir);
                if(invalid == 0) {
                        sfs_print_dir_entry(disk, &dir);
                }
        }
}

/* Print the file or directory name and data from its inode */
void sfs_print_dir_entry(struct sfs_disk* disk, struct sfs_dir_entry* dir) {
        printf(" %16s", dir->name);
        struct sfs_inode inode;
        sfs_read_inode(disk, dir->inum, &inode);
        sfs_print_inode(&inode);
}

/* Given a file/directory path, find the struct for it and fill in `entry`.
 * Only searches root directory. Returns 0, or -1 on failure */
int sfs_find_dir_entry(struct sfs_disk* disk, char* filename, struct sfs_dir_entry* entry)
{
        int n;
        struct sfs_inode* root_dir = &disk->root_dir_inode; // TODO: will need to change this to support nested directories

        /* FIXME: need to read each dir entry from disk and see if it has
         * the same filename. If it does, read all of its data into `entry` and return 0.
         * If the file can't be found, return -1. */

        return -1;

}
