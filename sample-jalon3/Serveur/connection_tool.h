#ifndef CONNECTION_TOOL_H_
#define CONNECTION_TOOL_H_

void multicast(int sock,int port);

int handle_bind(char *port);
int transmission_char(struct client *clients, struct client *client_courant,int fd);
#endif /* CONNECTION_TOOL */
