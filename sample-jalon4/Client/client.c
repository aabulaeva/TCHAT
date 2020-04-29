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
#include "connection_tool_client.h"
#include "salons.h"

#define MSG_LEN 1024

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
	int co=1;

  //On le coince ici tant qu'il n'a pas de pseudo !
	sfd = connection_avec_le_serveur(serv_addr,serv_port);

  while(co==1){
        co=communication_avec_le_serveur(sfd);
	}
  printf("Au reeeevoiiiiir !\n");
	close(sfd);
	return EXIT_SUCCESS;

}
