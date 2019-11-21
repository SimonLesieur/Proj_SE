#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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
  int fileDescClient[10]; //tableau de fileDescripteur des clients
  int connectedClient = 0;//Nb de client connecté


  //je mets PATH dans myfifo et on ajoute un 0
  strcpy(myfifo, PATH);
  strcat(myfifo, "0");

  //on creer le fichier pour la fifo ( avec test système ça serait mieux)
  mkdir(PATH, 0777);

  // Creating the named file(FIFO)
  // mkfifo(<pathname>,<permission>)

  signal(SIGINT, handle_sigint);

  //je crée la fifo et si erreur exit
  exit_if((mkfifo(myfifo, 0666) == -1), "mkfifo");

  //j'ouvre le fichier myfifo et si erreur exit
  fd = open(myfifo,O_RDONLY);
  exit_if((fd == -1), "open");




  char buffer[100];

  while(1)
    {
    

      exit_if((read(fd, buffer,100)==-1),"read");          //je lis et exit si erreur

      printf("serveur à reçu du client N : %s\n", buffer);   // Print the read string


      if (buffer[0] == '/')            //si /-> nouveau client
	{
	  fileDescClient[connectedClient] = open(buffer,O_WRONLY);

	  exit_if((fileDescClient[connectedClient] == -1), "open");

	  exit_if((write(fileDescClient[connectedClient], "Bienvenue", strlen("Bienvenue") + 1) == -1), "write");
	  connectedClient++;
	}
      else                               //sinon msg normal je renvoie a tous le monde
	{
	for (int i = 0; i<connectedClient; i++)
	  {
	    printf("buffer=%s\n", buffer);
	    exit_if((write(fileDescClient[i], buffer, strlen(buffer) + 1) == -1), "write");
	
	  }
	}

    }
  return 0;
}



//  dans le while 1
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
