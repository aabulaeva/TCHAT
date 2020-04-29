##########################################################################
##########################################################################
#######################   APP CLIENT/SERVEUR        ######################
##########################################################################
##########################################################################

                                 SOMMAIRE

I- Utilisation
II- Structure
III- Rapport d'erreur
IV- Fiche répartition du travail

--------------------------------------------------------------------------
I- Utilisation


Pour demarrer, lancer le serveur, et un/des clients(s).

                        ------------------------
Commande : 

COMMUN

- /nick (definir/redefinir le pseudo)
- /who  (liste des utilisateurs connecté)
- /whois (information sur un uilisateur)
- /msgall (envoyer un message à tous les utilisateurs)
- /msg (envoyer un message privé)
- /quit (quitter le chat. Si vous êtes dans un salon, quitter le salon uniquement)
- /create (cree un salon)
- /channel_list (liste des salons)
- /join (joindre un salon)
- /send envoye un fichié à un autre utilisateur

EXTRA

- /bloque (Si vous ne précisé aucun paramètre, bloque les /msgall entrants. Sinon, ajoute le pseudo mis en paramètre sur la liste noire)

- /debloque (Si aucun paramètre, débloque les /msgall entrant. Sinon, retir, le pseudo de la liste noire)

                        ------------------------

PSEUDO INTERDI :
Vous ne pouvez pas vous appeler "serveur", "Serveur" ou "unknown"
Ces noms sont réservés.

--------------------------------------------------------------------------
II- Structure du code

                        ------------------------

Côté serveur : 

3 bibliothèques 'fait maison' : connection_tool, info_client, gestionsalon. 

La boucle poll se trouve dans connection_tool. 

Le seveur stock les données sur les clients dans une liste chainée 'clients'
Il stock les données des salon dans une liste chainée salon.

                        ------------------------

Côté client : 

2 bibliothèques 'fait maison' : connection_tool_client, salon.

La boucle poll se trouve dans connection_tool_client. 

Plusieurs variable d'états sont utilisé :
	- liste chainée liste noire
	- bloque, si l'utilisateur est en cours de transfère de fichier
	- pseudo


--------------------------------------------------------------------------
III - Rapport d'erreur 



--------------------------------------------------------------------------
IV- Fiche répartition du travail

A l'exeption du jalon 1, nous avons fait la pluspart du travail ensemble.
Nous nous somme vus pendant les vacances pour travailler. 
Nous somme toute les deux très satifaite du travaille fournis par chacune.
Nous n'avons pas de plainte partuculière.
		

