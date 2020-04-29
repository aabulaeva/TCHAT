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
#include <string.h>
#include <assert.h>
#include "msg_struct.h"
#include "info_client.h"

void set_playload_client(struct client* client,int playload){
    assert(client);
    client->playload=playload;
}

void set_infos_client(struct client* client,char * infos){
    assert(client);
    client->infos=infos;
}

void set_msgtype_client(struct client* client,enum msg_type type){
    assert(client);
    client->type=type;
}

enum msg_type get_msgtype_client(struct client* client){
    assert(client);
    return client->type;
}

char* get_infos_client(struct client* client){
    assert(client);
    return client->infos;
}

int get_timer_client(struct client* user_cible){
   int time_connect=user_cible->timeconnect;
   int now=time(NULL);
   return (int)(now-time_connect);
}

int get_port_client(struct client* client){
    assert(client);
    return client->port;
}

 char *get_adress_client(struct client* client){
    assert(client);
    struct sockaddr_in *addr_in = (struct sockaddr_in *)client->adress;
    char *ip = inet_ntoa(addr_in->sin_addr);
    return ip;
}

int get_playload_client(struct client* client){
    assert(client);
    return client->playload;
}

int get_fd(struct client* client){
    assert(client);
    return client->sock;
}

char* get_username(struct client* client){
    assert(client);
    return client->username;
}

int check_name_available(char *username,struct client* fir){
    assert(fir);
    printf("%s\n", username );

    if (username==NULL) {
        return 0;
    }
    else{
        struct client* pt_client=fir;
        while (pt_client->next != NULL) {
          printf("on compare avec : %s\n",get_username(pt_client) );
            if (strcmp(username,get_username(pt_client))==0) {
              printf("le user name est invalide");
                return -1;
            }
            pt_client=pt_client->next;
        }
    }
    return 0;
}

struct client * chererche_client_par_descipteur(int fd,struct client* fir){
    assert(fir);
    if (fir->sock==fd) {
        return fir;
    }

    while (fir->next != NULL) {
        fir=fir->next;
        if (fir->sock==fd) {
            return fir;
        }
    }

    return NULL;
}

struct client * chererche_client_par_pseudo(char* pseudo,struct client* fir){
    assert(fir);
    if (strcmp(fir->username,pseudo)==0) {
        printf("touvé !\n");
        return fir;
    }

    while (fir->next != NULL) {
        fir=fir->next;
        if (strcmp(fir->username,pseudo)==0) {
            printf("trouvé !\n");
            return fir;
        }
    }

    return NULL;
}

void erase_client(int fd,struct client* fir){
    struct client* curs= fir->next;
    struct client* pres= fir;

    while (pres->next != NULL) {

      printf("%s\n",get_username(pres) );
        if (curs->sock==fd) {
          printf("le clients est : %s \n",get_username(curs) );
          if (curs->next!=NULL) {
            printf("au revoir : %s\n",get_username(curs) );
            pres->next=curs->next;
            break;
          }
          else{
            printf("au revoir : %s\n",get_username(curs) );
            printf("%s suprime:\n",get_username(pres->next) );

            pres->next=NULL;
            break;
          }
        }

        pres=pres->next;
        curs=curs->next;

    }
}


//Demander le username
int ask_username(int fd){

    char info[INFOS_LEN];
    char *buf="rien";
    strcpy(info,buf);
    struct message* message=malloc(sizeof(struct message));
    message= init_message_serveur(NICKNAME_WRONG,info,0);
    printf("un message nickname wrong have been send : %s\n                         %s \n",message_get_nick_sender(message),message_get_infos(message));
    if (send(fd,message,sizeof(struct message),0)==-1) {
        return -1;
    }
    free(message);
    return 0;
}

int send_list_online_user(struct client * clients,int fd){
    assert(clients);
    struct message* message_envoye=malloc(sizeof(struct message));
    char laliste[MSG_LEN];
    char tmp[50];
    strcpy(laliste,"la liste des utilisateur :");

    strcpy(tmp,"\n   - ");
    strcat(laliste,tmp);
    strcpy(tmp,get_username(clients));
    strcat(laliste,tmp);

    strcpy(tmp,"\n");
    strcat(laliste,tmp);

    while (clients->next != NULL) {
        clients=clients->next;
        strcpy(tmp,"   - ");
        strcat(laliste,tmp);

        strcpy(tmp,get_username(clients));
        strcat(laliste,tmp);

        strcpy(tmp,"\n");
        strcat(laliste,tmp);

    }
    message_envoye= init_message_serveur(INFO_SERVEUR,"rien",strlen(laliste));
    if (send(fd,message_envoye,sizeof(struct message),0)==-1) {
        return -1;
    }

    if (send(fd,laliste,strlen(laliste),0)==-1) {
        return -1;
    }
    return 0;
}

int send_user_name_ack(struct client* client,int fd){
  assert(client);
  printf("coucou\n");
  char info[INFOS_LEN];
  printf("nouveau user : %s\n",get_username(client));
  strcpy(info,get_username(client));
  struct message* message=malloc(sizeof(struct message));
  message= init_message_serveur(NICKNAME_ACCEPT,info,0);
  printf("un message nickname ack have been send : %d\n ",message_get_type(message));
  if (send(fd, message,sizeof(struct message),0)==-1) {
      return -1;
  }
  return 0;

}


int send_user_info(struct client * clients,int fd,char * pseudo){
    assert(clients);
    struct message* message_envoye=malloc(sizeof(struct message));
    struct client* user_cible=chererche_client_par_pseudo(pseudo,clients);
    char tmp[50];
    char laliste[MSG_LEN];
    //printf("%s\n", pseudo);
    if (user_cible==NULL) {
        strcpy(laliste,"L'utilisateur n'existe pas ou n'est pas connecté");
        message_envoye= init_message_serveur(INFO_SERVEUR,"rien",strlen(laliste));
        if (send(fd,message_envoye,sizeof(struct message),0)==-1) {
            return -1;
        }
        if (send(fd,laliste,strlen(laliste),0)==-1) {
            return -1;
        }
    }
    else{
        char port[4];
        char time[4];
        strcpy(laliste,"Info de : ");
        strcpy(tmp,get_username(user_cible));
        strcat(laliste,tmp);
        strcpy(tmp,"\n   Adresse IP= ");
        strcat(laliste,tmp);
        strcpy(tmp,get_adress_client(user_cible));
        strcat(laliste,tmp);
        strcpy(tmp,"\n   Connecté depuis= ");
        strcat(laliste,tmp);
        strcpy(tmp,"\n   Port= ");
        strcat(laliste,tmp);
        sprintf(port, "%d", get_timer_client(user_cible));
        strcpy(tmp,port);
        strcat(laliste,tmp);
        sprintf(time, "%d", get_port_client(user_cible));
        strcpy(tmp,time);
        strcat(laliste,tmp);
        strcpy(tmp,"\n");
        strcat(laliste,tmp);

        message_envoye= init_message_serveur(INFO_SERVEUR,"rien",strlen(laliste));
        if (send(fd,message_envoye,sizeof(struct message),0)==-1) {
            return -1;
        }
        printf("%s\n",laliste);
        if (send(fd,laliste,sizeof(char)*strlen(laliste),0)==-1) {
            return -1;
        }
    }
    return 0;

}

//Save username
struct client* set_username(struct client* client,char* username){
    assert(client);
    client->username=malloc(sizeof(char)*NICK_LEN);
    strcpy(client->username,username);
    return client;
}

struct client * add_to_clients(int sock,struct sockaddr *adress,int port,struct client* fir){
    assert(fir);
    assert(adress);
  struct client* client=malloc(sizeof(struct client));
  while (fir->next != NULL) {
    printf("%s est le fir courant\n",get_username(fir));
    fir=fir->next;
    printf("%s est le fir suivant\n",get_username(fir));
    printf("1passage\n" );
  }
  fir->next=client;
  client->sock=sock;
  client->adress=adress;
  client->playload=0;
  client->timeconnect=time(NULL);
  client->port=port;
  client->next=NULL;
  client->username="unknown";
  return client;
}

int send_client_error(int fd, enum msg_err type){
    char buff[MSG_LEN];
    memset(buff, 0, MSG_LEN);
    switch (type) {
        case DEST_UNKNOW:
        strncpy(buff, "Dest_user is unknow or has been disconnect",42);
        break;
        case SEND_IMPOSSIBLE:
        strncpy(buff, "Unidentified probleme when sending the msg. Please try again.", 73);
        break;
    }
    if (send(fd, &buff, MSG_LEN, 0) <= 0) {
        return -1;
    }
    else{
        return 0;
    }
}

void save_infos(struct client * client, struct message* message){
    assert(client);
    assert(message);
    printf("On entregistre les infos\n");
    set_playload_client(client,message_get_playload(message));
    printf("les infos pas complet %s\n",message->infos );
    set_infos_client(client,message_get_infos(message));

    set_msgtype_client(client,message_get_type(message));
    printf("le type du client %i\n",get_msgtype_client(client) );
    printf("le type %i\n",message_get_type(message) );
    printf("les info sont enregistrer\n");
}

int send_to_dest(struct client * client_courant, char* message,struct client * clients){
    struct message* message_envoye=malloc(sizeof(struct message));

    switch (get_msgtype_client(client_courant)) {
        case UNICAST_SEND:
            if (chererche_client_par_pseudo(get_infos_client(client_courant),clients)!=NULL) {
                message_envoye= init_message_transf(UNICAST_SEND,"rien",get_playload_client(client_courant),get_username(client_courant));

                if (send(get_fd(chererche_client_par_pseudo(get_infos_client(client_courant),clients)),message_envoye,sizeof(struct message),0)==-1) {
                    return -1;
                }
                if (send(get_fd(chererche_client_par_pseudo(get_infos_client(client_courant),clients)), message, get_playload_client(client_courant)*sizeof(char), 0) <= 0) {
                    return -1;
                }
                break;
                }
            else{
                char *msg_char="L'utilisateur n'existe pas ou n'est pas connecté";
                message_envoye= init_message_serveur(INFO_SERVEUR,"rien",strlen(msg_char));
                if (send(get_fd(client_courant),message_envoye,sizeof(struct message),0)==-1) {
                    return -1;
                }
                if (send(get_fd(client_courant),msg_char,strlen(msg_char),0)==-1) {
                    return -1;
                }
            }
        case BROADCAST_SEND:
            message_envoye= init_message_transf(BROADCAST_SEND,message,get_playload_client(client_courant),get_username(client_courant));
            while(clients->next!=NULL){
                clients=clients->next;
                printf("client : %s \n", get_username(client_courant) );
                if (get_fd(client_courant)!=get_fd(clients)) {
                    printf("le message envoye %s\n", message);
                    printf("le type du message envoye send %i\n", get_msgtype_client(client_courant));
                    printf("playload %i\n", get_playload_client(client_courant));
                    if (send(get_fd(clients), message_envoye, sizeof(struct message), 0) <= 0) {
                        return -1;
                    }
                    else
                      printf("message de type %i a ete envoyé\n", message_get_type(message_envoye));

                    if (send(get_fd(clients),message,strlen(message),0)==-1) {
                        printf("probleme ici\n");
                        return -1;
                    }
                    else
                      printf("message char envoyé %s\n", message);

                }
            }
            break;
        case ECHO_SEND:
        message_envoye= init_message_serveur(ECHO_SEND,"rien",get_playload_client(client_courant));
        printf("type = %d\n", message_get_type(message_envoye));
        printf("username = %s\n", message_get_nick_sender(message_envoye));
        printf("playload = %d\n", message_get_playload(message_envoye));
        printf("infos = %s\n", message_get_infos(message_envoye));
        if (send(get_fd(client_courant),message_envoye,sizeof(struct message),0)==-1) {
            printf("pb_envoye\n");
            return -1;
        }
        if (send(get_fd(client_courant), message, get_playload_client(client_courant)*sizeof(char), 0) <= 0) {
            printf("pb_envoye\n");
            return -1;
        }
        return 0;
        default:
        return -1;

    }
    printf("pas de pb_envoye\n");
    return 0;
}
