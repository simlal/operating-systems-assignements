// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

// Hello2

//IFT320: Quelques definitions utiles.

//Cette fonction est definie dans progtest.cc. Sera utile ici, wink-wink.
extern void StartProcess(char *filename);

//Union utilitaire pour travailler avec les mots provenant du mips simule.
//ReadMem a besoin d'un espace de 4 octets pour faire sa lecture (adresse de int).
//Cependant, on peut vouloir aller chercher chaque octet du groupe de 4 individuellement.
union Word32{
 int intVal;
 char charVal[4];
};


#define VirtualAddress int
#define KernelAddress char*
#define MAX_FILENAME_LENGTH 100


//Mise a jour du compteur ordinal du mips simule.
//Doit etre fait avant de terminer ExceptionHandler, si on veut que le processus qui a
//lance l'exception continue normalement. 
void incrementPC();

//IFT320: Declarations de fonctions pour les appels systeme.
//Ces fonctions seront appelees par ExceptionHandler.
//L'ordre dans lequel sont presentees les declarations est l'ordre dans lequel
//il vous est suggere d'implanter les fonctions.

//Ce qui appellera ces fonction est ExceptionHandler, definie plus bas. C'est la routine
//d'interruption pour l'instruction SysCall, ainsi que toutes les erreurs de programmes.
//Allez la lire, MAINTENANT.


//1-Exit, parametre = code de sortie de fin du programme (0 signale un deroulement normal).
//Verifiable avec la commande texit
void SysCallExit(int code);

//2-CopyFromUser, taille connue. Recupere les donnees dans l'espace d'adresses du processus. 
//processus ----> Noyau
void CopyFromUser(VirtualAddress userBufferSource, KernelAddress kernelBufferDestination,int size);

//3-Log Write. Ecriture dans le journal noyau (DEBUG 'l'), utile pour les algorithmes de planification.
//Verifiable avec la commande tlog
void SysCallLogWrite(VirtualAddress userBufferSource, int size);

//4-CopyFromUser, taille inconnue. Recupere une chaine de caracteres dans l'espace d'adresses du processus
//processus ----> Noyau
void CopyFromUser(VirtualAddress userBufferSource, KernelAddress kernelBufferDestination);

//5-Create, creation de fichier via fileSystem->Create. VERSION FILESYS_STUB!!
//Verifiable avec la commande tcreate
void SysCallCreate(VirtualAddress fileName);

//6-Open, ouverture de fichier via fileSystem->Open. VERSION FILESYS_STUB!! Pas de table de fichiers ouverts.
OpenFileId SysCallOpen(VirtualAddress fileName);

//7-Write, ecriture dans un fichier via Openfile->write.
//Verifiable avec la commande twrite
void SysCallWrite(OpenFileId fileDestination,VirtualAddress userBufferSource, int size);

//8-CopyToUser, taille connue. Ecrit copie les donnees du noyau vers le processus.
//Noyau ----> processus
void CopyToUser(KernelAddress kernelBufferSource,VirtualAddress userBufferDestination,int size);

//9-Read, lecture dans un fichier via Openfile->read.
//Verifiable avec la commande tread
//Verifiable avec la commande treadwrite
void SysCallRead(OpenFileId fileSource,VirtualAddress userBufferDestination, int size);

//10-Exec, ouverture d'un executable et chargement d'un processus dans un nouvel espace d'adresses
//Verifiable avec la commande texec
SpaceId SysCallExec(VirtualAddress executableName,int initialPriority);

//11- ALLEZ FAIRE UN TOUR DANS ADDRSPACE.
//L'heure a sonne d'apprendre comment les programmes sont charges dans le mips simule.

//12- Yield, permet a un processus de lacher volontairement l'UCT et de retourner dans la file
//des processus prets. Utile pour verifier que vous avez effectivement reussi a charger plusieurs
//programmes dans la memoire du mips simule.
//Verifiable avec les commandes tspace, tyield et tfull
void SysCallYield();

//13-Join, attente de la fin d'un processus. Le processus qui demande le Join recoit le code de sortie
//du processus enfant pour lequel il demande Join.
//Verifiable avec les commandes tminijoin et tjoin
int SysCallJoin(SpaceId id);

//14-C'est le moment d'aller faire les algorithmes de planification!
//Allez faire un tour dans Thread, Scheduler et Main.
//Les algorithmes sont testables avec les commandes suivantes:
//	FCFS: texec, tyield, tspace, tminijoin, tjoin
//  Round-Robin: trobin
//	Priorite statique: tprio
//  HRN: thrn


//----->>> Par ici les amis, on sert des Nachos jusqu'a 4h du matin dans cette fonction.
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------
// #include "machine.h"
// Machine* machine;

void ExceptionHandler(ExceptionType which)
{
    

	VirtualAddress name;
	VirtualAddress userbuffer;
	int size;
	OpenFileId handle;
	
    switch(which)
	{
		case SyscallException:
		{
			// Get the SC code
			int type = machine->ReadRegister(2);
			
			switch(type)
			{
				case SC_Halt:
				{
					printf("Shutdown, initiated by user program.\n");
					interrupt->Halt();
					break;
				}			
				case SC_Exit: 
				{
					int exitCode = machine->ReadRegister(4);
					SysCallExit(exitCode);
					break;
				}
				default:
				{
					printf("Unrecognized Syscall type: %d.\n",type);				
					ASSERT(FALSE);
				}
			}
		}
		// Deal with non syscall exeptions
		case PageFaultException:
		{
			printf("Fatal Error: PageFaultException (ExceptionType=%d). Exiting...\n", which);
			SysCallExit(-1);
			break;
		}
		// Shutdown memory related errors
		case ReadOnlyException:
		case BusErrorException:
		case AddressErrorException:
		{
			printf("Illegal Memory Access: ExceptionType=%d.\n", which);
			SysCallExit(-1);
			break;
		}
		case OverflowException:
		{
			printf("Overflow Error: ExceptionType=%d. Be careful!\n", which);
			break;
		}
		case IllegalInstrException:
		{
			printf("Illegal Instruction: ExceptionType=%d.\n", which);
			SysCallExit(-1);
			break;
		}
		case NoException:
		{
			printf("Fatal Error, probable memory corruption (ExceptionType=%d).\n", which);
			SysCallExit(-1);
			break;
		}
		default:
		{
			printf("Unexpected user mode exception %d\n", which);
			ASSERT(FALSE);			
		}
    } 
}



void incrementPC()
{
	int pc;
	
	pc=machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg,pc);
	pc = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg,pc);
	pc+=4;
	machine->WriteRegister(NextPCReg,pc);
}




void SysCallExit(int code){

	printf("Exiting process with exit code: %d\n", code);
	currentThread->Finish();
}



void CopyFromUser(VirtualAddress userBufferSource, KernelAddress kernelBufferDestination,int size)
{	
	printf("Unimplemented function...");
	ASSERT(FALSE);
}


void SysCallLogWrite(VirtualAddress userBufferSource, int size){

	printf("Unimplemented system call...");
	ASSERT(FALSE);

}


void CopyFromUser(VirtualAddress userBufferSource, KernelAddress kernelBufferDestination){

	printf("Unimplemented function...");
	ASSERT(FALSE);
}


void SysCallCreate(VirtualAddress fileName){
	
	printf("Unimplemented system call...");
	ASSERT(FALSE);

}


OpenFileId SysCallOpen(VirtualAddress fileName){

	printf("Unimplemented system call...");
	ASSERT(FALSE);
}


void SysCallWrite(OpenFileId fileDestination,VirtualAddress userBufferSource, int size){

	printf("Unimplemented system call...");
	ASSERT(FALSE);

}


void CopyToUser(KernelAddress kernelBufferSource,VirtualAddress userBufferDestination,int size){
		
	printf("Unimplemented function...");
	ASSERT(FALSE);
}


void SysCallRead(OpenFileId fileSource,VirtualAddress userBufferDestination, int size){
		
	printf("Unimplemented system call...");
	ASSERT(FALSE);
}

SpaceId SysCallExec(VirtualAddress executableName,int initialPriority){
	
	printf("Unimplemented system call...");
	ASSERT(FALSE);
}

void SysCallYield(){
	printf("Unimplemented system call...");
	ASSERT(FALSE);
}

int SysCallJoin(SpaceId id){
	
	printf("Unimplemented system call...");
	ASSERT(FALSE);
}


