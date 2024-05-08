################################################################################
# Makefile
# Par Kristian Benoit et Nicolas Dufresne
#
# Utilisation:
# make               Compilation du module.
# make {help|h}      Afficher cette aide.
# make demo          Insertion du module rs232_demo dans le noyau. Et creation
#                    du fichier representant le device.
# make {insmod|ins}  Insertion du module dans le noyau. Et creation du fichier
#                    representant le device (./$(MODULE_NAME)).
# make {rmmod|rm}    Retrait du module du noyau.
# make {status|stat} Information sur l'etat du module.
# make run           Meme chose que faire "make run" dans le repertoire parent.
# make clean         Nettoyage du repertoire.
################################################################################

################################################################################
# Ces variables configure ce Makefile. Vous ne devriez pas avoir a modifier
# autre chose.
#
# MODULE_NAME:  Contient le nom du module a compiler.
# SOURCES:      Contient le des fichier qui compose $(MODULE_NAME)
# INSMOD_PARAM: Contient les parametre a passer au module.
# CFLAGS:       Contient les parametre de compilation qu'on veut passer a gcc.
 
MODULE_NAME   = rs232_tut
MODULE_PATH   = $(HOME)/dev
SOURCES       = rs232_module.c rs232_tp.c cbuffer.c
INSMOD_PARAM ?= #DEFAULT: irq=3 base_port=0x2F8
CFLAGS       += -DMODULE_NAME=\"$(MODULE_NAME)\" -D_DEBUG

################################################################################
# Ces 2 variables sont utilisees par le Makefile de Linux. Ils specifient les
# fichiers object et le nom du module pour ce dernier.
$(MODULE_NAME)-objs := $(patsubst %.c,%.o,$(SOURCES))
obj-m       := $(addsuffix .o,$(MODULE_NAME))

################################################################################
# Le reste du Makefile ne doit pas etre inclus si on est appelle
# par le Makefile de Linux.
ifeq ($(KERNELRELEASE),)

DEMO_NAME            = rs232_demo

HEADERS             := $(wildcard *.h)
MODULE              := $(addsuffix .ko,$(MODULE_NAME))

ALL_GENERATED       := $($(MODULE_NAME)-objs) $(MODULE) $(obj-m)
ALL_GENERATED       += $(addsuffix .mod.o,$(MODULE_NAME))
ALL_GENERATED       += built-in.o
ALL_GENERATED       += $(addprefix .,$(addsuffix .cmd,$(ALL_GENERATED)))
ALL_GENERATED       += $(addsuffix .mod.c,$(MODULE_NAME))
ALL_GENERATED	    += $(MODULE_PATH)/$(MODULE_NAME)
ALL_GENERATED       += .tmp_versions

# Do not print entering directory as it get confusing...
MAKEFLAGS           += --no-print-directory

LOADED              := $(shell lsmod | sed -n -e 's/.*\<rs232_demo\>.*/DEMO/p' -e 's/.*\<rs232_tut\>.*/YES/p')

ifeq ($(LOADED),YES)
	CDEV_MAJOR          := $(shell sed -e 's/:.*//' /sys/module/$(MODULE_NAME)/dev)
	CDEV_MINOR          := $(shell sed -e 's/.*://' /sys/module/$(MODULE_NAME)/dev)
	IRQ                 := $(shell cat /sys/module/$(MODULE_NAME)/parameters/irq)
	BASE_PORT           := 0x$(shell printf "obase=16\n`cat /sys/module/$(MODULE_NAME)/parameters/base_port`\n" | bc)
else
ifeq ($(LOADED),DEMO)
	CDEV_MAJOR          := $(shell sed -e 's/:.*//' /sys/module/$(DEMO_NAME)/dev)
	CDEV_MINOR          := $(shell sed -e 's/.*://' /sys/module/$(DEMO_NAME)/dev)
	IRQ                 := $(shell cat /sys/module/$(DEMO_NAME)/parameters/irq)
	BASE_PORT           := 0x$(shell printf "obase=16\n`cat /sys/module/$(DEMO_NAME)/parameters/base_port`\n" | bc)
else # if nothing LOADED
	LOADED               = NO
	CDEV_MAJOR          := NONE
	CDEV_MINOR          := NONE
	IRQ                 := NONE
	BASE_PORT           := NONE
endif # LOADED
endif # LOADED

KERNEL_DIR  ?=/lib/modules/$(shell uname -r)/build
PATH :=$(PATH):/sbin

# Cette regle demande au Makefile de Linux de compiler notre module.
$(MODULE): $(SOURCES) $(HEADERS)
	@make -C $(KERNEL_DIR) SUBDIRS=$(CURDIR)

# Cette regle supprime tous les fichiers cree.
.PHONY: clean
clean: rmmod
	rm -rf $(ALL_GENERATED) 

# gnumake evalue les variables d'une section avant de l'executer. Ce qui
# nous oblige a separer le insmod et la creation du node en 2 regles.
# Celle-ci sert a la creation du node.
$(MODULE_PATH)/%:
	mknod $@ -m a+rw c $(CDEV_MAJOR) $(CDEV_MINOR)

# Cette regle charge le module dans le noyau et rappelle make pour creer le
# character device.
.PHONY: insmod ins 
insmod ins: $(MODULE) rmmod
	insmod $< $(INSMOD_PARAM)
	@make $(MODULE_PATH)/$(MODULE_NAME)

# Cette regle retire le module du noyau et supprime le character device.
.PHONY: rmmod rm
rmmod rm:
ifeq ($(LOADED),YES)
	rmmod $(MODULE_NAME) || true
endif #LOADED == YES
ifeq ($(LOADED),DEMO)
	rmmod $(DEMO_NAME) || true
endif #LOADED == DEMO
	rm -f $(MODULE_PATH)/$(MODULE_NAME) $(MODULE_PATH)/$(DEMO_NAME)

# Regle pour charger le module de demo.
.PHONY: demo
demo: rmmod
	modprobe $(DEMO_NAME)
	@make $(MODULE_PATH)/$(DEMO_NAME)
	
# Cette regle permet de visualiser l'etat du module.
.PHONY: status stat
status stat:
	@modinfo $(MODULE)
	@echo "Loaded:         $(LOADED)"
	@echo "major number:   $(CDEV_MAJOR)"
	@echo "irq:            $(IRQ)"
	@echo "base_port:      $(BASE_PORT)"

.PHONY: run
run:
	@make -C .. $(MAKECMDGOALS)

.PHONY: h help
h help:
	@echo Comment utiliser ce Makefile:
	@echo
	@echo "make               Compilation du module."
	@echo "make {help|h}      Afficher cette aide."
	@echo "make demo          Insertion du module rs232_demo dans le noyau. Et creation du"
	@echo "                   fichier representant le device."
	@echo "make {insmod|ins}  Insertion du module dans le noyau. Et creation du fichier"
	@echo "                   representant le device."
	@echo "make {rmmod|rm}    Retrait du module du noyau."
	@echo "make {status|stat} Information sur l'etat du module."
	@echo "make run           Meme chose que faire \"make run\" dans le repertoire parent."
	@echo "make clean         Nettoyage du repertoire."

endif # Pas appeller par le Makefile de Linux.
