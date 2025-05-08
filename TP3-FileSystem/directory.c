#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
    struct inode in;
    if (inode_iget(fs, dirinumber, &in) < 0) {
        return -1;
    }

    if ((in.i_mode & IFMT) != IFDIR) {
        return -1;
    }

    int size = inode_getsize(&in);
    int entries = size / sizeof(struct direntv6);
    char block[DISKIMG_SECTOR_SIZE];

    for (int i = 0; i < entries; i++) {
        int blockNum = (i * sizeof(struct direntv6)) / DISKIMG_SECTOR_SIZE;
        int offset = (i * sizeof(struct direntv6)) % DISKIMG_SECTOR_SIZE;

        if (offset == 0) {
            if (file_getblock(fs, dirinumber, blockNum, block) < 0) {
                return -1;
            }
        }

        struct direntv6 *entry = (struct direntv6 *)(block + offset);
        if (strncmp(entry->d_name, name, 14) == 0) {
            *dirEnt = *entry;
            return 0;
        }
    }

    return -1;
}
