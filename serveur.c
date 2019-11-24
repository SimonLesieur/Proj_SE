#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "utils.h"

#define NB_MAX_CLIENT 10


int  fileDescServ;
char pathServerFile[20] = "/tmp/chat/";
char pathServerPipe[12] = "/tmp/chat/0";


struct clientId{
  char name[20];
  char file[20];
  int pid;
};

struct clientId clientTab[NB_MAX_CLIENT]; //tableau de fileDescripteur des clients
int nbOfClient = 0;                       //Nb de client connecté



//-------gestion des CTRLC et fermeture client-------
void handle_sigint(int sig)
{
  printf("Caught signal %d\n", sig);
  for(int i = 0 ; i<nbOfClient ; i++){
    printf("je tue :%s\ns",clientTab[i].name);
    exit_if((write(clientTab[i].pid, "/c", strlen("/c")+1) == -1),"write");}
  close(fileDescServ);
  system( "rm -rf /tmp/chat/" );
  exit(0);
}

//---------Identification du client émetteur et retour de son numéro id----------
int clientIdentifier(char dataR1[100],struct clientId clientTab[NB_MAX_CLIENT],int nbOfClient)
{
  for(int i = 0 ; i<nbOfClient ; i++){
    if( strcmp(clientTab[i].file, dataR1) == 0)
      return i;}
}



int main()
{
  system( "rm -rf /tmp/chat/" );
  signal(SIGINT, handle_sigint);

  mkdir(pathServerFile, 0777);                              //on crée le fichier pour la fifo
  exit_if((mkfifo(pathServerPipe, 0666) == -1), "mkfifo");  //on crée le pipe serv
  fileDescServ = open(pathServerPipe,O_RDONLY);             //on ouvre le pipe serv
  exit_if((fileDescServ == -1), "open");

  printf("j'ai bien démarré\n");

  int  currentClientId;
  char dataR1[100];
  char dataR2[100];
  char bufferR[100];
  char bufferS[100];

  while(1)
  {
    for(int i =0 ; i<100; i++) { bufferR[i]=0;  bufferS[i]=0;}  //RESET BUFFER

    exit_if((read(fileDescServ, bufferR,100)==-1),"read");      //on lit et exit si erreur
    printf("\n~~~~~~RECV CLIENT: %s", bufferR);                 //Print the read string

    if (bufferR[0] == '/')                                      // si "/"" -> consigne
    {
      printf("++++++CONS ");

      //-------------ISOLATION DES DIFFERENTS PARAMETRES-------------
      int j = 0;
      int k = 0;
      for(int i = 3; i<100; i++){
        if(bufferR[i] == ';'){ j=1; k=0;}
        else{
          if(j==0) dataR1[k] = bufferR[i];
          else     dataR2[k] = bufferR[i];
          k++;}}
      //-------------ISOLATION DES DIFFERENTS PARAMETRES-------------

      currentClientId = clientIdentifier(dataR1,clientTab,nbOfClient);

      //-------------CONSIGNE IDENTIFICATION------------------
      if(bufferR[1]=='i')
      {
        if(NB_MAX_CLIENT > nbOfClient){
        printf("IDENTIFIER++++++\n");

        strcpy(clientTab[nbOfClient].file,dataR1);
        strcpy(clientTab[nbOfClient].name,dataR2);
        size_t ln = strlen(clientTab[nbOfClient].name) - 1;
        if (clientTab[nbOfClient].name[ln] == '\n')  clientTab[nbOfClient].name[ln] = '\0';


        clientTab[nbOfClient].pid = open(clientTab[nbOfClient].file,O_WRONLY);
        exit_if((clientTab[nbOfClient].pid == -1), "open");

        strcpy(bufferS,"bienvenue ");
        strcat(bufferS,clientTab[nbOfClient].name);
        strcat(bufferS,"\n");
        exit_if((write(clientTab[nbOfClient].pid, bufferS, strlen(bufferS) + 1) == -1), "write");
        nbOfClient++;}
        else{
        strcpy(bufferS,"Déja trop de client essayez plus tard...\n");
        exit_if((write(clientTab[currentClientId].pid, bufferS, strlen(bufferS) + 1) == -1), "write");}
      }

      //-------------CONSIGNE REPERTOIRE------------------
      else if(bufferR[1]=='w')
      {
        printf("REPERTORY++++++\n");
        if(nbOfClient == 1){
          strcpy(bufferS,"vous êtes seul sur ce tchat\n");
          printf("vous êtes seul sur ce tchat\n");
          exit_if((write(clientTab[currentClientId].pid, bufferS, strlen(bufferS) + 1) == -1), "write");}
        else{
          for(int i = 0 ; i<nbOfClient ; i++){
            if( currentClientId != i){
              printf("Il y a %s\n",clientTab[i].name);
              strcpy(bufferS,"-");
              strcat(bufferS,clientTab[i].name);
              strcat(bufferS,"\n");
              sleep(1);
              exit_if((write(clientTab[currentClientId].pid, bufferS, strlen(bufferS)+1) == -1),"write");}}}
      }

      //-------------CONSIGNE MESSAGE------------------
      else if(bufferR[1]=='m')
      {
        printf("MESSAGE++++++\n");
        strcat(bufferS,clientTab[currentClientId].name);
        strcat(bufferS,": ");
        strcat(bufferS,dataR2);
        for(int i = 0 ; i<nbOfClient ; i++){
          if( currentClientId != i){
            printf("j'envoie à %s\n",clientTab[i].name);
            exit_if((write(clientTab[i].pid, bufferS, strlen(bufferS)+1) == -1),"write");}}
      }

      //-------------CONSIGNE QUIT------------------
      else if(bufferR[1] == 'q')
      {
        exit_if((write(clientTab[currentClientId].pid, "/c", strlen("/c")+1) == -1),"write");
        printf("CLOSING CLIENT++++++\n");
        for(int i = currentClientId;i<(nbOfClient-1);i++){
          clientTab[i]=clientTab[i+1];}
        nbOfClient--;
      }

      //-------------CONSIGNE ERREUR------------------
      else{ printf("++++++ERROR UNKNOW CONS+++++\n"); }

    }
    else
      printf("++++++ERROR UNKNOW RECV+++++\n");
  }
  return 0;
}
