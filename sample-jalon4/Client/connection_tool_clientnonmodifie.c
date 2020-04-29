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

struct message* creatmsg(int sfd,int payload,char *pseudo,enum msg_type type,char * info){
	struct message* message=malloc(sizeof(struct message*));
	message->pld_len=payload;
	strncpy(message->nick_sender,pseudo,strlen(pseudo));
	message->type=type;
	strncpy(message->infos,info,0);
	return message;
}

char* unpseudo(int sfd){

	char nick[5]="/nick";
	int res;
	int i;
	int lennick=5;
	char *pseudo=malloc(sizeof(char)*NICK_LEN);
	char * commande=malloc(sizeof(char)*(NICK_LEN+lennick+1));//taille totale maximale avce l espace entre la commande et le pseudo
  char *buf=malloc(sizeof(char));
	read(0,commande,(NICK_LEN+lennick+1));
	int len=strlen(commande)-1;
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

		struct message* message=creatmsg(sfd,0,pseudo,NICKNAME_NEW,NULL);
		send(sfd, message, sizeof(struct message*), 0);
		printf("Vous avez choisi le pseudo %s... Nous allons vérifier si il est disponible\n",message_get_nick_sender(message));
    //free(commande);
    free(buf);
		return pseudo;
		}
	else
    //free(commande);
    free(buf);
		return NULL;

}

struct message* lecture_shell(int sfd,char *pseudo){

	printf("commandes\n");

	char who[4]="/who";
	char whois[6]="/whois";
	char msgall[7]="/msgall";
	char msg[4] = "/msg";
	char quit[5]="/quit";
  char * info=malloc(sizeof(char)*INFOS_LEN);
	char *commande=malloc(sizeof(char)*125);

	read(0,commande,(INFOS_LEN+7));
	printf("la commande est : %s\n",commande);
	if (strncmp(commande,who,4)==0) {
		info =recupinfo(commande,strlen(who));
		printf("On recupère les infos : %s\n",info );
		return creatmsg(sfd,INFOS_LEN,pseudo,NICKNAME_INFOS,info);
	}
	 else if (strncmp(commande,whois,6)==0) {
		info=recupinfo(commande,strlen(whois));
		return creatmsg(sfd,INFOS_LEN,pseudo,NICKNAME_LIST,info);
	}
	else if (strncmp(commande,msgall,7)==0) {
		info=recupinfo(commande,strlen(msgall));
		return creatmsg(sfd,INFOS_LEN,pseudo,BROADCAST_SEND,info);
	}
	else if (strncmp(commande,msg,4)==0) {
		info=recupinfo(commande,strlen(msg));
		return creatmsg(sfd,INFOS_LEN,pseudo,UNICAST_SEND,info);
	}
	else if (strncmp(commande,quit,5)==0) {
		return creatmsg(sfd,0,pseudo,QUIT,NULL);
	}
	else{
		info=recupinfo(commande,strlen(msgall));
		return creatmsg(sfd,INFOS_LEN,pseudo,ECHO_SEND,info);
	}

//free(commande);
return NULL;
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
int communication_avec_le_serveur(int sockfd,char * pseudo) {

	int i;

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

	//Envoi/Reception.
	while (1) {
	   struct message* message=	lecture_shell(sockfd,pseudo);
		 printf("%s pseudo\n", message->nick_sender);
	    if (message->type==QUIT) {
				printf("On envoie un msg %d\n",message_get_type(message) );
		      send(sockfd, message, sizeof(struct message*), 0);
					free(message);
		      close(sockfd);
					return 0;
	         }

	 printf("On envoie un msg %d\n",message_get_type(message) );
	  send(sockfd, message, sizeof(struct message*), 0);
		printf("%s\n",message->nick_sender );
		printf("%s\n",message->infos );

	}
	while (1) {
		poll(fds,2,-1);
		for (i = 0; i < 2; i++) {

			if(fds[i].revents==POLLIN){

				//Recevoir
				if (fds[i].fd==sockfd) {
					// Reception d'un message struct
					if (recv(sockfd, msgreceived, sizeof(struct message*), 0) > 0) {

						printf("sender: %s\n", message_get_nick_sender(msgreceived));
						printf("infos: %s\n", message_get_infos(msgreceived));
						printf("type: %d\n", message_get_type(msgreceived));

			            if (msgreceived->type==NICKNAME_WRONG) {
			              close(sockfd);
						  printf("The nick name is wrong please retry\n" );
			              return 1;
			            }
			            if (msgreceived->type==QUIT) {
			          		break;
			          	}
						break;
					}
					//Receptiond'un message char*
					else if (recv(sockfd,&message_char, MSG_LEN,0)>0) {
						printf("on est passé par la ? \n");
	          printf("%s \n",message_char);
						memset(&message_char, 0, MSG_LEN);
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
