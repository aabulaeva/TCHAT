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


#include "msg_struct.h"
#include "info_client.h"
#include "connection_tool.h"



int main(int argc,char **argv){

    /* Check program parameters */
    if(argc != 2){
        printf("Argument Invalide ! \nAgument à fournir : ./programe_name #PORT\n");
        exit(EXIT_FAILURE);
    }
  char *port=argv[1];

  //socket bind
  int sock=handle_bind(port);

  //listen
  if (listen(sock,SOMAXCONN)==-1) {
    perror("ecoute de la socket\n");
    return 1;
  }

  //poll
  printf("Multicatiting en cour... \n");
  multicast(sock,atoi(port));

  //close

  close(sock);


  return 0;
}
