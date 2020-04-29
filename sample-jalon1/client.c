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

// #include "common.h"
#define MSG_LEN 1024

// Fonction qui permet la communication avec le serveur.//
void communication_avec_le_serveur(int sockfd) {

	int i;
	int quit=0;
    char * buff=malloc(MSG_LEN); //Création du buffer.

	int len_quit=6;
	int comparaison;
	char quitter[len_quit];
	strcpy(quitter,"./quit");

	//Création du descipteur qui sera chargé de lire le message écrit dans
	// le terminal par le client et l'écrire dans la socket.
	struct pollfd fds[2];
	fds[0].fd=sockfd;
	fds[1].fd=0;
	fds[0].events=POLLIN;
	fds[1].events=POLLIN;

	//Envoie/Reception.
	while (quit==0) {

		poll(fds,2,-1);

		// Nettoyage de la mémoire.
		memset(buff, 0, MSG_LEN);

		for (i = 0; i < 2; i++) {
			if(fds[i].revents==POLLIN){
				printf("ACTION\n");

				//Recevoir
				if (fds[i].fd==sockfd) {
					printf("un message à été reçu\n", );
					// Reception d'un message.
					if (recv(sockfd, buff, MSG_LEN, 0) > 0) {
						break;
						printf("Received: %s", buff);
					}
					else{
						printf("ah non enfaite\n");
					}
				}

				//Envoyer
				else{
					// Getting message from client
					printf("Message: ");
		  	  		read(0,buff,MSG_LEN);
					// Si le client veut quitter.
					comparaison=strncmp(buff,quitter,len_quit);
					if(comparaison==0){
						quit=1;
						printf("La connection va se fermer\n ");
					}
					// Envoie du message (si le message n'est pas ./quit).
				else if(send(sockfd, buff, strlen(buff), 0) > 0) {
						printf("\n Message sent! \n");
					}

				}
		  	}
		}
	}

	free(buff);
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

int main(int argc, char * argv[]) {

	//On récupère l'addresse et le numéro de port.

	/* Check program parameters */
    if(argc != 3){
        printf("Argument Invalide ! \nAgument à fournir : ./programe_name @IP #PORT\n");
        exit(EXIT_FAILURE);
    }

	/* Récupération du numéro de port et de l'adresse du serveur */
	char * serv_addr= argv[1];
	char * serv_port= argv[2];
	printf("Connection a %s, port %s ... \n", serv_addr,serv_port);

	// Création de la socket.
	int sfd;
	sfd = connection_avec_le_serveur(serv_addr,serv_port);

	communication_avec_le_serveur(sfd);

	close(sfd);
	return EXIT_SUCCESS;

}
