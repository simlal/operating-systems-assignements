// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "noff.h"
#include <queue>


#define UserStackSize		1024 	// increase this as necessary!



class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 
	void PrintPageTable();
	void PrintPage(int,int);
  bool LoadFromExecutable(int badVaddr);
  char* GenerateSwapFilename(int exeId);
  int SwapOut();
  void SwapIn(int pageNumber, int frame);

	
	
  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    int numPages;		// Number of pages in the virtual 
					// address space
    NoffHeader noffH;  // file header tied to executable passed 
          //to addrspace constructor
    OpenFile* executable;
    FILE* swapFile;
    std::queue<int> loadedPages;
	
};

#endif // ADDRSPACE_H
