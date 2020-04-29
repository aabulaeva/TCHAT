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

#include "msg_struct.h"
#include "info_client.h"

struct client * chererche_client_par_descipteur(int fd,struct client* fir){
    while (fir->next != NULL) {
        if (fir->sock==fd) {
            return fir;
        }
      fir=fir->next;
    }
    return NULL;
}


//Demander le username
int ask_username(int fd){
    char *asknick=malloc(MSG_LEN);
    strcpy(asknick,"please login with /nick <your pseudo>");
    send(fd,asknick,strlen(asknick),0);
    return 0;
}

//Save username
int set_username(int fd,struct client* fir,char username[NICK_LEN]){
    while (fir->next != NULL && fir->sock!=fd) {
      fir=fir->next;
      printf("1passage\n" );
    }
    strcpy(fir->username,username);
    return 0;
}

struct client * chain(int sock,struct sockaddr *adress,int port,struct client* fir){
  struct client* client=malloc(sizeof(struct client*));
  while (fir->next != NULL) {
    fir=fir->next;
    printf("1passage\n" );
  }
  fir->next=client;
  client->sock=sock;
  client->adress=adress;
  client->next_playload=0;
  client->port=port;
  strcpy(fir->username,"\0");
  client->next=NULL;
  return client;
}
