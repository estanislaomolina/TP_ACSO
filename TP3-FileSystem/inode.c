#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "inode.h"
#include "diskimg.h"

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp)
{
    if (inumber < 1)
    {
        return -1;
    }

    int inodeBlock = ((inumber - 1) / (DISKIMG_SECTOR_SIZE / sizeof(struct inode))) + INODE_START_SECTOR;
    struct inode buffer[DISKIMG_SECTOR_SIZE / sizeof(struct inode)];

    int bytes = diskimg_readsector(fs->dfd, inodeBlock, buffer);
    if (bytes == -1)
    {
        return -1;
    }

    int inodeIndex = (inumber - 1) % (DISKIMG_SECTOR_SIZE / sizeof(struct inode));
    *inp = buffer[inodeIndex];

    return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum)
{
    if ((inp->i_mode & ILARG) == 0)
    {
        if (blockNum < 0 || blockNum >= 8)
        {
            return -1;
        }
        return inp->i_addr[blockNum];
    }
    else
    {
        // ILARG = archivo grande, usar bloques indirectos
        if (blockNum < 0 || blockNum >= 7 * 256 + 256 * 256)
        {
            return -1;
        }

        int indirectIndex = blockNum / 256;
        int indirectOffset = blockNum % 256;

        if (indirectIndex < 7)
        {
            uint16_t indirectBlock[256];
            int indirectBlockNum = inp->i_addr[indirectIndex];

            if (diskimg_readsector(fs->dfd, indirectBlockNum, indirectBlock) < 0)
            {
                return -1;
            }

            return indirectBlock[indirectOffset];
        }
        else
        {
            uint16_t doubleIndirectBlock[256];
            int doubleIndirectBlockNum = inp->i_addr[7];

            if (diskimg_readsector(fs->dfd, doubleIndirectBlockNum, doubleIndirectBlock) < 0)
            {
                return -1;
            }

            int first = (blockNum - 7 * 256) / 256;
            int second = (blockNum - 7 * 256) % 256;

            uint16_t indirectBlock[256];
            if (diskimg_readsector(fs->dfd, doubleIndirectBlock[first], indirectBlock) < 0)
            {
                return -1;
            }

            return indirectBlock[second];
        }
    }
}

int inode_getsize(struct inode *inp)
{
    return ((inp->i_size0 << 16) | inp->i_size1);
}
