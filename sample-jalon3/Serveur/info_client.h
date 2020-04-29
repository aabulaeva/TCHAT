#ifndef INFO_CLIENT_H_
#define INFO_CLIENT_H_

struct client{
  char *username;
  char *salon;
  int sock;
  int port;
  time_t timeconnect;
  struct sockaddr *adress;
  int playload; //Saving data for when the char* msg will be recived.
  char* infos; //Saving data for when the char* msg will be recived.
  enum msg_type type; //Saving data for when the char* msg will be recived.
  struct client* next;
};

enum msg_err {
	DEST_UNKNOW,
	SEND_IMPOSSIBLE
};

//Send clients msg if error.
int send_client_error(int fd, enum msg_err type);

//Le nom est suffisant pour comprendre je pense..
struct client * chererche_client_par_descipteur(int fd,struct client* fir);
struct client * chererche_client_par_pseudo(char* pseudo,struct client* fir);

//Pour entrer les info du clients.
struct client * add_to_clients(int sock,struct sockaddr *adress,int port,struct client* fir);
void erase_client(int fd,struct client* fir);

//Sets
void set_playload_client(struct client* client,int playload);
void set_infos_client(struct client* client,char * infos);
void set_msgtype_client(struct client* client,enum msg_type type);
struct client* set_username(struct client* client,char * username);
void client_set_salon(struct client* client,char * salon);
char* set_user_salon(struct client* client,char * name);

// gets
int get_playload_client(struct client* client);
char* get_infos_client(struct client* client);
enum msg_type get_msgtype_client(struct client* client);
char* get_username(struct client* client);
char* get_user_salon(struct client* client);
int get_fd(struct client* client);
char * set_user_salon(struct client* client, char *salon);

//Fonction autres
int check_name_available(char* username,struct client* fir);

//Messages à envoeyer au clients (en provenance du serveur)
//Name ack/refuse
int ask_username(int fd);
int send_user_name_ack(struct client* client,int fd);
//Who and whois
int send_list_online_user(struct client* clients,int fd);
int send_user_info(struct client * clients,int fd,char* pseudo);
//nom salon ack and refuse
int send_salon_name_wrong(int fd);
int send_salon_name_ack(struct client* client,int fd);
int send_add_salon_ack(int fd,char * info);
int send_salon_wrong(int fd);

//Enregister les infos struct en attendant le msg char*
void save_infos(struct client * client, struct message* message);

//Transmission des messages
int send_to_dest(struct client * client, char* message,struct client * clients);


#endif /* INFO_CLIENT */
