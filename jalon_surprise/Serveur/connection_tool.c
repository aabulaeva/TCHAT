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
#include "gestionsalon.h"

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
        s=send_to_dest(client_courant,buffer_char,clients);
        if(s==-1){
            send_client_error(fd,SEND_IMPOSSIBLE);
            return -1;
        }
        else{
            return 0;
        }
      }
      return -1;

}


int analyse_msg(struct message * message, struct client *clients,struct salon* salons, struct client *client_courant,int fd){
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
            save_infos(client_courant,message);
            transmission_char(clients,client_courant,fd);

        break;

        case UNICAST_SEND:
            save_infos(client_courant,message);
            transmission_char(clients,client_courant,fd);
        break;

        case BROADCAST_SEND:
            save_infos(client_courant,message);
            transmission_char(clients,client_courant,fd);
        break;

        case MULTICAST_CREATE:
            if (check_name_salon_available(message_get_infos(message),salons)==0) {
                struct salon* saloncreat=creatsalon(client_courant,message,salons);
                set_salon_name(saloncreat,message_get_infos(message));
                client_set_salon(client_courant,message_get_infos(message));
                set_user_salon(client_courant,message_get_infos(message));
                send_salon_name_ack(client_courant,fd);
            }
            else{
                send_salon_name_wrong(fd);
            }
        break;

        case MULTICAST_LIST:
            send_list_salon(salons,fd);
        break;

        case MULTICAST_JOIN:
            if (strcmp(message_get_infos(message),"aucun")!=0 && joinsalon(client_courant,message,salons)) {
                send_add_salon_ack(fd,message_get_infos(message));
            }
            else{
                send_salon_wrong(fd);
            }
        break;

        case MULTICAST_SEND:
            save_infos(client_courant,message);
            transmission_char(clients,client_courant,fd);
        break;
        case MULTICAST_QUIT:

        break;
        case SALON_EMPTY:
            salon_empty(salons,client_courant);
        break;
        case FILE_REQUEST:
            save_infos(client_courant,message);
            transmission_char(clients,client_courant,fd);
        break;
        case FILE_ACCEPT:
            save_infos(client_courant,message);
            transmission_msg_struct(clients,client_courant);
        break;
        case FILE_REJECT:
            save_infos(client_courant,message);
            transmission_msg_struct(clients,client_courant);
        break;
        case FILE_SEND:
            save_infos(client_courant,message);
            transmission_msg_struct(clients,client_courant);
        break;
        case FILE_ACK:
            save_infos(client_courant,message);
            transmission_msg_struct(clients,client_courant);
        break;
        case QUIT:
        msg_quitter(get_username(client_courant));
        return -1;

        default:
        break;
    }

    return 0;
}


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
    fds[1].fd = 0;
    fds[1].events=POLLIN;

    struct sockaddr_in addr_client;
    int len= sizeof(addr_client);

    struct client *clients=malloc(sizeof(struct client));
    clients->sock=sock;
    clients->username="serveur";
    clients->adress=NULL;
    clients->port=port;
    clients->playload=10*sizeof(char);
    clients->next=NULL;

    struct salon *salons=malloc(sizeof(struct salon));
    salons->salonname="aucun";
    salons->nb_part=0;
    salons->next=NULL;

    int sock2;


    while (1) {
    if (poll(fds,FDS_LEN,-1)==-1) {
      perror("poll :");
    }
    int i;

    for (i = 0; i < FDS_LEN; i++) {


        if(fds[i].revents==POLLIN){//info recu,initialement revents=0
            if (fds[i].fd==sock) {//socket du serveur,socket d ecoute donc on a un nouveau client
                if((sock2=accept(sock,(struct sockaddr *)&addr_client,(socklen_t *)&len)) == -1){
                    perror("accept error");
                }
                else{
                  printf("Nouveau client\n");
                }
                fds[compteur].fd=sock2;
                fds[compteur].events=POLLIN;
                compteur++;
                add_to_clients(sock2,(struct sockaddr *)&addr_client,port,clients);
            }
            // else if(fds[i].fd==0){
            //     printf("***Envoie à tous les clients***\n");
            //     read(0,buff,(INFOS_LEN+7));
            //     msg_all_serv(buff,clients);
            //     memset(buff,0,MSG_LEN);
            // }
            else{
                client_courant=chererche_client_par_descipteur(fds[i].fd,clients);
                if (recv(fds[i].fd,&message_struct, sizeof(struct message),0)>0) {
                    if (analyse_msg(&message_struct,clients,salons,client_courant,fds[i].fd)==-1) {
                        erase_client(fds[i].fd,clients);
                        close(fds[i].fd); //ferme la socket
                        fds[i].fd = -1; //champs descripteur  associe a la socket ferme avec pollin revents
                    }
                    memset(&message_struct, 0, sizeof(message_struct));
                }
                else if (recv(fds[i].fd,&message_char, sizeof(char),0)==0) {
                    erase_client(fds[i].fd,clients);
                    close(fds[i].fd);
                }
            }

        }
    }
    }
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
