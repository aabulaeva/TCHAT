CFLAGS=-Wall
#LDFLAGS=-lpthread
all: client

client : client.o connection_tool_client.o msg_struct.o
	gcc -o client client.o connection_tool_client.o msg_struct.o -Wall

connection_tool_client.o : connection_tool_client.c msg_struct.h
	gcc -o connection_tool_client.o -c connection_tool_client.c -Wall -O

msg_struct.o : msg_struct.c
	gcc -o msg_struct.o -c msg_struct.c -Wall -O
clean:
	rm -f client
