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

#define FDS_LEN 512

void msg_quitter(){
    printf("socket fermé message avec ./exit\n" );
    printf("************************************************\n" );
}


int analyse_msg(struct message * message, struct client *client,int fd){
    switch (message->type) {
        case NICKNAME_NEW:
        break;

        case QUIT:
            msg_quitter();
        return -1;

        case NICKNAME_LIST:
        break;

        case NICKNAME_INFOS:
        break;

        case ECHO_SEND:
        break;

        case UNICAST_SEND:
        break;

        case BROADCAST_SEND:
        break;

        case MULTICAST_CREATE:
        break;

        case MULTICAST_LIST:
        break;

        case MULTICAST_JOIN:
        break;

        case MULTICAST_SEND:
        break;

        case MULTICAST_QUIT:
        break;

        case FILE_REQUEST:
        break;

        case FILE_ACCEPT:
        break;

        case FILE_REJECT:
        break;

        case FILE_SEND:
        break;

        case FILE_ACK:
        break;
    }
    return 0;
}


// //Fonction pour le nettoyage MESSAGE char
// char * cleansocket(char * msg){
//   char *esp="\n";
//   //int size_msg=1024;
//   char *rep=malloc(MSG_LEN);
//
//   memset(rep,0,MSG_LEN);
//   int i;
//   for (i = 0; i < strlen(msg); i++) {
//     if (msg[i]==esp[0]) {
//       return rep;
//       }
//     rep[i]=msg[i];
//     }
//   return rep;
// }

//Fonction pour la communication
void multicast(int sock,int port){

    //Variables

    int recvresult;
    int operation=0;
    struct client* client_courant;
    //int size_msg=1024;

    int compteur=1;//compte la derniere case du tableau fds
    struct pollfd fds[FDS_LEN];
    fds[0].fd=sock;
    fds[0].events=POLLIN;

    struct sockaddr_in addr_client;
    int len= sizeof(addr_client);

    struct client *clients=malloc(sizeof(struct client));
    clients->sock=sock;
    clients->adress=NULL;
    clients->port=port;
    clients->next=NULL;

    int sock2;

    struct message *message_struct=malloc(sizeof(struct message));
    struct message *message_char=malloc(MSG_LEN*sizeof(char));

    while (1) {
    poll(fds,FDS_LEN,-1);//on attend de recevoir un client
    int i;

    for (i = 0; i < FDS_LEN; i++) {

        if(fds[i].revents==POLLIN){//info recu,initialement revents=0

            if (fds[i].fd==sock) {//socket du serveur,socket d ecoute donc on a un nouveau client
                if((sock2=accept(sock,(struct sockaddr *)&addr_client,(socklen_t *)&len)) == -1){
                    perror("accept error");
                }
                else{
                  askusername(fds[i].fd);
                }
                fds[compteur].fd=sock2;
                fds[compteur].events=POLLIN;
                compteur++;
                chain(sock2,(struct sockaddr *)&addr_client,port,clients);
            }

            else{
                memset(&message_struct, 0, sizeof(message_struct));
                if (recv(fds[i].fd,&message_struct, sizeof(struct message),0)>0) {
                    client_courant=chererche_client_par_descipteur(fds[i].fd,clients);
                    operation=analyse_msg(message_struct,clients,fds[i].fd);

                    if (operation==-1) {
                        close(fds[i].fd); //ferme la socket
                        fds[i].fd = -1; //champs descripteur  associe a la socket ferme avec pollin revents
                        erase_client(fd);
                    }
                    if (operation>0) {
                        client_courant.playload=operation;
                    }

                    printf("le client à envoyé une struct message\n",recvresult );
                }

                memset(&message_char, 0, MSG_LEN);
                if (recv(fds[i].fd,&message_char, playload*sizeof(char),0)>0) {


                    printf("\n",recvresult );
                }
            }

        }
    }
    }
    free(message_struct);
}

//Fonction pour la connection
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
