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
#include "gestionsalon.h"



char* get_salon_name(struct salon* salon){
  assert(salon);
  return salon->salonname;
}
int get_salon_part( struct salon* salon){
  assert(salon);
  return salon->nb_part;
}

struct pseudo* get_salon_creator(struct salon* salon){
  assert(salon);
  return salon->premier;
}

void set_salon_name(struct salon* salon,char * info){
  assert(salon);
  salon->salonname=malloc(NICK_LEN);
  strcpy(salon->salonname,info);
}

void inc_salon_part(struct salon* salon){
  assert(salon);
  salon->nb_part+=1;
}
void dec_salon_part( struct salon* salon){
  assert(salon);
  salon->nb_part-=1;
}


void set_salon_firstusr(struct salon* salon,struct client* client){
  assert(salon);
  salon->premier=malloc(sizeof(struct pseudo));
  set_salon_creator(salon->premier,client);
}

void set_salon_creator(struct pseudo* pseudo,struct client* client){
  pseudo->username=malloc(NICK_LEN);
  strcpy(pseudo->username,get_username(client));
  pseudo->next=NULL;
}



void add_salon_part(struct salon* salon,struct client* client){
  assert(salon);
  struct pseudo* premier=get_salon_creator(salon);
  while (premier->next != NULL) {
    premier=premier->next;
  }
  premier->next=malloc(sizeof(struct pseudo));
  premier->next->username=malloc(NICK_LEN);
  strcpy(premier->next->username,get_username(client));
}

int joinsalon(struct client* client, struct message* message,struct salon* salons){
    char* salon=message_get_infos(message);
    salon_empty(salons,client);
    while (salons->next != NULL) {
        if (strcmp(salons->next->salonname,salon)==0) {
            add_salon_part(salons->next,client);
            inc_salon_part(salons->next);
            set_user_salon(client,salon);
            return 1;
        }
    salons=salons->next;
    }
    return 0;
}

struct salon* creatsalon(struct client *client,struct message* message,struct salon* salons){
    salon_empty(salons,client);
    struct salon* salon=malloc(sizeof(struct salon));
    set_salon_name(salon,message_get_infos(message));
    inc_salon_part(salon);
    set_salon_firstusr(salon,client);
    while (salons->next != NULL) {
        salons=salons->next;
    }
    salons->next=salon;
    salon->nb_part=0;
    inc_salon_part(salon);
    return salon;

}




int check_name_salon_available(char *nom,struct salon* salons){
    assert(salons);
    if (nom==NULL) {
        return 0;
    }
    else{
        struct salon* salle_client=salons;
        while (salle_client != NULL) {

            if (strcmp(nom,get_salon_name(salle_client))==0) {
                return -1;
            }
            salle_client=salle_client->next;
        }
    }
    return 0;
}

int send_list_salon(struct salon * salons,int fd){
    assert(salons);
    struct message* message_envoye=malloc(sizeof(struct message));
    char laliste[MSG_LEN];
    char tmp[NICK_LEN];
    strcpy(laliste,"la liste des salons :");


    while (salons != NULL) {

        if (strcmp(get_salon_name(salons),"aucun")) {
          strcpy(tmp,"   - ");
          strcat(laliste,tmp);

          strcpy(tmp,get_salon_name(salons));
          strcat(laliste,tmp);

          strcpy(tmp,"\n");
          strcat(laliste,tmp);
        }
        salons=salons->next;


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
int salon_empty(struct salon * salons,struct client* client){
    assert(salons);
    char* name=malloc(NICK_LEN);
    memset(name,0,NICK_LEN);
    name=get_user_salon(client);
    while (salons->next != NULL) {
        struct salon* salonprec=salons;
        salons=salons->next;
        if (strcmp(salons->salonname,name)==0) {
          dec_salon_part(salons);
          int part=get_salon_part(salons);
          if (part==0) {
            set_user_salon(client,"aucun");
            printf("salon vide,supprimé: %s\n",name);
            salonprec->next=salons->next;
          }
        }


    }

    return 0;
}
