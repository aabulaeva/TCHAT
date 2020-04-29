#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <assert.h>

#include "msg_struct.h"

#define MAXLINE 1024

int analyse_msg(struct message * message,char *pseudo){
  assert(message);
    switch (message_get_type(message)) {
				case NICKNAME_WRONG:
				return 0;
				case NICKNAME_ACCEPT:
            printf("nouveau pseudo !\n");
						strcpy(pseudo,message_get_infos(message));
				return 2;
        default:
        return 0;
    }
    return 0;
}



char *recupinfo(char *commande,int lencom){
	int i;
	int len=strlen(commande);

	char *buf=malloc(sizeof(char));
	char *res=malloc(sizeof(char)*INFOS_LEN);
	for (i = lencom +1; i < len; i++) {

		*buf=commande[i];
		strcat(res,buf);

	}
  free(buf);
	return res;
}

struct message* creatmsg(int playload,char pseudo[NICK_LEN],enum msg_type type,char info[INFOS_LEN]){
	struct message* message=malloc(sizeof(struct message*));
	message_set_nick_sender(message,pseudo);
	message_set_playload(message,playload);
	message_set_type(message,type);
	message_set_infos(message,info);
	return message;
}

char* verif_pseudo(char * commande){

	char nick[5]="/nick";
	int res;
	int i;
	int lennick=5;
	char *pseudo=malloc(sizeof(char)*NICK_LEN);
  char *buf=malloc(sizeof(char));
	int len=strlen(commande)-1;
	if ((res=strncmp(commande,nick,5))==0) {
		if (lennick==len || lennick+1==len) {
			printf("pseudo non valide,retapez la commande nick avec un pseudo (lettres et chiffres sans espaces)\n");
			return NULL;
		}
		for (i = lennick+1; i < len ; i++) {
			if ((commande[i]>47 && commande[i]<58) || (commande[i]<123 && commande[i] >96) || (commande[i]<91 && commande[i]>64) )  {
				*buf=commande[i];
				strncat(pseudo,buf,1);
			}
			else{
				printf("pseudo non valide,retapez la commande nick avec un pseudo (lettres et chiffres sans espaces)\n");
				return NULL;
			}
			}

		return pseudo;
		}
	else{
		return NULL;
	}
}

char* recup_message(char * commande, int nb_space){
	int i;
	int len=strlen(commande);
	int compteur=0;
	char *buf=malloc(sizeof(char));
	char *res=malloc(sizeof(char)*MSG_LEN);
	for (i = 0; i < len; i++) {
		if (compteur>=nb_space) {
			*buf=commande[i];
			strcat(res,buf);
		}
		if (commande[i]==' ') {
			compteur++;
		}
	}
	free(buf);
	return res;
}

int lecture_shell(int sockfd,char *pseudo,char *commande){

	printf("commandes\n");

	char nick[5]="/nick";
	char who[4]="/who";
	char whois[6]="/whois";
	char msgall[7]="/msgall";
	char msg[4] = "/msg";
	char quit[5]="/quit";
  char * info=malloc(sizeof(char)*INFOS_LEN);
	char new_pseudo[NICK_LEN];
	memset(&new_pseudo,0,NICK_LEN);
	struct message* message=malloc(sizeof(struct message*));

	if (strncmp(commande,nick,5)==0) {
		strcpy(new_pseudo,verif_pseudo(commande));
		if (new_pseudo!=NULL) {
			message=creatmsg(0,new_pseudo,NICKNAME_NEW,new_pseudo);
			send(sockfd, message, sizeof(struct message*), 0);
			return 0;
		}
		return -1;
	}
	else if (strncmp(commande,who,4)==0) {
		info =recupinfo(commande,strlen(who));
		message = creatmsg(0,pseudo,NICKNAME_INFOS,info);
		if(send(sockfd, message, sizeof(struct message*), 0))
		return 0;
	}
	 else if (strncmp(commande,whois,6)==0) {
		info=recupinfo(commande,strlen(whois));
		message=creatmsg(0,pseudo,NICKNAME_LIST,info);
		send(sockfd, message, sizeof(struct message*), 0);
		return 0;
	}
	else if (strncmp(commande,msgall,7)==0) {
		info=recupinfo(commande,strlen(msgall));
		char * msg_char=recup_message(commande,1);
		message=creatmsg(sizeof(msg_char),pseudo,BROADCAST_SEND,info);
		send(sockfd, message, sizeof(struct message*), 0);
		send(sockfd, &msg_char, sizeof(msg_char), 0);
		return 0;
	}
	else if (strncmp(commande,msg,4)==0) {
		info=recupinfo(commande,strlen(msg));
		char * msg_char=recup_message(commande,2);
		message=creatmsg(sizeof(msg_char),pseudo,UNICAST_SEND,info);
		send(sockfd, message, sizeof(struct message*), 0);
		send(sockfd, &msg_char, sizeof(msg_char), 0);
		return 0;
	}
	else if (strncmp(commande,quit,5)==0) {
		message=creatmsg(0,pseudo,QUIT,NULL);
		send(sockfd, message, sizeof(struct message*), 0);
		return -2;
	}
	else{
		message=creatmsg(MSG_LEN,pseudo,ECHO_SEND,NULL);
		send(sockfd, message, sizeof(struct message*), 0);
		send(sockfd, &commande,MSG_LEN, 0);
		return 0;
	}

//free(commande);
return -1;
}
//Fonction qui creer la connection avec le serveur. Renvoie le decripteur associé.//
int connection_avec_le_serveur(char * serv_addr,char* serv_port) {
	// Mise en place des variables.
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol=0;

	// Récupération des informations du serveur.
	if (getaddrinfo(serv_addr, serv_port, &hints, &result) != 0) {
		perror("Connection impossible : ");
		exit(EXIT_FAILURE);
	}

	// Choisi un descipteur libre.
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
}
		close(sfd);
	}
	if (rp == NULL) {
		fprintf(stderr, "Connection impossible : pas de descipteur libre\n");
		exit(EXIT_FAILURE);

}
	freeaddrinfo(result);
	return sfd;

}

// Fonction qui permet la communication avec le serveur.//
int communication_avec_le_serveur(int sockfd) {


	int i, acp;
	char * buffer=malloc(MAXLINE);
	char * SendMessage=malloc(MAXLINE);

	struct pollfd fds[MAXLINE];

	memset(buffer,0,MAXLINE);
	memset(SendMessage,0,MAXLINE);

	//POLL
	memset(fds , 0, MAXLINE*sizeof(struct pollfd));
	fds[0].fd = sockfd ;
	fds[1].fd = 0;
	fds[0].events = POLLIN;
	fds[1].events=POLLIN;
	int nfds = 2;
	while (1) {
		int verif = poll(fds, nfds, -1);
		if (verif < 0) {
			perror("poll");
			exit(EXIT_FAILURE);
		}
		for (i = 0; i < nfds; i++) {
			if (fds[i].fd == -1) {
				continue;
			}
			if (fds[i].revents == POLLIN) {
				if (fds[i].fd==sockfd) {
					memset(buffer,0,MAXLINE);
					if ((recv(sockfd, buffer,sizeof(buffer),0))>0) {
						printf("Message reçu par le serveur : %s\n", buffer);
					}
	        if(strcmp(buffer, "/quit\n") == 0) {
	        	printf("Session terminée\n");
	          free(buffer);
						free(SendMessage);
	          close(sockfd);
						return 0;
	        }
				}
				else {
					if( fds[i].fd == STDIN_FILENO) {
	       		fgets(SendMessage,MAXLINE,stdin);
						if(( acp = send(sockfd, SendMessage, strlen(SendMessage), 0))<0 ) {
							perror("send");
							exit(EXIT_FAILURE);
						}
					}
				}
			}
		}
	}

  free(buffer);
	free(SendMessage);
	close(sockfd);
	for(i=0;i<nfds;i++){
		close(fds[i].fd);
	}
	return 0 ;
}
