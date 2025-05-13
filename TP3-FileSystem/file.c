#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf)
{
    struct inode in;
    if (inode_iget(fs, inumber, &in) < 0)
    {
        return -1;
    }

    int sector = inode_indexlookup(fs, &in, blockNum);
    if (sector == -1)
    {
        return -1;
    }

    int filesize = inode_getsize(&in);
    int totalBlocks = (filesize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;

    if (blockNum >= totalBlocks)
    {
        return -1;
    }

    if (diskimg_readsector(fs->dfd, sector, buf) < 0)
    {
        return -1;
    }

    if (blockNum == totalBlocks - 1)
    {
        return filesize % DISKIMG_SECTOR_SIZE ? filesize % DISKIMG_SECTOR_SIZE : DISKIMG_SECTOR_SIZE;
    }

    return DISKIMG_SECTOR_SIZE;
}
