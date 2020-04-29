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

void client_set_salon(struct client* client,char * salon){
    assert(client);
    client->salon=salon;
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

char* get_user_salon(struct client* client){
    assert(client);
    return client->salon;
}



time_t get_timer_client(struct client* user_cible){
   time_t time_connect=user_cible->timeconnect;
   time_t now=time(NULL);
   return now-time_connect;
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
char * set_user_salon(struct client* client, char *salon){
  assert(client);
  client->salon=malloc(NICK_LEN);
  strcpy(client->salon,salon);
  return client->salon;
}

int check_name_available(char *username,struct client* fir){
    assert(fir);
    if (username==NULL) {
        return 0;
    }
    else{
        struct client* pt_client=fir;
        while (pt_client->next != NULL) {
          int compar=strcmp(username,get_username(pt_client));
            if (compar==0) {
              printf("le user name est invalide ou deja utilisé : %s \n",get_username(pt_client));
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
    int size=0;
    while(pseudo[size]!=0){
        size++;
    }
    if (strncmp(fir->username,pseudo,size-1)==0) {
        return fir;
    }

    while (fir->next != NULL) {
        fir=fir->next;
        if (strncmp(fir->username,pseudo,size-1)==0) {
            return fir;
        }
    }

    return NULL;
}

void erase_client(int fd,struct client* fir){
    struct client* curs= fir->next;
    struct client* pres= fir;

    while (pres->next != NULL) {

        if (curs->sock==fd) {
          if (curs->next!=NULL) {
            pres->next=curs->next;
            break;
          }
          else{
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
    if (send(fd,message,sizeof(struct message),0)==-1) {
        return -1;
    }
    free(message);
    return 0;
}
int send_add_salon_ack(int fd,char *info ){
  char infos[INFOS_LEN];
  strcpy(infos,info);
  struct message* message=malloc(sizeof(struct message));
  message= init_message_serveur(SALON_JOIN_ACCEPT,infos,0);
  if (send(fd,message,sizeof(struct message),0)==-1) {
      return -1;
  }
  free(message);
  return 0;
}


int send_salon_name_wrong(int fd){
    char info[INFOS_LEN];
    char *buf="rien";
    strcpy(info,buf);
    struct message* message=malloc(sizeof(struct message));
    message= init_message_serveur(SALON_WRONG,info,0);
    if (send(fd,message,sizeof(struct message),0)==-1) {
        return -1;
    }
    free(message);
    return 0;
}

int send_salon_wrong(int fd){
    char info[INFOS_LEN];
    char *buf="rien";
    strcpy(info,buf);
    struct message* message=malloc(sizeof(struct message));
    message= init_message_serveur(SALON_JOIN_REFUS,info,0);
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
    char tmp[NICK_LEN];
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
  char info[INFOS_LEN];
  printf("nouveau user : %s\n",get_username(client));
  strcpy(info,get_username(client));
  struct message* message=malloc(sizeof(struct message));
  message= init_message_serveur(NICKNAME_ACCEPT,info,0);
  if (send(fd, message,sizeof(struct message),0)==-1) {
      return -1;
  }
  return 0;
}

int send_salon_name_ack(struct client* client,int fd){
    assert(client);
    char info[INFOS_LEN];
    strcpy(info,get_user_salon(client));
    struct message* message=malloc(sizeof(struct message));
    message= init_message_serveur(SALON_ACCEPT,info,0);
    if (send(fd, message,sizeof(struct message),0)==-1) {
        return -1;
    }
    return 0;
}



int send_user_info(struct client * clients,int fd,char * pseudo){
    assert(clients);
    struct message* message_envoye=malloc(sizeof(struct message));
    //Comme il y a un espace parasite dans le char pseudo, on fait &pseudo[1] pour prendre la chaine a partir de 2e caratère.
    char *pseud=malloc(NICK_LEN);
    memset(pseud,0,NICK_LEN);
    strcpy(pseud,&pseudo[1]);
    struct client* user_cible=chererche_client_par_pseudo(pseud,clients);
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
    else if (user_cible==clients) {
        strcpy(laliste,"Le serveur, à votre service.");
        message_envoye= init_message_serveur(INFO_SERVEUR,"rien",strlen(laliste));
        if (send(fd,message_envoye,sizeof(struct message),0)==-1) {
            return -1;
        }
        if (send(fd,laliste,sizeof(char)*strlen(laliste),0)==-1) {
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
        sprintf(time, "%d", (int)get_timer_client(user_cible));
        strcpy(tmp,time);
        strcat(laliste,tmp);
        strcpy(tmp,"\n   Port= ");
        strcat(laliste,tmp);
        sprintf(port, "%d", get_port_client(user_cible));
        strcpy(tmp,port);
        strcat(laliste,tmp);
        strcpy(tmp,"\n");
        strcat(laliste,tmp);

        message_envoye= init_message_serveur(INFO_SERVEUR,"rien",strlen(laliste));
        if (send(fd,message_envoye,sizeof(struct message),0)==-1) {
            return -1;
        }
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
    fir=fir->next;
  }
  fir->next=client;
  client->sock=sock;
  client->adress=adress;
  client->playload=0;
  client->timeconnect=time(NULL);
  client->port=port;
  client->next=NULL;
  client->username="unknown";
  client->salon="aucun";
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
    set_playload_client(client,message_get_playload(message));
    set_infos_client(client,message_get_infos(message));
    set_msgtype_client(client,message_get_type(message));
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
        break;
        case BROADCAST_SEND:
            message_envoye= init_message_transf(BROADCAST_SEND,message,get_playload_client(client_courant),get_username(client_courant));
            while(clients->next!=NULL){
                clients=clients->next;
                if (get_fd(client_courant)!=get_fd(clients)) {
                    if (send(get_fd(clients), message_envoye, sizeof(struct message), 0) <= 0) {
                        return -1;
                    }
                    else
                    if (send(get_fd(clients),message,strlen(message),0)==-1) {
                        return -1;
                    }


                }
            }
        break;
        case MULTICAST_SEND:
          message_envoye= init_message_transf(MULTICAST_SEND,message,get_playload_client(client_courant),get_username(client_courant));
          while(clients->next!=NULL ){
            clients=clients->next;
            if (get_fd(client_courant)!=get_fd(clients) && strcmp(get_user_salon(client_courant),get_user_salon(clients))==0) {
                if (send(get_fd(clients), message_envoye, sizeof(struct message), 0) <= 0) {
                    return -1;
                }


                if (send(get_fd(clients),message,get_playload_client(client_courant),0)==-1) {
                    return -1;
                }

            }
        }
        break;
        case ECHO_SEND:
        message_envoye= init_message_serveur(ECHO_SEND,"rien",get_playload_client(client_courant));
        if (send(get_fd(client_courant),message_envoye,sizeof(struct message),0)==-1) {
            return -1;
        }
        if (send(get_fd(client_courant), message, get_playload_client(client_courant)*sizeof(char), 0) <= 0) {
            return -1;
        }
        return 0;
        case FILE_REQUEST:
        if (chererche_client_par_pseudo(get_infos_client(client_courant),clients)!=NULL) {
            message_envoye= init_message_transf(FILE_REQUEST,"rien",get_playload_client(client_courant),get_username(client_courant));

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
        break;
        default:
        return -1;

    }
    return 0;
}

int transmission_msg_struct(struct client *clients, struct client *client_courant){
    struct message* message_envoye=malloc(sizeof(struct message));
    switch (get_msgtype_client(client_courant)) {
        case FILE_ACCEPT:
            if (chererche_client_par_pseudo(get_infos_client(client_courant),clients)!=NULL) {
                message_envoye= init_message_transf(FILE_ACCEPT,"rien",0,get_username(client_courant));
                if (send(get_fd(chererche_client_par_pseudo(get_infos_client(client_courant),clients)),message_envoye,sizeof(struct message),0)==-1) {
                    return -1;
                }
                break;
            }
            else{
                char *msg_char="Votre correspondant à changé de nom ou c'est déconnecté.";
                message_envoye= init_message_serveur(INFO_SERVEUR,"rien",strlen(msg_char));
                if (send(get_fd(client_courant),message_envoye,sizeof(struct message),0)==-1) {
                    return -1;
                }
                if (send(get_fd(client_courant),msg_char,strlen(msg_char),0)==-1) {
                    return -1;
                }
            }
        break;
        case FILE_REJECT:
            if (chererche_client_par_pseudo(get_infos_client(client_courant),clients)!=NULL) {
                message_envoye= init_message_transf(FILE_REJECT,"rien",0,get_username(client_courant));
                if (send(get_fd(chererche_client_par_pseudo(get_infos_client(client_courant),clients)),message_envoye,sizeof(struct message),0)==-1) {
                    return -1;
                }
                break;
            }
            else{
                char *msg_char="Votre correspondant à changé de nom ou c'est déconnecté.";
                message_envoye= init_message_serveur(INFO_SERVEUR,"rien",strlen(msg_char));
                if (send(get_fd(client_courant),message_envoye,sizeof(struct message),0)==-1) {
                    return -1;
                }
                if (send(get_fd(client_courant),msg_char,strlen(msg_char),0)==-1) {
                    return -1;
                }
            }
        break;
        case FILE_SEND:
        if (chererche_client_par_pseudo(get_infos_client(client_courant),clients)!=NULL) {
            message_envoye= init_message_transf(FILE_SEND,"rien",0,get_username(client_courant));
            if (send(get_fd(chererche_client_par_pseudo(get_infos_client(client_courant),clients)),message_envoye,sizeof(struct message),0)==-1) {
                return -1;
            }
            break;
        }
        else{
            char *msg_char="Votre correspondant à changé de nom ou c'est déconnecté.";
            message_envoye= init_message_serveur(INFO_SERVEUR,"rien",strlen(msg_char));
            if (send(get_fd(client_courant),message_envoye,sizeof(struct message),0)==-1) {
                return -1;
            }
            if (send(get_fd(client_courant),msg_char,strlen(msg_char),0)==-1) {
                return -1;
            }
        }
        break;
        case FILE_ACK:
        if (chererche_client_par_pseudo(get_infos_client(client_courant),clients)!=NULL) {
            message_envoye= init_message_transf(FILE_ACK,"rien",0,get_username(client_courant));
            if (send(get_fd(chererche_client_par_pseudo(get_infos_client(client_courant),clients)),message_envoye,sizeof(struct message),0)==-1) {
                return -1;
            }
            break;
        }
        else{
            char *msg_char="Votre correspondant à changé de nom ou c'est déconnecté.";
            message_envoye= init_message_serveur(INFO_SERVEUR,"rien",strlen(msg_char));
            if (send(get_fd(client_courant),message_envoye,sizeof(struct message),0)==-1) {
                return -1;
            }
            if (send(get_fd(client_courant),msg_char,strlen(msg_char),0)==-1) {
                return -1;
            }
        }
        break;
        default:
        return -1;
    }
    return 0;
}

int msg_all_serv(char * message, struct client * clients){
    struct message* message_envoye=malloc(sizeof(struct message));
    message_envoye= init_message_serveur(BROADCAST_SEND,"rien",strlen(message));
    while(clients->next!=NULL){
        clients=clients->next;
            if (send(get_fd(clients), message_envoye, sizeof(struct message), 0) <= 0) {
                return -1;
            }
            if (send(get_fd(clients),message,strlen(message),0)==-1) {
                return -1;
            }
    }
    return 0;
}
