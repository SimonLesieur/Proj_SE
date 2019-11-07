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

    int fd = open(fname, O_WRONLY);
    exit_if(fd == -1, fname);

    printf("file %s is open for write\n", fname);
    char buffer[100];
    int n = snprintf(buffer, sizeof(buffer),
            "I'm the child %d, of %d\n", getpid(), getppid());
    write(fd, buffer, n);
    printf("end of writer\n");
    return 0;
}
