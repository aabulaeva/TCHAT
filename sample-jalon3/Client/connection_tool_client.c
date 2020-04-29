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
#include "salons.h"

#define MAXLINE 1024

int analyse_msg(struct message * message,char *pseudo,char* salon,int sockfd){
  assert(message);
  char * buffer_char=malloc(MSG_LEN);
  char unknown[7]="unknown";
  char aucun[5]="aucun";
  memset(buffer_char,0,MSG_LEN);
    switch (message_get_type(message)) {
		case NICKNAME_WRONG:
		      printf("Ce pseudo est déjà utilisé par un autre utilisateur ! :( \n");
		case NICKNAME_ACCEPT:
            printf("Nouveau pseudo !\n");
			strcpy(pseudo,message_get_infos(message));
		return 2;
    case SALON_WRONG:
      printf("Ce nom de salon est déjà utilisé ! :( \n");
      return 0;
    case SALON_ACCEPT:
      printf("Salon crée avec succès !\n");
      strcpy(salon,message_get_infos(message));
      return 2;
    case ECHO_SEND:
      if (strncmp(pseudo,unknown,7)!=0 && recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
        if (buffer_char!=NULL) {
          printf("vous : %s\n",buffer_char );
          memset(buffer_char,0,MSG_LEN);
          }
        }
        return 0;
        case UNICAST_SEND:
            if (strncmp(pseudo,unknown,7)!=0 && recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
                printf("(privé) %s : %s \n",message_get_nick_sender(message), buffer_char );
                memset(buffer_char,0,MSG_LEN);
            }
        return 0;
        case BROADCAST_SEND:
            if (strncmp(pseudo,unknown,7)!=0 && recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
                printf("%s : %s\n", message_get_nick_sender(message), buffer_char );
                memset(buffer_char,0,MSG_LEN);
            }
        return 0;
        case INFO_SERVEUR:
        if (recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
            printf("serveur : %s\n",buffer_char );
            memset(buffer_char,0,MSG_LEN);
        }
        break;
        return 0;
        case MULTICAST_SEND:
        if (strncmp(salon,aucun,5)!=0){
            if (recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
                printf("[%s] %s : %s\n",salon, message_get_nick_sender(message), buffer_char );
                memset(buffer_char,0,MSG_LEN);
            }
        return 0;
        }
        case SALON_JOIN_ACCEPT:
          printf(" ajouté au Salon  avec succès !\n");
          strcpy(salon,message_get_infos(message));
          return 0;
        case 	SALON_JOIN_REFUS:
          printf("ce salon n'existe pas \n");
        return 0;
        default:
          break;
        return 0;
      }
    return 0;
}


char *recupinfo(char *commande,int lencom,int cond){
	int i;
	int len=strlen(commande);
  int compteur=0;

	char *buf=malloc(sizeof(char));
	char *res=malloc(sizeof(char)*INFOS_LEN);
	for (i = lencom-1; i < len; i++) {
    if (commande[i]==' ') {
      compteur++;
    }
    if (compteur<1 && cond==1) {
		*buf=commande[i];
		strcat(res,buf);
	 }
   else if (cond==0){
     *buf=commande[i];
     strcat(res,buf);

   }
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
  char unknown[7]="unknown";
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
    if (strncmp(pseudo,unknown,7)==0) {
            printf("Vous n'avez pas le droit de vous appeler comme ça ! \n");
      return NULL;
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
	char *res=malloc(MSG_LEN);
	for (i = 0; i < len; i++) {
    if (commande[i]==' ') {
			compteur++;
		}
		if (compteur>=nb_space) {

			*buf=commande[i];
			strcat(res,buf);
		}

	}
	free(buf);
	return res;
}


int lecture_shell(int sockfd,char *pseudo,char* salon,char *commande){


	char nick[6]="/nick ";
    char who[4]="/who";
	char whois[7]="/whois ";
	char msgall[8]="/msgall ";
	char msg[5] = "/msg ";
	char quit[5]="/quit";
    char create[8]="/create ";
    char channel_list[13]="/channel_list";
    char join[6]="/join ";
    char unknown[7]="unknown";
    char aucun[5]="aucun";

  char * info=malloc(sizeof(char)*INFOS_LEN);
	char new_pseudo[NICK_LEN];
    char new_salon[NICK_LEN];
	memset(&new_pseudo,0,NICK_LEN);
	struct message* message=malloc(sizeof(struct message));
	if (strncmp(commande,nick,6)==0) {
    if (verif_pseudo(commande) != NULL) {
      strcpy(new_pseudo,verif_pseudo(commande));
		    if (new_pseudo!=NULL) {
			       message=creatmsg(0,new_pseudo,NICKNAME_NEW,new_pseudo);
			       send(sockfd, message, sizeof(struct message), 0);
			       return 0;
		         }
           }

		return -1;
	}
    else if (strncmp(commande,whois,7)==0 && strncmp(pseudo,unknown,7)!=0) {
       strcpy(info,recupinfo(commande,strlen(whois),0));
       message = creatmsg(0,pseudo,NICKNAME_INFOS,info);
       if(send(sockfd, message, sizeof(struct message), 0))
       return 0;
   }
   else if(strncmp(commande,whois,6)==0 && strncmp(commande,whois,7)!=0 && strncmp(pseudo,unknown,7)!=0){
       printf("utilisation : /whois pseudo \n");
       return 0;
   }
   else if (strncmp(commande,who,4)==0 && strncmp(pseudo,unknown,7)!=0 ) {
       info=recupinfo(commande,strlen(who),1);
       message=creatmsg(0,pseudo,NICKNAME_LIST,info);
       send(sockfd, message, sizeof(struct message), 0);
       return 0;
   }

  else if (strncmp(commande,msgall,8)==0 && strncmp(pseudo,unknown,7)!=0) {
    info=recupinfo(commande,strlen(msgall),1);
    char *msg_char=malloc(MSG_LEN);
    msg_char=recup_message(commande,1);
    message=creatmsg(strlen(msg_char),pseudo,BROADCAST_SEND,info);
    send(sockfd, message, sizeof(struct message), 0);
    send(sockfd, msg_char, strlen(msg_char), 0);
    return 0;
  }
	else if (strncmp(commande,msg,5)==0 && strncmp(pseudo,unknown,7)!=0) {
		info=recupinfo(commande,strlen(msg),1);
		char * msg_char=recup_message(commande,2);
		message=creatmsg(strlen(msg_char),pseudo,UNICAST_SEND,info);
		send(sockfd, message, sizeof(struct message), 0);
		send(sockfd, msg_char, strlen(msg_char), 0);
		return 0;
	}
    else if (strncmp(commande,create,8)==0 && strncmp(pseudo,unknown,7)!=0) {
        info=recupinfo(commande,strlen(create),0);
        strcpy(new_salon,verif_salon_name(commande));
             if (new_salon!=NULL) {
                message=creatmsg(0,pseudo,MULTICAST_CREATE,info);
                send(sockfd, message, sizeof(struct message), 0);
                return 0;
            }
        return 0;
    }
    else if (strncmp(commande,channel_list,13)==0 && strncmp(pseudo,unknown,7)!=0) {
        message=creatmsg(0,pseudo,MULTICAST_LIST,"rien");
        send(sockfd, message, sizeof(struct message), 0);
        return 0;
    }
    else if (strncmp(commande,join,6)==0 && strncmp(pseudo,unknown,7)!=0) {
        info=recupinfo(commande,strlen(join),0);
        strcpy(new_salon,info);
              if (new_salon!=NULL) {
                message=creatmsg(0,pseudo,MULTICAST_JOIN,info);
                send(sockfd, message, sizeof(struct message), 0);
                return 0;
            }
        return 0;
    }
	else if (strncmp(commande,quit,5)==0 ) {
        if (strncmp(salon,aucun,5)!=0) {
          printf("vous quittez le salon\n" );
            message=creatmsg(0,pseudo,MULTICAST_QUIT,"rien");
            send(sockfd, message, sizeof(struct message), 0);
            strcpy(salon,aucun);
            message=creatmsg(0,pseudo,SALON_EMPTY,"rien");
            send(sockfd, message, sizeof(struct message), 0);
            return 0;
        }
        else{
		message=creatmsg(0,pseudo,QUIT,"rien");
		send(sockfd, message, sizeof(struct message), 0);
		return -2;
        }
	}
	else{
      if(strncmp(pseudo,unknown,7)!=0){
        if (strncmp(salon,aucun,5)!=0) {
          char *msg_char=malloc(MSG_LEN);
          msg_char=recup_message(commande,0);
          message=creatmsg(strlen(msg_char),pseudo,MULTICAST_SEND,"rien");
          send(sockfd, message, sizeof(struct message), 0);
          send(sockfd, msg_char, strlen(msg_char), 0);
          return 0;
        }
        else{
    		message=creatmsg(MSG_LEN,pseudo,ECHO_SEND,"rien");
            printf("message envoyer : %d\n",message_get_type(message));
            printf("je suis : %s\n",message_get_nick_sender(message));
    		send(sockfd, message, sizeof(struct message), 0);
    		send(sockfd, commande,MSG_LEN, 0);
        }
    }
    else
      printf("connectez vous avec /nick\n" );
		return 0;
	}

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
	struct message buffer_struct;
	char * SendMessage=malloc(MAXLINE);
    char *commande=malloc(MSG_LEN*sizeof(char));
    memset(commande, 0, MSG_LEN);
	int quit;
	char *pseudo=malloc(sizeof(char)*NICK_LEN);
	strcpy(pseudo,"unknown");
    char *salon=malloc(sizeof(char)*NICK_LEN);
    strcpy(salon,"aucun");
	struct pollfd fds[MAXLINE];

	memset(&buffer_struct,0,sizeof(struct message));
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
                    memset(&buffer_struct,0,sizeof(struct message));
					if (recv(sockfd, &buffer_struct,sizeof(struct message),0)>0) {
                        analyse_msg(&buffer_struct,pseudo,salon,sockfd);
					}
          else if (recv(sockfd, &buffer_struct,sizeof(struct message),0)==0) {
                        printf("le serveur s est fermé\n");
                        free(SendMessage);
                      	close(sockfd);
                      	for(i=0;i<nfds;i++){
                      		close(fds[i].fd);
                      	}
                      	return 0 ;

					}
				}
				else {
					if( fds[i].fd == STDIN_FILENO) {
                        printf(" *** \n");
                        read(0,commande,MSG_LEN);
                        quit=lecture_shell(sockfd,pseudo,salon,commande);
                        memset(commande, 0, MSG_LEN);
                        if (quit==-2) {
                            close(sockfd);
                            return 0;
                        }
					}
				}
			}
		}
	}

	free(SendMessage);
	close(sockfd);
	for(i=0;i<nfds;i++){
		close(fds[i].fd);
	}
	return 0 ;
}
