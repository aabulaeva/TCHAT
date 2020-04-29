#ifndef INFO_CLIENT_H_
#define INFO_CLIENT_H_

struct client{
  int sock;
  int next_playload;
  char username[NICK_LEN];
  struct sockaddr *adress;
  int port;
  struct client* next;
};

//Le nom est suffisant pour comprendre je pense..
struct client * chererche_client_par_descipteur(int fd,struct client* fir);

//Pour entrer les info du clients.
struct client * chain(int sock,struct sockaddr *adress,int port,struct client* fir);

//demande au client son username
int askusername(int fd);

//Enregistre le username.
int set_username(int fd,struct client* fir,char username[128]);


#endif /* INFO_CLIENT */
