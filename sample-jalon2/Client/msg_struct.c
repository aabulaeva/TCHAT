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
#include <assert.h>

#include "msg_struct.h"

char* message_get_infos(struct message* message){
    assert(message);
    return message->infos;
}

enum msg_type message_get_type(struct message* message){
    assert(message);
    return message->type;
}

int message_get_playload(struct message* message){
    assert(message);
    return message->pld_len;
}

char * message_get_nick_sender(struct message* message){
    assert(message);
    return message->nick_sender;
}

void message_set_infos(struct message* message, char info[INFOS_LEN]){
    assert(message);
    strcpy(message->infos,info);
}

void message_set_type(struct message* message, enum msg_type type){
    assert(message);
    message->type=type;
}

void message_set_playload(struct message* message, int playload){
    assert(message);
    message->pld_len=playload;
}

void message_set_nick_sender(struct message* message,char nom[NICK_LEN]){
    assert(message);
    strcpy(message->nick_sender,nom);
}

struct message* init_message_serveur(enum msg_type type){
    struct message *message=malloc(sizeof(message));
    char pseudo[NICK_LEN];
    strcpy(pseudo,"serveur");
    message_set_nick_sender(message,pseudo);
    message_set_playload(message,0);
    message_set_type(message,type);
    message_set_infos(message,"rien");
    return message;
}
