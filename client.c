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

int  fileDescServ;
int  fileDescClie;
char pathClientFile[12]  = "/tmp/chat/";
char pathClientPipe[30];
char pathServerPipe[12] = "/tmp/chat/0";


void handle_sigint(int sig) {
  if(sig == 2) {
    printf("OK, merci et au revoir!\n");
    char buff2quit[30];
    strcpy(buff2quit,"/q;");
    strcat(buff2quit,pathClientPipe);
    exit_if((write(fileDescServ, buff2quit, strlen(buff2quit)+1) == -1), "write");
  }
  else {
  printf("Vous etes déconnecte(e) !\n");
  printf("Caught signal %d\n", sig);
  close(fileDescClie);
  remove( pathClientFile );
  exit(EXIT_SUCCESS);
  }
}


int main() {
  signal(SIGINT, handle_sigint);

  int pid = getpid();
  char strPid[7] = {0};
  sprintf(strPid, "%d", pid);

  strcpy(pathClientPipe, pathClientFile);
  strcat(pathClientPipe, strPid);

//-------------------------LANCEMENT DU SERVEUR--------------------------
  fileDescServ = open(pathServerPipe, O_WRONLY);
  if (fileDescServ < 0)
  {
    if (fork() == 0) {
        int fd_null = open("/dev/null", O_RDONLY);
        exit_if(fd_null == -1," open");
        dup2(fd_null, STDIN_FILENO);
        dup2(fd_null, STDOUT_FILENO);
        dup2(fd_null, STDERR_FILENO);
        exit_if(execl("./serveur","serveur", (char*)NULL) !=0, "ERROR Client could not open server");}
    printf("Le serveur est deja lance\n");

    while ((fileDescServ = open(pathServerPipe, O_WRONLY | O_NONBLOCK)) == -1){
            if (errno == ENOENT || errno == ENXIO){}
            else{ exit_if(fileDescServ < 0, "ERROR launching server"); }
            usleep(10);}
  }




  //--------------------LANCEMENT PIPE ET INTITULE----------------------
  exit_if((mkfifo(pathClientPipe, 0777) == -1), "mkfifo");
  printf("Bienvenue sur ce tchat\n");
  printf("Les commandes disponibles sont:\n");
  printf("/w pour savoir qui est connecte\n");
  printf("/q pour se deconnecter\n");



  //------------------------------ID CLIENT-----------------------------
  char nameClient[16];
  printf("Entrer votre nom d'utilisateur:");
  exit_if((fgets(nameClient, 16, stdin) == NULL),"fgets");
  char idClient[40]= "/i";
  strcat(idClient,";");
  strcat(idClient,pathClientPipe);
  strcat(idClient,";");
  strcat(idClient,nameClient);
  exit_if((write(fileDescServ, idClient, strlen(idClient)+1) == -1), "write");   //envoit l'identité

  //------------------------------OPEN SERV----------------------------
   fileDescClie = open(pathClientPipe, O_RDONLY);
   exit_if((fileDescClie == -1), "open fd");



  char buffOfText[80];
  char buff2Send[100];
  char buffRecv[100];

  int pid_t = fork();

  while (1)
  {
    for(int i =0 ; i<100; i++) { buff2Send[i]=0;  buffRecv[i]=0;}      //RESET BUFFER
    switch(pid_t)
    {
      case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        break;

      case 0:
        exit_if((fgets(buffOfText, 80, stdin) == NULL),"fgets");
        if(buffOfText[0] == '/')    //CMD DETECTPR
        {
          //CMD REPERTORY
          if(buffOfText[1] == 'w')
          {
            printf("les utilisateurs sont :\n");
            strcpy(buff2Send,"/w;");
            strcat(buff2Send,pathClientPipe);
          }

          //CMD QUIT
          else if(buffOfText[1] == 'q') { handle_sigint(SIGINT);}

          //CMD UNKNOW
          else {printf("commande inconnue\n");}
        }
        else                        //MESSAGE STANDARD
        {
          strcpy(buff2Send,"/m;");
          strcat(buff2Send,pathClientPipe);
          strcat(buff2Send,";");
          strcat(buff2Send,buffOfText);
        }
        exit_if((write(fileDescServ, buff2Send, strlen(buff2Send)+1) == -1), "write");
        break;

      default:
        exit_if(((read(fileDescClie, buffRecv, sizeof(buffRecv)) == -1) && (errno != EAGAIN)),"read");
        if(strcmp(buffRecv,"/c")==0)
          handle_sigint(SIGTERM);
        else
          printf(">>>%s", buffRecv);
        break;
    }
  }
  return 0;
}
