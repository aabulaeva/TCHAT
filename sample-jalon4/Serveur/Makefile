CFLAGS=-Wall
#LDFLAGS=-lpthread
all: serveur

serveur : serveur.o connection_tool.o gestionsalon.o info_client.o msg_struct.o
	gcc -o serveur serveur.o connection_tool.o gestionsalon.o info_client.o msg_struct.o -Wall

connection_tool.o : connection_tool.c gestionsalon.h info_client.h msg_struct.h
	gcc -o connection_tool.o -c connection_tool.c -Wall -O

gestionsalon.o : gestionsalon.c info_client.h msg_struct.h
		gcc -o gestionsalon.o -c gestionsalon.c -Wall -O

info_client.o : info_client.c msg_struct.h
	gcc -o info_client.o -c info_client.c -Wall -O

msg_struct.o : msg_struct.c
	gcc -o msg_struct.o -c msg_struct.c -Wall -O
clean:
	rm -f serveur
