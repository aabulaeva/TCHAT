#ifndef CONNECTION_TOOL_CLIENT_H_
#define CONNECTION_TOOL_CLIENT_H_

char *recupinfo(char *commande,int lencom,int cond);

// Fonction qui permet la communication avec le serveur.//
int communication_avec_le_serveur(int sockfd);

struct message* envoimsg(int sfd,int payload,char *pseudo,enum msg_type type,char * info);

char* unpseudo(int sfd);

struct message* commandes(int sfd,char *pseudo);

//Fonction qui creer la connection avec le serveur. Renvoie le decripteur associé.//
int connection_avec_le_serveur(char * serv_addr,char* serv_port);

#endif /* CONNECTION_TOOL_CLIENT */
