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
#include "salons.h"



char* verif_salon_name(char * commande){
    char serveur[7]="serveur";
	char create[7]="/create";
	int res;
	int i;
	int lencreate=7;
	char *nom_salon=malloc(sizeof(char)*NICK_LEN);
    char *buf=malloc(sizeof(char));
	int len=strlen(commande)-1;

	if ((res=strncmp(commande,create,7))==0) {
		if (lencreate==len || lencreate+1==len) {
			printf("Nom non valide, le nom de doit contenir ni lettres ni chiffres ni espaces\n");
			return NULL;
		}
		for (i = lencreate+1; i < len ; i++) {
			if ((commande[i]>47 && commande[i]<58) || (commande[i]<123 && commande[i] >96) || (commande[i]<91 && commande[i]>64) )  {
				*buf=commande[i];
				strncat(nom_salon,buf,1);
			}
			else{
				printf("Nom non valide, le nom de doit contenir ni lettres ni chiffres ni espaces\n");
				return NULL;
			}
			}
    if (strncmp(nom_salon,serveur,7)==0) {
      printf("Nom non valide, le nom de doit contenir ni lettres ni chiffres ni espaces\n");
      return NULL;
    }
		// struct message* message=creatmsg(sfd,0,nom_salon,NICKNAME_NEW,NULL,0);
		// send(sfd, message, sizeof(struct message*), 0);
		// printf("Vous avez choisi le nom_salon %s... Nous allons vérifier si il est disponible\n",message_get_nick_sender(message));
		return nom_salon;
		}
	else{
		return NULL;
	}
}
