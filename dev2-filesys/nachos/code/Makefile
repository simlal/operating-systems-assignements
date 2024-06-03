# Copyright (c) 1992 The Regents of the University of California.
# All rights reserved.  See copyright.h for copyright notice and limitation 
# of liability and disclaimer of warranty provisions.

LPR = lpr

# Si appelé par GNU Make (comme sur sirius), la ligne suivante ne fait rien.
# Sinon, on localise l'exécutable GNU Make. 
MAKE:sh = /bin/sh -c "if [ -f /opt/gnu/bin/make ]; then echo /opt/gnu/bin/make; elif [ -f /usr/local/bin/make ]; then echo /usr/local/bin/make; else echo make; fi"

all: 
	cd threads; $(MAKE) depend
	cd threads; $(MAKE) nachos
	cd userprog; $(MAKE) depend 
	cd userprog; $(MAKE) nachos 
	cd vm; $(MAKE) depend
	cd vm; $(MAKE) nachos 
	cd filesys; $(MAKE) depend
	cd filesys; $(MAKE) nachos 
	cd bin; $(MAKE) all
	cd test; $(MAKE) all

# don't delete executables in "test" in case there is no cross-compiler
clean:
	-/bin/csh -c "\rm -f */{core,nachos,DISK,*.o,swtch.s} test/{*.coff,halt,sort,shell,matmult} bin/{coff2flat,coff2noff,disassemble,out}"

print:
	/bin/csh -c "$(LPR) Makefile* */Makefile"
	/bin/csh -c "$(LPR) threads/*.h threads/*.cc threads/*.s"
	/bin/csh -c "$(LPR) userprog/*.h userprog/*.cc" 
	/bin/csh -c "$(LPR) filesys/*.h filesys/*.cc
	/bin/csh -c "$(LPR) machine/*.h machine/*.cc
	/bin/csh -c "$(LPR) bin/noff.h bin/coff.h bin/coff2noff.c"
	/bin/csh -c "$(LPR) test/*.h test/*.c test/*.s"
