# chat/Makefile
# Ce fichier contient les regles pour contruire le programme chat. Le programme
# chat est un simple programme qui part 2 threads, une pour lire et une pour
# ecrire dans le fichier representant le peripherique.
#
# Vous pouvez modifier ces variables au besoin.
SRCS      = chat.c
EXE       = chat
CFLAGS   += -Wall -g
LDFLAGS  += -lpthread

DEMO_NODE = ~/dev/rs232_demo
NODE      = ~/dev/rs232_tut
OBJS     := $(patsubst %.c,%.o,$(SRCS))

# Edition des liens. La compilation se fait par les regles implicite de make.
$(EXE): $(OBJS)
	gcc $(LDFLAGS) $^ -o $@

.PHONY: clean run demo help

# Nettoyage.
clean:
	rm -f $(EXE) $(OBJS)

# Rouler le programme de chat.
run: $(EXE)
	./$(EXE) $(NODE)

# Rouler le pragramme de chat avec le demo_node.
demo: $(EXE)
	./$(EXE) $(DEMO_NODE)

# Affichage de l'aide.
help:
	@echo "Comment utiliser ce Makefile:"
	@echo
	@echo "make             Compilation du programme de chat."
	@echo "make run         Demarrage du chat avec le fichier $(NODE)."
	@echo "make demo        Demarrage du chat avec le fichier $(DEMO_NODE)."
	@echo "make clean       Suppression des fichiers creer par la compilation."
	@echo "make help        Affichage de cette aide."
