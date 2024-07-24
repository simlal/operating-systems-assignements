# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

include ../Makefile.dep

#CPPFLAGS = $(INCDIR)
INCDIR =-I../userprog -I../threads

#CC = $(MIPS_PREFIX)gcc
#AS = $(MIPS_PREFIX)as
#LD = $(MIPS_PREFIX)ld

#ifeq "$(LOCAL_ARCH)" "Linux"
#LDFLAGS = -T script_linux -N --cref -S -EL
#ASFLAGS = -mips1 -EL
#CFLAGS =-ggdb -mcpu=r3000 -mno-mips-tfile $(INCDIR)
#else
#LDFLAGS = -T script_sun -N
#ASFLAGS = 
#CFLAGS = -G 0 -c $(INCDIR)
#endif

GCCDIR = ../../../gnu-decstation-ultrix/decstation-ultrix/2.95.3/

LDFLAGS = -T script -N
ASFLAGS = -mips2
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)

PATH = $(GCCDIR):/lib:/usr/bin:/bin

CC = $(GCCDIR)gcc -B../../../gnu-decstation-ultrix/
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = gcc -E
#INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)



all: halt shell matmult sort mon_test mon_test1 mon_test2 test_err1 test_err2 test_err3


ifeq "$(LOCAL_ARCH)" "Linux"

.c.o:
	$(CC) $(CFLAGS) -S $< -o - | $(AS) $(ASFLAGS) - -o $@

#start.o: start.s ../userprog/syscall.h
#	$(CPP) -DTEST $(CPPFLAGS) $< -o str.s
#	$(AS) $(ASFLAGS) str.s -o $@
#	rm str.s
start.o: start.s ../userprog/syscall.h
        $(CPP) $(CPPFLAGS) start.c > strt.s
        $(AS) $(ASFLAGS) -o start.o strt.s
        rm strt.s

mon_test.o: mon_test.c
	$(CC) $(CFLAGS) -c mon_test.c
mon_test: mon_test.o start.o
	$(LD) $(LDFLAGS) start.o mon_test.o -o mon_test.coff
	../bin/coff2noff mon_test.coff mon_test

mon_test1.o: mon_test1.c
	$(CC) $(CFLAGS) -c mon_test1.c
mon_test1: mon_test1.o start.o
	$(LD) $(LDFLAGS) start.o mon_test1.o -o mon_test1.coff
	../bin/coff2noff mon_test1.coff mon_test1

mon_test2.o: mon_test2.c
	$(CC) $(CFLAGS) -c mon_test2.c
mon_test2: mon_test2.o start.o
	$(LD) $(LDFLAGS) start.o mon_test2.o -o mon_test2.coff
	../bin/coff2noff mon_test2.coff mon_test2

halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt
	
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult
	
else

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.c > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

mon_test.o: mon_test.c
	$(CC) $(CFLAGS) -c mon_test.c
mon_test: mon_test.o start.o
	$(LD) $(LDFLAGS) start.o mon_test.o -o mon_test.coff
	../bin/coff2noff mon_test.coff mon_test

mon_test1.o: mon_test1.c
	$(CC) $(CFLAGS) -c mon_test1.c
mon_test1: mon_test1.o start.o
	$(LD) $(LDFLAGS) start.o mon_test1.o -o mon_test1.coff
	../bin/coff2noff mon_test1.coff mon_test1

mon_test2.o: mon_test2.c
	$(CC) $(CFLAGS) -c mon_test2.c
mon_test2: mon_test2.o start.o
	$(LD) $(LDFLAGS) start.o mon_test2.o -o mon_test2.coff
	../bin/coff2noff mon_test2.coff mon_test2
shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

endif
