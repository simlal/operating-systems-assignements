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

#ifdef USER_PROGRAM
#include "copy.h"
extern int SysCallRead(OpenFile*, int,int,int);
extern int SysCallWrite(OpenFile*, int,int,int);
#define IN_SWAP_FILE -2
#endif

static void SwapHeader (NoffHeader *);

extern BitMap* freeFrame;

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
    
    unsigned int size;
    // NoffHeader noffH;
    this->executable = executable;
    
    // Create a swap file for this executable and keep it open
    char* swapFileName = GenerateSwapFilename((int) executable);
    swapFile = fopen(swapFileName, "w+");
    if (swapFile == NULL)
    {
        printf("Could not create swap file for executable\n");
        delete[] swapFileName;
        currentThread->Finish();
    }
    printf("Opened swapfile: %s for executable\n", swapFileName);
    delete[] swapFileName;


		
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && (WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

	// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;		
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

	
	//IFT320: verifie si espace suffisant pour charger le programme.	
	
	
	DEBUG('a', "espace libre =  %d, \n", freeFrame->NumClear());
	// ASSERT(numPages <= freeFrame->NumClear());	
    


    DEBUG('a', "Initializing address space, num pages %d, size %d\n",numPages, size);

    pageTable = new TranslationEntry[numPages];
    for (int i = 0; i < numPages; i++) {
      pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
      pageTable[i].physicalPage = -1;		
      pageTable[i].valid = FALSE;	//Traduction valide ou non
      pageTable[i].use = FALSE;	//Utilisee dernierement (read ou write)
      pageTable[i].dirty = FALSE;	//Modifiee dernierement (write)
      pageTable[i].readOnly = FALSE;  //Interdite en ecriture ou non
    }

	//IFT320: on assume que le AddrSpace est construit par le Thread associe.
	//C'est-a-dire, PAS par le Thread qui demande l'exec de ce programme, mais
	//par celui qui a ete cree pour rouler ce programme.
	//Sinon, BIG BADA BOOM
	RestoreState();
	
	
	
	//IFT320: chargement du programme au complet.	
	// SysCallRead(executable,noffH.code.virtualAddr,noffH.code.size,noffH.code.inFileAddr);
	
	if(noffH.initData.size>0){
		// SysCallRead(executable,noffH.initData.virtualAddr,noffH.initData.size,noffH.initData.inFileAddr);		
	}

  
	
}




//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
	
	
	//IFT320: liberation des cadres
	int i;
	DEBUG('e',"Liberation des cadres...\n");
	for (i = 0; i < numPages; i++)
		
		if (pageTable[i].physicalPage != -1 && pageTable[i].physicalPage != IN_SWAP_FILE)
    {
    freeFrame->Clear(pageTable[i].physicalPage); //libere cadre 
    }

	delete pageTable;

  // Close the swap file
  fclose(swapFile);
  //TODO : delete swap file
  

}

void AddrSpace::PrintPageTable(){

	DEBUG('l',"Page table contents for thread %s:0x%x\n",currentThread->getName(),(int)currentThread);
	DEBUG('l',"\n V |   P   | v | u | d | ro\n");
	for (int i = 0; i < numPages; i++) {		
		DEBUG('l',"%2d | %5d | ",pageTable[i].virtualPage,pageTable[i].physicalPage);	
		DEBUG('l',"%d | %d | %d | %d\n",pageTable[i].valid,pageTable[i].use,pageTable[i].dirty,pageTable[i].readOnly);
    }
}

//IFT320: Affiche le contenu d'une page, octet par octet. 
//Le parametre "swapPos" sera utilise plus tard pour comparer le contenu avec ce qu'il y a dans 
//le fichier d'echanges. 
void AddrSpace::PrintPage(int page,int swapPos){

	DEBUG('a',"----Page %d contents----",page);
	unsigned char content[128];
	
	if(pageTable[page].valid==TRUE){
		DEBUG('a',"Valid, in frame %d:\n",pageTable[page].physicalPage);
		memcpy(content,&(machine->mainMemory[pageTable[page].physicalPage*PageSize]),128);
		for (int i=0;i<128;i++){
			DEBUG('a',"%2x ",content[i]);
		}
    DEBUG('a',"\n");
	}
	else{
		DEBUG('a',"Invalid, not in memory.\n");
	}	
	
	if(swapPos>=0){
		DEBUG('l',"\nPage in swap, occupies block %d:\n",swapPos);
		
		//IFT320: ajoutez ici l'affichage du contenu du swap correspondant a la page

    // Find the swap file offset
    fseek(swapFile, swapPos, SEEK_SET);
    fread(content, 128, 1, swapFile);
    for (int i=0;i<128;i++){
      DEBUG('l',"%2x ",content[i]);
    }    
    DEBUG('a',"\n");

	}
	else{
		DEBUG('l',"\nPage Not in swap. \n",swapPos);	
	}	
	
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
// Load an executable to correct the page fault
//----------------------------------------------------------------------
bool AddrSpace::LoadFromExecutable(int pageNumber)
{
    loadedPages.push(pageNumber);    // For victim choice in SwapOut
    
    // Check if the page is already loaded
    if (pageTable[pageNumber].valid)
    {
        printf("Page %d is already loaded\n", pageNumber);
        return TRUE;
    }

    // Find a free frame, use swaping if necessary
    int frame = freeFrame->Find();
    if (frame == -1)
    {
        // printf("No more frames available, swapping out...\n");
        // Swap out a page to make room
        frame = SwapOut();
        if (frame == -1)
        {
          printf("Failed to find a free frame after SwapOut\n");
          return FALSE;
        }
    }
    // Use swap file or load from executable
    if (pageTable[pageNumber].physicalPage == IN_SWAP_FILE)
    {
        SwapIn(pageNumber, frame);
    }
    else
    {
        // Only load the executable's portion based on offset/code
        int offset = pageNumber * PageSize;
        int exeSize = noffH.code.size + noffH.initData.size;
        if (offset >= exeSize)
        {
            printf("Page %d is not part of the executable. \n", pageNumber);
            bzero(&(machine->mainMemory[frame * PageSize]), PageSize);
        }
        else
        {
            int bytesRead = executable->ReadAt(&(machine->mainMemory[frame * PageSize]), PageSize, noffH.code.inFileAddr + offset);
            // printf("Read %d bytes from executable\n", bytesRead);
        }
        // Update the page table entry
        pageTable[pageNumber].virtualPage = pageNumber;
        pageTable[pageNumber].physicalPage = frame;
        pageTable[pageNumber].valid = TRUE;

    }
    printf("Page %d loaded into frame %d\n", pageNumber, frame);
    return TRUE;
  }

//----------------------------------------------------------------------
// GenerateSwapFilename
// Use a combination of threadname_OpenFileId.swp to generate the swap
// file name
//----------------------------------------------------------------------
char* AddrSpace::GenerateSwapFilename(int executableId)
{
  int lenSwapFileName;
  char* swapFileName = new char[1000];
  lenSwapFileName = sprintf(swapFileName, "%s_exe-%d.swp", currentThread->getName(), executableId);

  printf("SwapFileName: %s (len=%d)\n", swapFileName, lenSwapFileName);
  return swapFileName;
}

//----------------------------------------------------------------------
// SwapOut
// Choose a victim to invalidate from page table and
// copy its content to swapfile
//----------------------------------------------------------------------
int AddrSpace::SwapOut()
{
    if (loadedPages.empty())
    {
      printf("Loaded pages queue is empty\n");
      return -1;
    }

    // Find the first valid page to swap out
    int frame;
    bool found = FALSE;
    int pageIndex;
    
    for (int i = 0; i < loadedPages.size(); i++)
    {
      pageIndex = loadedPages.front();
      if (pageTable[pageIndex].valid)
      {
        found = TRUE;
        break;
      }
      loadedPages.pop();
      loadedPages.push(pageIndex);
    }
    if (!found)
    {
      printf("No valid page found in the queue\n");
      return -1;
    }
    
    // Save the frame and update the queue
    // printf("Swapping out page %d\n", pageIndex);
    // PrintPage(pageIndex, -1);
    loadedPages.pop();
    frame = pageTable[pageIndex].physicalPage;
    
    // Make full copy of exe into swapfile
    int swapFileOffset = pageIndex * PageSize;
    fseek(swapFile, swapFileOffset, SEEK_SET);
    fwrite(&(machine->mainMemory[frame * PageSize]), PageSize, 1, swapFile);

    // PrintPage(pageIndex, swapFileOffset);
    
    // Invalidate victim and return the free frame
    pageTable[pageIndex].valid = FALSE;
    pageTable[pageIndex].physicalPage = IN_SWAP_FILE;
    currentThread->stats->incSwapOuts();
    return frame;
}
//----------------------------------------------------------------------
// SwapIn
// Load the page from swapfile to the free frame
//----------------------------------------------------------------------
void AddrSpace::SwapIn(int pageNumber, int frame)
{
    // Find the swap file offset
    int swapFileOffset = pageNumber * PageSize;
    fseek(swapFile, swapFileOffset, SEEK_SET);
    fread(&(machine->mainMemory[frame * PageSize]), PageSize, 1, swapFile);
    // PrintPage(pageNumber, swapFileOffset);
    
    // Update the page table entry
    pageTable[pageNumber].physicalPage = frame;
    pageTable[pageNumber].valid = TRUE;
    
    // Remove the page from swap file
    char* zeroes = new char[PageSize];
    bzero(zeroes, PageSize);
    fseek(swapFile, swapFileOffset, SEEK_SET);
    fwrite(zeroes, PageSize, 1, swapFile);

    // Clean up and update stats
    // delete[] zeroes;
    currentThread->stats->incSwapIns();

    // printf("Swapped in page %d to frame %d\n", pageNumber, frame);
}

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
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
/*
                                             ,-.
                                          _.|  '
                                        .'  | /
                                      ,'    |'
                                     /      /
                       _..----""---.'      /
 _.....---------...,-""                  ,'
 `-._  \                                /
     `-.+_            __           ,--. .
          `-.._     .:  ).        (`--"| \
               7    | `" |         `...'  \
               |     `--'     '+"        ,". ,""-
               |   _...        .____     | |/    '
          _.   |  .    `.  '--"   /      `./     j
         \' `-.|  '     |   `.   /        /     /
         '     `-. `---"      `-"        /     /
          \       `.                  _,'     /
           \        `                        .
            \                                j
             \                              /
              `.                           .
                +                          \
                |                           L
                |                           |
                |  _ /,                     |
                | | L)'..                   |
                | .    | `                  |
                '  \'   L                   '
                 \  \   |                  j
                  `. `__'                 /
                _,.--.---........__      /
               ---.,'---`         |   -j"
                .-'  '....__      L    |
              ""--..    _,-'       \ l||
                  ,-'  .....------. `||'
               _,'                /
             ,'                  /
            '---------+-        /
                     /         /
                   .'         /
                 .'          /
               ,'           /
             _'....----""""" 
*/
