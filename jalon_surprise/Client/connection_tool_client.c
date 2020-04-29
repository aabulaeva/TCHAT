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

struct list_noir{
    char *intru;
    struct list_noir* next;
};

char * get_intru_name(struct list_noir * list_noir){
	assert(list_noir);
	return list_noir->intru;
}

int is_in_list_noir(struct list_noir * list_noir,char * intru){
	while(list_noir->next!=NULL){
		list_noir=list_noir->next;
		if (strcmp(intru,list_noir->intru)==0) {
			return -1;
		}
	}
return 0;
}


void add_intru(struct list_noir * list_noir,char * intru){
	while(list_noir->next!=NULL){
		list_noir=list_noir->next;
	}
	struct list_noir* new_intru=malloc(sizeof(struct list_noir));
	new_intru->intru=intru;
	new_intru->next=NULL;
	list_noir->next=new_intru;
}

void rm_intru(struct list_noir * list_noir,char * intru){
	while(list_noir->next!=NULL){
		if (strcmp(intru,list_noir->next->intru)==0) {
			list_noir->next=list_noir->next->next;
			break;
		}
		list_noir=list_noir->next;
	}
}

char *recupinfo2(char *commande){
	int i;
  int compteur=0;
	printf("%s\n",commande );
	char *buf=malloc(sizeof(char));
	char *res=malloc(sizeof(char)*INFOS_LEN);
	for (i = 0; i < strlen(commande)-1; i++) {
    if (compteur==1) {
		*buf=commande[i];
		strcat(res,buf);
	 }
	 if (commande[i]==' ') {
		 compteur++;
	 }


	 }
  //free(buf);
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

int send_file(char user_cible[NICK_LEN],char user[NICK_LEN], int sockfd,int* fichier){

	struct message * message =malloc(sizeof(struct message));
	message=creatmsg(0,user,FILE_SEND,user_cible);
	if(send(sockfd, message, sizeof(struct message), 0)==-1){
		return -1;
	}
return 0;
}

int recv_file(struct message * file,char user[NICK_LEN], int sockfd){
	struct message * message =malloc(sizeof(struct message));
	message=creatmsg(0,user,FILE_ACK,message_get_nick_sender(file));
	if(send(sockfd, message, sizeof(struct message), 0)==-1){
		return -1;
	}
return 0;
}

int analyse_msg(struct message * message,char *pseudo,char* salon,int sockfd,char * bloque,int* fichier,struct list_noir* liste_noir){
  assert(message);
  char * buffer_char=malloc(MSG_LEN);
  char unknown[7]="unknown";
  char aucun[5]="aucun";
  memset(buffer_char,0,MSG_LEN);
    switch (message_get_type(message)) {
		case NICKNAME_WRONG:
		      printf("Ce pseudo est déjà utilisé par un autre utilisateur ! :( \n");
        break;
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
				if (is_in_list_noir(liste_noir,message_get_nick_sender(message))==0) {
					printf("(privé) %s : %s \n",message_get_nick_sender(message), buffer_char );
				}
                memset(buffer_char,0,MSG_LEN);
            }
        return 0;
        case BROADCAST_SEND:
            if (strncmp(pseudo,unknown,7)!=0 && recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
				if (strcmp(get_intru_name(liste_noir),"all")!=0) {
                	printf("%s : %s\n", message_get_nick_sender(message), buffer_char );
				}
                memset(buffer_char,0,MSG_LEN);
            }
        return 0;
        case INFO_SERVEUR:
            if (recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
                printf("serveur : %s\n",buffer_char );
                memset(buffer_char,0,MSG_LEN);
            }
        break;
        case MULTICAST_SEND:
        if (strncmp(salon,aucun,5)!=0){
            if (recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
              int a=strlen(salon);
              char *saloncast=malloc(a);
              memset(saloncast,0,a);
              saloncast=recupinfo2(salon);
                printf("[%s] %s : %s\n",saloncast, message_get_nick_sender(message), buffer_char );
                memset(buffer_char,0,MSG_LEN);
                //free(saloncast);
            }
            //free(buffer_char);
            return 0;
            }
        break;
        case SALON_JOIN_ACCEPT:
            printf(" ajouté au Salon  avec succès !\n");
            strcpy(salon,message_get_infos(message));
        return 0;
        case SALON_JOIN_REFUS:
            printf("Ce salon n'existe pas, /channel_list pour avoir la liste des salons.\n");
        return 0;
        case FILE_REQUEST:
			if (strncmp(bloque,unknown,7)!=0 && recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
				message=creatmsg(0,pseudo,FILE_REJECT,message_get_nick_sender(message));
				send(sockfd, message, sizeof(struct message), 0);
			}
			else if (is_in_list_noir(liste_noir,message_get_nick_sender(message))!=0 && recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
				message=creatmsg(0,pseudo,FILE_REJECT,message_get_nick_sender(message));
				send(sockfd, message, sizeof(struct message), 0);
			}
      else if (strncmp(pseudo,unknown,7)!=0 && recv(sockfd, buffer_char,message_get_playload(message),0)>0) {
				printf(" %s veux vous envoyer le fichier '%s' \n Accepter ? [Y/N]\n",message_get_nick_sender(message), buffer_char );
				memset(buffer_char,0,MSG_LEN);
				strcpy(bloque,message_get_nick_sender(message));
      }
        break;
		case FILE_SEND:
			recv_file(message,pseudo,sockfd);
		break;
        case FILE_ACCEPT:
			send_file(message_get_nick_sender(message),pseudo,sockfd,fichier);
        break;
        case FILE_REJECT:
            printf("%s à refusé votre demande de transmission de fichier ou n'est pas apte à la traiter pour le moment.\n", message_get_nick_sender(message));
			*fichier=0;
        break;
		case FILE_ACK:
			printf("%s à bien reçu votre fichier ! \n", message_get_nick_sender(message));
			*fichier=0;
		break;
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


char* verif_pseudo(char * commande){
    char unknown[7]="unknown";
    char serveur[7]="serveur";
    char server[7]="Serveur";
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
    if (strncmp(pseudo,unknown,7)==0 || strncmp(pseudo,serveur,7)==0 || strncmp(pseudo,server,7)==0) {
            printf("Vous n'avez pas le droit de vous appeler comme ça ! :( \n");
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


int lecture_shell(int sockfd,char *pseudo,char* salon,char *commande,char *bloque,int* fichier,struct list_noir * liste_noir){

	char nick[6]="/nick ";
    char who[4]="/who";
	char whois[7]="/whois ";
	char msgall[8]="/msgall ";
	char msg[5] = "/msg ";
	char quit[5]="/quit";
    char create[8]="/create ";
    char channel_list[13]="/channel_list";
    char join[6]="/join ";
	char bloquer[8]="/bloque ";
	char debloque[10]="/debloque ";
    char csend[6]="/send ";
    char unknown[7]="unknown";
    char aucun[5]="aucun";

  char * info=malloc(sizeof(char)*INFOS_LEN);
	char new_pseudo[NICK_LEN];
    char new_salon[NICK_LEN];
	memset(&new_pseudo,0,NICK_LEN);
	struct message* message=malloc(sizeof(struct message));
    //Condition si il est bloqué par un file request
    if ( (strncmp(bloque,unknown,7)!=0) && (strncmp(commande,"Y",1)!=0 && strncmp(commande,"y",1)!=0 && strncmp(commande,"yes",1)!=0 && strncmp(commande,"Y",1)!=0 && strncmp(commande,"N",1)!=0  && strncmp(commande,"n",1)!=0 && strncmp(commande,"no",1)!=0)){
        printf("Veillez faire un choix avant de continuer. Accepter ? [Y/N]\n");
        return 0;
    }
    if (( strncmp(commande,"Y",1)==0 || strncmp(commande,"y",1)==0 || strncmp(commande,"yes",1)==0) && (strncmp(pseudo,unknown,7)!=0 && strncmp(bloque,unknown,7)!=0)){
        message=creatmsg(0,pseudo,FILE_ACCEPT,bloque);
        send(sockfd, message, sizeof(struct message), 0);
		    strcpy(bloque,"unknown");
        return 0;
    }
    if (( strncmp(commande,"N",1)==0 || strncmp(commande,"n",1)==0 || strncmp(commande,"no",1)==0) && strncmp(pseudo,unknown,7)!=0 && (strncmp(bloque,unknown,7)!=0)){
        message=creatmsg(0,pseudo,FILE_REJECT,bloque);
        send(sockfd, message, sizeof(struct message), 0);
        strcpy(bloque,"unknown");
        return 0;
    }

    //Sinon
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
	else if (strncmp(commande,bloquer,8)==0 && strncmp(pseudo,unknown,7)!=0) {
		info=recupinfo2(commande);
		add_intru(liste_noir,info);
		printf("%s à été ajouté à la liste noir\n",info);
	}
	else if (strncmp(commande,bloquer,7)==0 && strncmp(pseudo,unknown,7)!=0) {
		liste_noir->intru="all";
		printf("Les messages all seront bloqué\n");
	}
	else if (strncmp(commande,debloque,10)==0 && strncmp(pseudo,unknown,7)!=0) {
		info=recupinfo2(commande);
		rm_intru(liste_noir,info);
		printf("%s à été retiré de la liste noir\n",info);
	}
	else if (strncmp(commande,debloque,9)==0 && strncmp(pseudo,unknown,7)!=0) {
		liste_noir->intru="aucun";
		printf("Les messages all seront à nouveau reçu\n");
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
    else if (strncmp(commande,csend,6)==0 && strncmp(pseudo,unknown,7)!=0){
		if (*fichier!=0) {
			printf("Vous ne pouvez envoyer qu'un fichier à la fois. Veuillez attendre la fin du traitement de la requet précédente\n");
		}
        info=recupinfo2(commande);
        printf("%s\n",info);
        char * msg_char=recup_message(commande,2);
    		FILE * fp=fopen(msg_char,"r");
        if (fp==NULL) {
          printf("Le fichier n'existe pas. \n");
        }
    		*fichier=1;
        message=creatmsg(strlen(msg_char),pseudo,FILE_REQUEST,info);
        send(sockfd, message, sizeof(struct message), 0);
        send(sockfd, msg_char, strlen(msg_char), 0);
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
    char *bloque=malloc(sizeof(char)*NICK_LEN);
    strcpy(bloque,"unknown");
	char *pseudo=malloc(sizeof(char)*NICK_LEN);
	strcpy(pseudo,"unknown");
    char *salon=malloc(sizeof(char)*NICK_LEN);
    strcpy(salon,"aucun");
	int *fichier=malloc(sizeof(int));
	*fichier=0;
	struct list_noir* liste_noir=malloc(sizeof(struct list_noir));
	liste_noir->intru="aucun";
	liste_noir->next=NULL;
	struct pollfd fds[MAXLINE];

	memset(&buffer_struct,0,sizeof(struct message));
	memset(SendMessage,0,MAXLINE);

	//POLL
	memset(fds , 0, MAXLINE*sizeof(struct pollfd));
	fds[0].fd = sockfd ;
	fds[1].fd = 0;
    fds[1].events=POLLIN;
	fds[0].events = POLLIN;
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
                        analyse_msg(&buffer_struct,pseudo,salon,sockfd,bloque,fichier,liste_noir);
					}
            else if (recv(sockfd, &buffer_struct,sizeof(struct message),0)==0) {
                        printf("Le serveur s'est fermé.\n");
                        free(SendMessage);
                      	close(sockfd);
                        int j;
                      	for(j=0;j<nfds;j++){
                      		close(fds[j].fd);
                      	}
                      	return 0 ;

					}
				}
			else {
				if( fds[i].fd == STDIN_FILENO) {
                    printf(" *** \n");
                    read(0,commande,MSG_LEN);
                    quit=lecture_shell(sockfd,pseudo,salon,commande,bloque,fichier,liste_noir);
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
