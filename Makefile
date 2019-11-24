client :
	gcc client.c utils.c -o client -std=gnu99

serveur :
	gcc serveur.c utils.c -o serveur -std=gnu99

all :
	gcc client.c utils.c -o client -std=gnu99
	gcc serveur.c utils.c -o serveur -std=gnu99

clean :
	rm -f /tmp/*.o
