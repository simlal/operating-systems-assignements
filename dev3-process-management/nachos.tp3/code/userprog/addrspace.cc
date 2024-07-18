// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include "bitmap.h"
#include "syscall.h"

static void SwapHeader (NoffHeader *);
static int nextFreePhysicalPage = 0;

#define VirtualAddress int
extern void SysCallRead(OpenFileId fileSource, VirtualAddress userBufferDestination, int size, int position);
extern void SysCallYield();

//IFT320: C'est ici que ca se passe!
//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------
AddrSpace::AddrSpace(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int size;

	//Read the executable header.
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    
	//The header data might need little to big endian conversion
	if ((noffH.noffMagic != NOFFMAGIC) && (WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
	
	//The file is not a nachos executable if this fails (magic number isn't present).
    ASSERT(noffH.noffMagic == NOFFMAGIC);

	// Calculate address space total size (code + rodata + bss + stack)
	// Size is increased by a constant (UserStackSize) to leave room for the stack.
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;	
	
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
	
    int startPageIndex = AllocatePhysicalPages();
	// printf("startPageIndex: %d\n", startPageIndex);
	if (startPageIndex == -1)
	{
		printf("Not enough physical memory to allocate pages for the program\n");
		ASSERT(FALSE);
	}
	
	// Fails if the program size is larger than physical memory.
	// Shouldn't matter once virtual memory is implemented.
	ASSERT(numPages <= NumPhysPages);		

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", numPages, size);

	
	// Set up the page table to translate virtual addresses to physical addresses
    pageTable = new TranslationEntry[numPages];
    for (unsigned int i = 0; i < numPages; i++) {
		pageTable[i].virtualPage = i;	
		pageTable[i].physicalPage = startPageIndex + i;
		pageTable[i].valid = TRUE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		// if the code segment was entirely on a separate page, we could set its 
		// pages to be read-only
		pageTable[i].readOnly = FALSE;
    }
	// PrintPageTable();
	
	// Assign the page table in the machine with the one we just created
	if (currentThread->space == this)
	{
		printf("Setting page table for current thread\n");
		this->RestoreState();
	}
	else 
	{
		this->RestoreState();
	}
	// zero out the page in user space memory
	for (int i = 0; i < numPages; i++)
	{
		int vaddrStart = i * PageSize;
		for (int j = 0; j < PageSize; j++)
		{
			bool success = machine->WriteMem(vaddrStart + j, 1, 0);
			if (!success)
			{
				printf("Failed to write to memory at address %d\n", vaddrStart + j);
				ASSERT(FALSE);
			}
		}
	}

	if (noffH.code.size > 0) 
	{
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
		// printf("Reading code seg from pos=%d at vaddr=%d of size %d\n", noffH.code.inFileAddr, noffH.code.virtualAddr, noffH.code.size);
		SysCallRead(reinterpret_cast<OpenFileId>(executable), noffH.code.virtualAddr, noffH.code.size, noffH.code.inFileAddr);
    }
	else
	{
		DEBUG('a', "No code segment to initialize\n");
	}

    if (noffH.initData.size > 0) 
	{
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
		// printf("Reading data seg from pos=%d at vaddr=%d of size %d\n", noffH.initData.inFileAddr, noffH.initData.virtualAddr, noffH.initData.size);
		SysCallRead(reinterpret_cast<OpenFileId>(executable), noffH.initData.virtualAddr, noffH.initData.size, noffH.initData.inFileAddr);
    }
	else
	{
		DEBUG('a', "No data segment to initialize\n");
	}

	// Make the process yield cpu so that other processes can run
	SysCallYield();
}

void AddrSpace::PrintPageTable(){

	DEBUG('l',"Page table contents for thread %s:0x%x\n",currentThread->getName(),(int)currentThread);
	DEBUG('l',"\nVirtual | Physical\n");
	for (unsigned int i = 0; i < numPages; i++) {		
		DEBUG('l',"%d | %d\n",pageTable[i].virtualPage,pageTable[i].physicalPage);		
    }
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}


//----------------------------------------------------------------------
// AddrSpace::AllocatePhysicalPages
//
// Allocate a number of physical pages based on the number of pages
// required by the userprocess. Returns the startpage index if enough
// pages free are found otherwise -1
//----------------------------------------------------------------------
int AddrSpace::AllocatePhysicalPages()
{
	int startPage = nextFreePhysicalPage;

	// Move 
	if (startPage + numPages <= NumPhysPages)
	{
		nextFreePhysicalPage += numPages;
		return startPage;
	}
	else
	{
		return -1;
	}
}
