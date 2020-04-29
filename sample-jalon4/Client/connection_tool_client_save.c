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

#include "msg_struct.h"



int analyse_msg(struct message * message,char *pseudo){
    switch (message_get_type(message)) {
				case NICKNAME_WRONG:
				return 0;
				case NICKNAME_ACCEPT:
						strcpy(pseudo,message_get_infos(message));
				return 0;
        default:
        break;
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
	printf("verification du pseudo en cours...\n");
	if ((res=strncmp(commande,nick,5))==0) {
		if (lennick==len || lennick+1==len) {
			printf("pseudo non valide,retapez la commande nick avec un pseudo (lettres et chiffres sans espaces)\n");
			return NULL;
		}
		for (i = lennick+1; i < len ; i++) {
			if ((commande[i]>47 && commande[i]<58) || (commande[i]<123 && commande[i] >96) || (commande[i]<91 && commande[i]>64) )  {
				*buf=commande[i];
				strcat(pseudo,buf);
			}
			else{
				printf("pseudo non valide,retapez la commande nick avec un pseudo (lettres et chiffres sans espaces)\n");
				return NULL;
			}
			}

		// struct message* message=creatmsg(sfd,0,pseudo,NICKNAME_NEW,NULL,0);
		// send(sfd, message, sizeof(struct message*), 0);
		// printf("Vous avez choisi le pseudo %s... Nous allons vérifier si il est disponible\n",message_get_nick_sender(message));
		printf("les pseudo est bon %s\n",pseudo);
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

int lecture_shell(int sockfd,char *pseudo){

	printf("commandes\n");

	char nick[5]="/nick";
	char who[4]="/who";
	char whois[6]="/whois";
	char msgall[7]="/msgall";
	char msg[4] = "/msg";
	char quit[5]="/quit";
  char * info=malloc(sizeof(char)*INFOS_LEN);
	char *commande=malloc(sizeof(char)*MSG_LEN);
	char new_pseudo[NICK_LEN];
	memset(&new_pseudo,0,NICK_LEN);
	struct message* message=malloc(sizeof(struct message*));

	read(0,commande,(INFOS_LEN+7));
	if (strncmp(commande,nick,5)==0) {
		strcpy(new_pseudo,verif_pseudo(commande));
		if (new_pseudo!=NULL) {
			message=creatmsg(0,new_pseudo,NICKNAME_NEW,new_pseudo);
			printf("old nickname : %s\n", message_get_nick_sender(message));
			printf("new nickname : %s\n", message_get_infos(message));
			send(sockfd, message, sizeof(struct message*), 0);
			return 0;
		}
		return -1;
	}
	if (strncmp(commande,who,4)==0) {
		info =recupinfo(commande,strlen(who));
		message = creatmsg(0,pseudo,NICKNAME_INFOS,info);
		send(sockfd, message, sizeof(struct message*), 0);
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

	int i;
	int quit;
	char *pseudo=malloc(sizeof(char)*NICK_LEN);
	strcpy(pseudo,"unknow");
	//Création du descipteur qui sera chargé de lire le message écrit dans
	// le terminal par le client et l'écrire dans la socket.
	struct pollfd fds[2];
	fds[0].fd=sockfd;
	fds[1].fd=0;
	fds[0].events=POLLIN;
	fds[1].events=POLLIN;
	struct message* msgreceived=malloc(sizeof(struct message*));
	char *message_char=malloc(MSG_LEN*sizeof(char));
	memset(&message_char, 0, MSG_LEN);
	memset(&msgreceived, 0, sizeof(msgreceived));

	//Tant que le pseudo est pas valié.
	while (1) {
    quit=lecture_shell(sockfd,pseudo);
		if (quit==-2) {
				printf("On va quitter mtn \n");
				close(sockfd);
				return 0;
				 }
		if (recv(sockfd, msgreceived, sizeof(struct message*), 0) > 0) {
			printf("message de type: %d\n", message_get_type(msgreceived));
			analyse_msg(msgreceived,pseudo);
		}
		//Si il reçoit le bienvenu, alors c'est bon.
		if (recv(sockfd,&message_char, MSG_LEN,0)>0) {
			printf("%s \n",message_char);
			memset(&message_char, 0, MSG_LEN);
		}

	}

	//Envoi/Reception.
	while (1) {

		poll(fds,2,-1);
		for (i = 0; i < 2; i++) {

			if(fds[i].revents==POLLIN){

				//Recevoir
				if (fds[i].fd==sockfd) {
					// Reception d'un message struct
					if (recv(sockfd, msgreceived, sizeof(struct message*), 0) > 0) {
						printf("message de type: %d\n", message_get_type(msgreceived));
						analyse_msg(msgreceived,pseudo);
						break;
					}
					//Receptiond'un message char*
					if (recv(sockfd,&message_char, MSG_LEN,0)>0) {
	          printf("%s \n",message_char);
						memset(&message_char, 0, MSG_LEN);
					}

				}
				if (fds[i].fd==0) {

					lecture_shell(sockfd,pseudo);
					if (quit==-2) {
							printf("On va quitter mtn \n");
							close(sockfd);
							return 0;
					 }
				}


				}
		    }
    }
		free(msgreceived);
	  free(msgreceived->nick_sender);
	  free(msgreceived->infos);
		close(sockfd);
		return 0;
  }
