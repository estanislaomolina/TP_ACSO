#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (pathname[0] != '/') return -1;

    int inum = 1;
    char pathcopy[1024];
    strncpy(pathcopy, pathname, sizeof(pathcopy));
    pathcopy[sizeof(pathcopy) - 1] = '\0';

    char *token = strtok(pathcopy, "/");
    while (token != NULL) {
        struct direntv6 dirEnt;
        if (directory_findname(fs, token, inum, &dirEnt) < 0) {
            return -1;
        }
        inum = dirEnt.d_inumber;
        token = strtok(NULL, "/");
    }

    return inum;
}
