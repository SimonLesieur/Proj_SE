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

struct clientId{
  char name[20];
  char file[20];
  int pid;
};

void handle_sigint(int sig)
{
  printf("Caught signal %d\n", sig);
  close(fd);
  remove(myfifo);
  exit(0);
}







int main()
{
  struct clientId clientTab[10];           //tableau de fileDescripteur des clients
  int nbOfClient = 0;//Nb de client connecté


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

  char dataR1[100];
  char dataR2[100];
  char bufferR[100];
  char bufferS[100];

  while(1)
    {
      for(int i =0 ; i<100; i++){
	bufferR[i]=0;
	bufferS[i]=0;}

      exit_if((read(fd, bufferR,100)==-1),"read");          //je lis et exit si erreur

      printf("-------------------------------------\nSercRECEP :%s-------------------------------------------\n", bufferR);   // Print the read string

      
      if (bufferR[0] == '/')            //si / -> consigneeeeee
	{
	  printf("je reçois une consigne  ");

	  if(nbOfClient == 9)
	    printf("ERREUR trop de client");

	  int j = 0;
	  int k = 0;
	  for(int i = 3; i<100; i++){ 
	    if(bufferR[i] == ';'){
	      j=1;
	      k=0;}
	    else{
	      if(j==0)
		dataR1[k] = bufferR[i];
	      else
		dataR2[k] = bufferR[i];	
	      k++;}
	  }



	  if(bufferR[1]=='i')          //si i -> identité
	    {
	      printf("d'identification\r\n");

	      strcpy(clientTab[nbOfClient].file,dataR1);
	      strcpy(clientTab[nbOfClient].name,dataR2);


	      printf("le fd est %s et le nom est %s \n",clientTab[nbOfClient].file,clientTab[nbOfClient].name);

	      clientTab[nbOfClient].pid = open(clientTab[nbOfClient].file,O_WRONLY);
	      exit_if((clientTab[nbOfClient].pid == -1), "open");
	      
	      strcpy(bufferS,"bienvenue ");
	      strcat(bufferS,clientTab[nbOfClient].name);

	      exit_if((write(clientTab[nbOfClient].pid, bufferS, strlen(bufferS) + 1) == -1), "write");
	      nbOfClient++;
	    }


	  else if(bufferR[1]=='w')
	    {
	      printf("de répertoire\r\n");
	    }




	  else if(bufferR[1]=='m')
	    {
	      printf("de message\r\n");


	      strcpy(bufferS,dataR2);

	      for(int i = 0 ; i<nbOfClient ; i++){
		if( strcmp(clientTab[i].file, dataR1) != 0)
		  {
		    printf("j'envoie à %s",clientTab[i].name);
		    exit_if((write(clientTab[i].pid, bufferS, strlen(bufferS)+1) == -1),"write");
		  }
	      }


	    }



	  else if(bufferR[1] == 'q')
	    {
	      printf("de fermeture client\r\n");
	    }



	  else{
	    printf("erreur consigne non standarde");}

	}

      else
	    printf("erreur reception non standarde");
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
