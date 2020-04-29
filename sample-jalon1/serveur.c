#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

struct client{
  int sock;
  struct sockaddr *adress;
  int port;
  struct client* next;
};



struct client * chain(int sock,struct sockaddr *adress,int port,struct client* fir){
  struct client* client=malloc(sizeof(struct client*));
  while (fir->next != NULL) {
    fir=fir->next;
    printf("1passage\n" );
  }
  fir->next=client;
  client->sock=sock;
  client->adress=adress;
  client->port=port;
  client->next=NULL;
  return client;
}
int fin(char * chaine){
  char *quit="./quit";
  char *esp=" ";
  int compteur=0;

  for (size_t i = 0; i < strlen(chaine); i++) {
    if (chaine[i]==quit[i] && i<strlen(quit))
      compteur ++;
    else if (i>=strlen(quit) && chaine[i]==esp[0] && (compteur+2>strlen(quit))){
      return 0;
    }
    else if (i>strlen(quit) && chaine[i]!=esp[0] )
      return 1;
    }
  if (compteur+2==strlen(quit)) {
    return 0;
    }

  return 1;
}

int handle_bind(char *port){
  int pt=atoi(port);
  struct sockaddr_in saddr; // sockaddr_in avec INADDRANY pour l adresse
  saddr.sin_family = AF_INET;
  saddr.sin_port= htons(pt);
  saddr.sin_addr.s_addr=INADDR_ANY;
  int sock;

  //1 creer une socket  avec socket()

  if ((sock=socket(AF_INET,SOCK_STREAM,0))==-1) {
    perror("erreur a la creation de la socket\n");
    return 1;
  }

  //2 bind()
  if (bind(sock,(struct sockaddr *)&saddr,sizeof(saddr)) == -1){
    perror("bind()");
    return 1;
  }
  return sock;
}
char * cleansocket(char * msg){
  char *esp="\n";
  int size_msg=1024;
  char *rep=malloc(size_msg);

  memset(rep,0,size_msg);
  for (int i = 0; i < strlen(msg); i++) {
    if (msg[i]==esp[0]) {
      return rep;
      }
    rep[i]=msg[i];
    }
  return rep;
}

void multicast(int sock,int port){
  int taillefds=512;
  int size_msg=1024;

  int compteur=1;//compte la derniere case du tableau fds
  struct pollfd fds[taillefds];
  fds[0].fd=sock;
  fds[0].events=POLLIN;

  struct sockaddr_in addr_client;
  int len= sizeof(addr_client);

  struct client *res=malloc(sizeof(struct client));
  res->sock=sock;
  res->adress=NULL;
  res->port=port;
  res->next=NULL;

  int sock2;

  char *received_msg=malloc(size_msg);



  while (1) {
    poll(fds,taillefds,-1);//on attend de recevoir un client
    for (size_t i = 0; i < taillefds; i++) {

      if(fds[i].revents==POLLIN){//info recu,initialement revents=0
        if (fds[i].fd==sock) {//socket du serveur,socket d ecoute donc on a un nouveau client
          if((sock2=accept(sock,(struct sockaddr *)&addr_client,(socklen_t *)&len)) == -1)
            perror("accept error");

          fds[compteur].fd=sock2;
          fds[compteur].events=POLLIN;
          compteur++;
          chain(sock2,(struct sockaddr *)&addr_client,port,res);
          }
        else{
          memset(received_msg, 0, size_msg);
          int recvresult = recv(fds[i].fd,received_msg,size_msg,0);
          received_msg=cleansocket(received_msg);
          printf("message recu%s\n",received_msg );
          int quit=fin(received_msg);
          printf("%i\n",recvresult );
          if ((recvresult>0 && quit==0) || recvresult==0) {
            printf("socket fermé message avec ./exit\n" );
            printf("************************************************\n" );
            close(fds[i].fd); //ferme la socket
            fds[i].fd = -1; //champs descripteur  associe a la socket ferme avec pollin revents
            }
          else{

            received_msg=cleansocket(received_msg);
            printf("message de :%s\n",inet_ntoa(addr_client.sin_addr));
            int resultsendto =send(fds[i].fd,received_msg,strlen(received_msg),0);
            printf("%s\n", received_msg);
            printf("donnees envoyees en retour: %d\n",resultsendto);
            }
          }
        }
      }
    }
    free(received_msg);
}


int main(int argc,char **argv){

    /* Check program parameters */
    if(argc != 2){
        printf("Argument Invalide ! \nAgument à fournir : ./programe_name #PORT\n");
        exit(EXIT_FAILURE);
    }
  char *port=argv[1];

  //socket bind
  int sock=handle_bind(port);

  //listen
  if (listen(sock,SOMAXCONN)==-1) {
    perror("ecoute de la socket\n");
    return 1;
  }
  //poll
  multicast(sock,atoi(port));

  //close

  close(sock);


  return 0;
}
