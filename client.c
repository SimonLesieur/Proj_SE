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




//---------------------ID CLIENT-----------------------
    char nameClient[15];
    printf("Entrer votre nom d'utilisateur:");
    exit_if((fgets(nameClient, 15, stdin) == NULL),"fgets");

    char idClient[30]= "/i";
    strcat(idClient,";");
    strcat(idClient,myfifo);
    strcat(idClient,";");
    strcat(idClient,nameClient);
    exit_if((write(filDescServ, idClient, strlen(idClient)+1) == -1), "write");   //envoit l'identité
//-------------------ID CLIENT-----------------------

   fd = open(myfifo, O_RDONLY);
    exit_if((fd == -1), "open fd");


    char str_Recept[80], str_Envoi[80];

   char buffOfText[80];
    char buff2Send[100];


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
	    exit_if((fgets(buffOfText, 80, stdin) == NULL),"fgets");
 
	    strcpy(buff2Send,"/m;");
	    strcat(buff2Send,myfifo);
	    strcat(buff2Send,";");
	    strcat(buff2Send,buffOfText);

	    exit_if((write(filDescServ, buff2Send, strlen(buff2Send)+1) == -1), "write");
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
