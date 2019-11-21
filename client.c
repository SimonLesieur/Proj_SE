#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "utils.h"

#define TAILLE_MESSAGE	256
#define PATH	"/tmp/chat/"

int fd;
char myfifo[50];

void handle_sigint(int sig)
{
    printf("Caught signal %d\n", sig);
    close(fd);
    remove(myfifo);
    exit(0);
}

int main()
{
    int pid = getpid();
    char strPid[7] = {0};        // a mieux faire
    sprintf(strPid, "%d", pid);


    strcpy(myfifo, PATH);
    strcat(myfifo, strPid);
    printf("je suis le client : %s\n", myfifo);

    signal(SIGINT, handle_sigint);


    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    exit_if((mkfifo(myfifo, 0777) == -1), "mkfifo");
    printf("mkfifo %s\n", myfifo);


    int filDescServ = open("/tmp/chat/0", O_WRONLY);
    exit_if((filDescServ == -1), "open fileDescServ");

    exit_if((write(filDescServ, myfifo, strlen(myfifo)+1) == -1), "write");


    fd = open(myfifo, O_RDONLY);
    printf("open\n");
    exit_if((fd == -1), "open fd");

    char str_Recept[80], str_Envoi[80];

    int pid_t = fork();

    while (1)
      {
	switch(pid_t)
	  {
	  case -1:
	    perror("fork");
	    exit(EXIT_FAILURE);
	    break;

	  case 0:
	    printf("Ecris ton message : \n");
	    exit_if((fgets(str_Envoi, 100, stdin) == NULL),"fgets");
	    exit_if((write(filDescServ, str_Envoi, strlen(str_Envoi)+1) == -1), "write");
	    break;

	  default:

	    exit_if(((read(fd, str_Recept, sizeof(str_Recept)) == -1) && (errno != EAGAIN)),"read");
	  
	    printf("Le serveur a envoyé : %s\n", str_Recept);
	    break;
	  }
	str_Recept[0] = '\0';
      }
    return 0;
}


//Dans while 1
     //   // Open FIFO for write only
      //   fd = open(myfifo, O_WRONLY);
      //   exit_if((fd == -1),"open");
      //
      //
      //   // Take an input arr2ing from user.
      //   // 80 is maximum length
      //   exit_if((fgets(arr2, 80, stdin) == NULL),"fgets");
      //
      //   // Write the input arr2ing on FIFO
      //   // and close it
      // exit_if((write(fd, arr2, strlen(arr2)+1) == -1), "write");
      //
      //   exit_if((close(fd)==-1),"close");
      //
      //   // Open FIFO for Read only
      //   fd = open(myfifo, O_RDONLY);
      //   exit_if((fd == -1),"open");
      //
      //   // Read from FIFO
      //   exit_if((read(fd, arr1, sizeof(arr1)) == -1),"read");
      //
      //
      //   // Print the read message
      //   printf("User2: %s\n", arr1);
      //   exit_if((close(fd)==-1),"close");







// #include <stdio.h>
//
// #include <unistd.h>
// #include <fcntl.h>
// #include <errno.h>
//
// #include "utils.h"
//
// #define FIFO_PATH "/tmp/ex-fifo"
//
//
//
// int main(int argc, char *argv[])
// {
//     const char *fname = (argc < 2) ? FIFO_PATH : argv[1];
//     exit_if(mkfifo(fname, 0666) == -1 && errno != EEXIST, fname);
//
//     int fd = open(fname, O_WRONLY);
//     exit_if(fd == -1, fname);
//
//     printf("file %s is open for write\n", fname);
//     char buffer[100];
//     int n = snprintf(buffer, sizeof(buffer),
//             "I'm the child %d, of %d\n", getpid(), getppid());
//     write(fd, buffer, n);
//     printf("end of writer\n");
//     return 0;
// }
//
//
