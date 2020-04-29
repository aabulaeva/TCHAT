#ifndef GESTIONSALON_H_
#define GESTIONSALON_H_

struct pseudo{
  char *username;
  struct pseudo* next;
};
struct salon{
  char *salonname;
  int nb_part;
  struct pseudo* premier;
  struct salon* next;
};

// sets
void set_salon_name( struct salon* salon,char * info);
void inc_salon_part( struct salon* salon);
void dec_salon_part( struct salon* salon);
void set_salon_fistusr( struct salon* salon,struct client* client);
void set_salon_creator(struct pseudo* pseudo,struct client* client);

// gets
struct pseudo* get_salon_creator(struct salon* salon);
char* get_salon_name(struct salon* salon);
int get_salon_part( struct salon* salon);



// salon managment
void add_salon_part(struct salon* salon,struct client* client);
int joinsalon(struct client* client, struct message* message,struct salon* salons);

struct salon* creatsalon(struct client *client,struct message* message,struct salon* salons);

//Verif du nom
int check_name_salon_available(char *nom,struct salon* salons);

int salon_empty(struct salon * salons,struct client* client);

//Envoie liste des salons
int send_list_salon(struct salon * salons,int fd);


#endif /* GESTIONSALON */
