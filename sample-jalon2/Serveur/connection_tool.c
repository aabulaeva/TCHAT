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
#include <time.h>
#include <assert.h>


#include "msg_struct.h"
#include "info_client.h"

#define FDS_LEN 512

void msg_quitter(char* username){
    printf("%s : socket fermé message avec ./exit\n",username);
    printf("************************************************\n" );
}

int transmission_char(struct client *clients, struct client *client_courant,int fd){
    char * buffer_char=malloc(MSG_LEN);
    memset(buffer_char,0,MSG_LEN);
    int s;
    if (recv(fd,buffer_char, get_playload_client(client_courant)*sizeof(char),0)>0) {
        printf("oui\n");
        printf("message : %s\n",buffer_char );
        s=send_to_dest(client_courant,buffer_char,clients);
        if(s==-1){
            printf("pas envoyé\n" );
            send_client_error(fd,SEND_IMPOSSIBLE);
            return -1;
        }
        else{
            printf("envoyer avec succes\n");
            printf("%i octet\n",s);

            return 0;
        }
      }
      return -1;

}


int analyse_msg(struct message * message, struct client *clients, struct client *client_courant,int fd){
    printf("type = %d\n", message_get_type(message));
    printf("username = %s\n", message_get_nick_sender(message));
    printf("playload = %d\n", message_get_playload(message));
    printf("infos = %s\n", message_get_infos(message));
    printf("%s infos \n", message->infos);
    switch (message_get_type(message)) {
        case NICKNAME_NEW:
            if(check_name_available(message_get_nick_sender(message),clients)==0){
                client_courant=set_username(client_courant,message_get_nick_sender(message));
                send_user_name_ack(client_courant,fd);
                return 0;
             }
            else{
               ask_username(fd);
            }
        break;
        case NICKNAME_LIST:
            send_list_online_user(clients,fd);
        break;
        case NICKNAME_INFOS:
            send_user_info(clients,fd,message_get_infos(message));
        break;

        case ECHO_SEND:
            printf("echo send\n");
            save_infos(client_courant,message);
            transmission_char(clients,client_courant,fd);

        break;

        case UNICAST_SEND:
            save_infos(client_courant,message);
            transmission_char(clients,client_courant,fd);
        break;

        case BROADCAST_SEND:
            save_infos(client_courant,message);
            printf("le type %i\n", message_get_type(message));
            transmission_char(clients,client_courant,fd);
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

        case QUIT:
        msg_quitter(get_username(client_courant));
        return -1;

        default:
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

    struct client* client_courant=malloc(sizeof(struct client));
    struct message message_struct;
    char *message_char=malloc(MSG_LEN*sizeof(char));
    char * buff=malloc(MSG_LEN);
    memset(&message_struct, 0, sizeof(message_struct));
    memset(buff,0,MSG_LEN);
    //int size_msg=1024;

    int compteur=1;//compte la derniere case du tableau fds
    struct pollfd fds[FDS_LEN];
    fds[0].fd=sock;
    fds[0].events=POLLIN;

    struct sockaddr_in addr_client;
    int len= sizeof(addr_client);

    struct client *clients=malloc(sizeof(struct client));
    clients->sock=sock;
    clients->username="serveur";
    clients->adress=NULL;
    clients->port=port;
    clients->playload=10*sizeof(char);
    clients->next=NULL;

    int sock2;


    while (1) {
    if (poll(fds,FDS_LEN,-1)==-1) {
      perror("poll :");
    }
    int i;

    for (i = 0; i < FDS_LEN; i++) {


        if(fds[i].revents==POLLIN){//info recu,initialement revents=0
            printf("ACTION\n" );
            if(fds[i].fd==0){
                read(0,buff,(INFOS_LEN+7));
                printf("%s\n",buff );
            }
            if (fds[i].fd==sock) {//socket du serveur,socket d ecoute donc on a un nouveau client
                printf("Nouveau client\n");
                if((sock2=accept(sock,(struct sockaddr *)&addr_client,(socklen_t *)&len)) == -1){
                    perror("accept error");
                }
                else{
                  printf("accept avec sucess\n");
                  //ask_username(fds[i].fd);
                }
                fds[compteur].fd=sock2;
                fds[compteur].events=POLLIN;
                compteur++;
                add_to_clients(sock2,(struct sockaddr *)&addr_client,port,clients);
            }
            else{
                client_courant=chererche_client_par_descipteur(fds[i].fd,clients);
                if (recv(fds[i].fd,&message_struct, sizeof(struct message),0)>0) {
                    printf("un message a été reçu\n");
                    if (analyse_msg(&message_struct,clients,client_courant,fds[i].fd)==-1) {
                        erase_client(fds[i].fd,clients);
                        close(fds[i].fd); //ferme la socket
                        fds[i].fd = -1; //champs descripteur  associe a la socket ferme avec pollin revents
                    }
                    printf("le client à envoyé une struct message et à été pris en compte avec succès\n");
                    memset(&message_struct, 0, sizeof(message_struct));
                }
                else if (recv(fds[i].fd,&message_char, sizeof(char),0)==0) {
                    printf("on va erase un clients\n");
                    erase_client(fds[i].fd,clients);
                    close(fds[i].fd);
                }
            }

        }
    }
    }
    //free(message_struct);
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
  printf("Socket crée avec succès \n");
  return sock;
}
