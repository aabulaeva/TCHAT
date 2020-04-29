#ifndef MSG_STRUCT_H_
#define MSG_STRUCT_H_

#define MSG_LEN 1024

#define NICK_LEN 128
#define INFOS_LEN 128

enum msg_type {
	NICKNAME_NEW,
	NICKNAME_LIST,
	NICKNAME_INFOS,
	NICKNAME_WRONG,
	NICKNAME_ACCEPT,
	ECHO_SEND,
	UNICAST_SEND,
	BROADCAST_SEND,
	INFO_SERVEUR,
	MULTICAST_CREATE,
	MULTICAST_LIST,
	MULTICAST_JOIN,
	MULTICAST_SEND,
	MULTICAST_QUIT,
	FILE_REQUEST,
	FILE_ACCEPT,
	FILE_REJECT,
	FILE_SEND,
	FILE_ACK,
	QUIT,

};

struct message {
	int pld_len;
	char nick_sender[NICK_LEN];
	enum msg_type type;
	char infos[INFOS_LEN];
};

//Not used for now...
// static char* msg_type_str[] = {
// 	"NICKNAME_NEW",
// 	"NICKNAME_LIST",
// 	"NICKNAME_INFOS",
// 	"ECHO_SEND",
// 	"UNICAST_SEND",
// 	"BROADCAST_SEND",
// 	"MULTICAST_CREATE",
// 	"MULTICAST_LIST",
// 	"MULTICAST_JOIN",
// 	"MULTICAST_SEND",
// 	"MULTICAST_QUIT",
// 	"FILE_REQUEST",
// 	"FILE_ACCEPT",
// 	"FILE_REJECT",
// 	"FILE_SEND",
// 	"FILE_ACK"
// };

//Init d'un msg
struct message* init_message_serveur(enum msg_type type, char info[INFOS_LEN],int playload);
struct message* init_message_transf(enum msg_type type, char info[INFOS_LEN],int playload,char *pseudo);

//gets and sets
enum msg_type message_get_type(struct message* message);
char * message_get_nick_sender(struct message* message);
char* message_get_infos(struct message* message);
int message_get_playload(struct message* message);
void message_set_infos(struct message* message, char info[INFOS_LEN]);
void message_set_type(struct message* message, enum msg_type type);
void message_set_playload(struct message* message, int playload);
void message_set_nick_sender(struct message* message,char nom[NICK_LEN]);


#endif /* MSG_STRUCT */
