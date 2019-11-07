#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "utils.h"

#define FIFO_PATH "/tmp/ex-fifo"

int main(int argc, char *argv[])
{
    const char *fname = (argc < 2) ? FIFO_PATH : argv[1];
    exit_if(mkfifo(fname, 0666) == -1 && errno != EEXIST, fname);

    int fd = open(fname, O_RDONLY);
    exit_if(fd == -1, fname);

    printf("file %s is opened for read\n", fname);
    int n;
    char buffer[100];
    while((n = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = 0;
        printf("reader: %s", buffer);
    }
    printf("end of reader\n");
    return 0;
}
