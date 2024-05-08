# Un simple Makefile pour appeler les Makefiles des repertoires enfants.
.PHONY: all run demo clean help

MAKEFLAGS    := $(filter-out --no-print-directory,$(MAKEFLAGS))

# Compiler le tout.
all:
	@make -C driver
	@make -C chat

# Compiler, charger et rouler le chat.
run:
	@make -C driver insmod
	@make -C chat run

# Charger et rouler le demo.
demo:
	@make -C driver demo
	@make -C chat demo
	
# Supprimer tout les fichiers genere par la compilation.
clean:
	@make -C driver clean
	@make -C chat clean

# Affichier l'aide.
help:
	@echo "Comment utiliser ce Makefile:"
	@echo
	@echo "make             Compilation du module et programme de chat."
	@echo "make run         Chargement de votre module et demarrage du chat."
	@echo "make demo        Chargement du module rs232_demo et demarrage du chat."
	@echo "make clean       Nettoyage de votre module et du programme de chat."
	@echo "make help        Affichage de cette aide."
