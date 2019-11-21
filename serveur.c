#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "utils.h"

#define TAILLE_MESSAGE	256
#define PATH						"/tmp/chat/"

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
    // int fd;
		int fdClient[10];
		int connectedClient = 0;
    // FIFO file path
    // char * myfifo = PATH "0";
		strcpy(myfifo, PATH);
		strcat(myfifo, "0");

		mkdir(PATH, 0777);	// avec test système ça serait mieux

    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)

		signal(SIGINT, handle_sigint);

    exit_if((mkfifo(myfifo, 0666) == -1), "mkfifo");
		fd = open(myfifo,O_RDONLY);
		exit_if((fd == -1), "open");

    char str1[80], str2[80];
    while (1)
    {
        // // First open in read only and read
        // fd1 = open(myfifo,O_RDONLY);
				// exit_if((fd1 == -1), "open");
				//
        exit_if((read(fd, str1, 80)==-1),"read");

        // Print the read string and close
        printf("User1: %s\n", str1);

				if (str1[0] == '/')
				{
					fdClient[connectedClient] = open(str1,O_WRONLY);
					exit_if((fdClient[connectedClient] == -1), "open");
					exit_if((write(fdClient[connectedClient], "Bienvenue", strlen("Bienvenue") + 1) == -1), "write");
					connectedClient++;
				}
				else
					for (int i = 0; i<connectedClient; i++)
						exit_if((write(fdClient[i], str1, strlen(str1) + 1) == -1), "write");


			  // exit_if((close(fd1) == -1),"close");
				//
        // // Now open in write mode and write
        // // string taken from user.
        // fd1 = open(myfifo,O_WRONLY);
				// exit_if((fd1 == -1), "open");
				//
        // exit_if((fgets(str2, 80, stdin) == NULL),"fgets");
        // exit_if((write(fd1, str2, strlen(str2)+1) == -1),"write");
        // exit_if((close(fd1) == -1),"close");
    }
    return 0;
}




























// #include <fcntl.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
//
// #include "utils.h"
//
//
// #define TAILLE_MESSAGE	256
// #define PATH						"/tmp/chat/"
//
// int main(void)
// {
// 	int sortieTube;
//
// 	char nomTube[] = (PATH, "0");
//
// 	char chaineALire[TAILLE_MESSAGE];
//
// 	sortieTube = open (nomTube, O_RDONLY));
// 	exit_if((sortieTube == -1), "open");
//
//
// 	while (1) {
// 		read(sortieTube, chaineALire, TAILLE_MESSAGE);
//     printf("Serveur : %s", chaineALire);
// 	}
//
// 	return EXIT_SUCCESS;
// }
