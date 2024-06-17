// filesys.cc 
//	Routines to manage the overall operation of the file system.
//	Implements routines to map from textual file names to files.
//
//	Each file in the file system has:
//	   A file header, stored in a sector on disk 
//		(the size of the file header data structure is arranged
//		to be precisely the size of 1 disk sector)
//	   A number of data blocks
//	   An entry in the file system directory
//
// 	The file system consists of several data structures:
//	   A bitmap of free disk sectors (cf. bitmap.h)
//	   A directory of file names and file headers
//
//      Both the bitmap and the directory are represented as normal
//	files.  Their file headers are located in specific sectors
//	(sector 0 and sector 1), so that the file system can find them 
//	on bootup.
//
//	The file system assumes that the bitmap and directory files are
//	kept "open" continuously while Nachos is running.
//
//	For those operations (such as Create, Remove) that modify the
//	directory and/or bitmap, if the operation succeeds, the changes
//	are written immediately back to disk (the two files are kept
//	open during all this time).  If the operation fails, and we have
//	modified part of the directory and/or bitmap, we simply discard
//	the changed version, without writing it back to disk.
//
// 	Our implementation at this point has the following restrictions:
//
//	   there is no synchronization for concurrent accesses
//	   files have a fixed size, set when the file is created
//	   files cannot be bigger than about 3KB in size
//	   there is no hierarchical directory structure, and only a limited
//	     number of files can be added to the system
//	   there is no attempt to make the system robust to failures
//	    (if Nachos exits in the middle of an operation that modifies
//	    the file system, it may corrupt the disk)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "disk.h"
#include "bitmap.h"
#include "directory.h"
#include "filehdr.h"
#include "filesys.h"
#include "system.h"

// Sectors containing the file headers for the bitmap of free sectors,
// and the directory of files.  These file headers are placed in well-known 
// sectors, so that they can be located on boot-up.
#define FreeMapSector 		0
#define DirectorySector 	1

// Initial file sizes for the bitmap and directory; until the file system
// supports extensible files, the directory size sets the maximum number 
// of files that can be loaded onto the disk.
#define FreeMapFileSize 	(NumSectors / BitsInByte)
#define NumDirEntries 		10
#define DirectoryFileSize 	(sizeof(DirectoryEntry) * NumDirEntries)


// Max size of the path of the current directory
#define PathMaxLen (NumSectors * (FileNameMaxLen + 1))  // Pour simplif 


//----------------------------------------------------------------------
// FileSystem::FileSystem
// 	Initialize the file system.  If format = TRUE, the disk has
//	nothing on it, and we need to initialize the disk to contain
//	an empty directory, and a bitmap of free sectors (with almost but
//	not all of the sectors marked as free).  
//
//	If format = FALSE, we just have to open the files
//	representing the bitmap and the directory.
//
//	"format" -- should we initialize the disk?
//----------------------------------------------------------------------
FileSystem::FileSystem(bool format)
{
    DEBUG('g', "Initializing the file system.\n");
    if (format)
    {
        BitMap *freeMap = new BitMap(NumSectors);
        Directory *directory = new Directory(NumDirEntries);
        FileHeader *mapHdr = new FileHeader;
        FileHeader *dirHdr = new FileHeader;

        DEBUG('g', "Formatting the file system.\n");

        // First, allocate space for FileHeaders for the directory and bitmap
        // (make sure no one else grabs these!)
        freeMap->Mark(FreeMapSector);
        freeMap->Mark(DirectorySector);

        // Second, allocate space for the data blocks containing the contents
        // of the directory and bitmap files.  There better be enough space!

        ASSERT(mapHdr->Allocate(freeMap, FreeMapFileSize));
        ASSERT(dirHdr->Allocate(freeMap, DirectoryFileSize));

        // Flush the bitmap and directory FileHeaders back to disk
        // We need to do this before we can "Open" the file, since open
        // reads the file header off of disk (and currently the disk has garbage
        // on it!).

        DEBUG('f', "Writing headers back to disk.\n");
        mapHdr->WriteBack(FreeMapSector);
        dirHdr->WriteBack(DirectorySector);

        // OK to open the bitmap and directory files now
        // The file system operations assume these two files are left open
        // while Nachos is running.

        freeMapFile = new OpenFile(FreeMapSector);
        directoryFile = new OpenFile(DirectorySector);

        // Once we have the files "open", we can write the initial version
        // of each file back to disk.  The directory at this point is completely
        // empty; but the bitmap has been changed to reflect the fact that
        // sectors on the disk have been allocated for the file headers and
        // to hold the file data for the directory and bitmap.

        DEBUG('f', "Writing bitmap and directory back to disk.\n");
        freeMap->WriteBack(freeMapFile); // flush changes to disk
        directory->WriteBack(directoryFile);

        if (DebugIsEnabled('f'))
        {
            freeMap->Print();
            directory->Print();

            delete freeMap;
            delete directory;
            delete mapHdr;
            delete dirHdr;
        }
    }
    else
    {
        // if we are not formatting the disk, just open the files representing
        // the bitmap and directory; these are left open while Nachos is running
        freeMapFile = new OpenFile(FreeMapSector);
        directoryFile = new OpenFile(DirectorySector);
    }
    // Set the current directory to root at the beginning
    cdSector = DirectorySector;

    // Create the OpenFileTable
    openFileTable = new OpenFileTable();
}

//----------------------------------------------------------------------
// FileHandle::FileHandle
// 	Constructor for FileHandle
//----------------------------------------------------------------------
FileHandle::FileHandle(char* name, bool writeable, OpenFile* file)
{
    this->name = name;
    this->writeable = writeable;
    this->file = file;
    this->position = 0; // start at the beginning of the file
    this->useCount = 1; // start with one user
}

//----------------------------------------------------------------------
// FileHandle::~FileHandle
// 	Destructor for FileHandle
//----------------------------------------------------------------------
FileHandle::~FileHandle()
{
    delete file;
}

//----------------------------------------------------------------------
// FileHandle::GetOpenFile
// Pointer to the OpenFile object
OpenFile* FileHandle::GetOpenFile()
{
    return file;
}

//----------------------------------------------------------------------
// FileHandle::GetSector
// 	Return the sector of the file
//----------------------------------------------------------------------
int FileHandle::GetSector()
{
    return sector;
}

//----------------------------------------------------------------------
// FileHandle::GetUseCount
// 	Return the useCount of the file
//----------------------------------------------------------------------
int FileHandle::GetUseCount()
{
    return useCount;
}
//----------------------------------------------------------------------
// FileHandle::SetUseCount
// 	Set the useCount of the file
//----------------------------------------------------------------------
int FileHandle::SetUseCount(int count)
{
    useCount = count;
}

//----------------------------------------------------------------------
// FileHandle::IncrementUseCount
// 	Increment the useCount of the file
//----------------------------------------------------------------------
void FileHandle::IncrementUseCount()
{
    useCount++;
}

//----------------------------------------------------------------------
// FileHandle::DecrementUseCount
// 	Decrement the useCount of the file
//----------------------------------------------------------------------
void FileHandle::DecrementUseCount()
{
    useCount--;
}

//----------------------------------------------------------------------
// FileHandle::IsWriteable
// 	Return the writeable status of the file
bool FileHandle::IsWriteable()
{
    return writeable;
}


//----------------------------------------------------------------------
// FileHandle::Read
// 	Read the file
//----------------------------------------------------------------------
int FileHandle::Read(char *into, int numBytes)
{
    return file->Read(into, numBytes);
}

//----------------------------------------------------------------------
// FileHandle::Write
// 	Write to the file
//----------------------------------------------------------------------
int FileHandle::Write(char *from, int numBytes)
{
    return file->Write(from, numBytes);
}

//----------------------------------------------------------------------
// FileHandle::ReadAt
//     Read the file at a specific position
//----------------------------------------------------------------------
int FileHandle::ReadAt(char *into, int numBytes, int position)
{
    return file->ReadAt(into, numBytes, position);
}

//----------------------------------------------------------------------
// FileHandle::WriteAt
//     Write to the file at a specific position
//----------------------------------------------------------------------
int FileHandle::WriteAt(char *from, int numBytes, int position)
{
    return file->WriteAt(from, numBytes, position);
}


//-------------------------###############################################-------------------------


//----------------------------------------------------------------------
// OpenFileTable::OpenFileTable
//  Constructor for OpenFileTable
//  Maximum of 10 files
//----------------------------------------------------------------------
OpenFileTable::OpenFileTable()
{
    table = new FileHandle*[10];  // array of FileHandle pointers
    count = 0;
}
OpenFileTable::~OpenFileTable()
{
    for (int i = 0; i < count; i++)
    {
        delete table[i];
    }
    delete[] table;
}

//----------------------------------------------------------------------
// OpenFileTable::IncrementCount
//  Increment the count of open files
//----------------------------------------------------------------------
void OpenFileTable::IncrementCount()
{
    count++;
}

//----------------------------------------------------------------------
// OpenFileTable::DecrementCount
//  Decrement the count of open files
//----------------------------------------------------------------------
void OpenFileTable::DecrementCount()
{
    count--;
}


//----------------------------------------------------------------------
// OpenFileTable::FindFile
//  Find a file in the table
//----------------------------------------------------------------------
int OpenFileTable::FindFile(int sector)
{
    for (int i = 0; i < OpenFileTableSize; i++)
    {
        if (table[i]->GetUseCount() > 0 && table[i]->GetSector() == sector)
        {
            return i;
        }
    }
    return -1;
}

//----------------------------------------------------------------------
// OpenFileTable::GetFile
//  Get a file from the table
//----------------------------------------------------------------------
FileHandle* OpenFileTable::GetFile(int index)
{
    return table[index];
}

//----------------------------------------------------------------------
// OpenFileTable::AddFile
//  Add a file to the table
//----------------------------------------------------------------------
bool OpenFileTable::AddFile(FileHandle* file)
{
    // Table Pleine
    if (count >= OpenFileTableSize)
    {
        return FALSE;
    }

    for (int i = 0; i < OpenFileTableSize; i++)
    {
        if (table[i] == NULL)
        {
            table[i] = file;
            count++;
            return TRUE;
        }
        // Multiple opens of the same file
        else if (table[i]->GetOpenFile() == file->GetOpenFile())
        {
            table[i]->IncrementUseCount();
            return TRUE;
        }
    }
    return FALSE;
}

//----------------------------------------------------------------------
// OpenFileTable::RemoveFile
//  Remove a file from the table
//----------------------------------------------------------------------
bool OpenFileTable::RemoveFile(FileHandle* file)
{
    for (int i = 0; i < OpenFileTableSize; i++)
    {
        if (table[i] == file)
        {
            if (table[i]->GetUseCount() > 1)
            {
                table[i]->DecrementUseCount();
                return TRUE;
            }
            delete table[i];
            table[i] = NULL;
            count--;
            return TRUE;
        }
    }
    return FALSE;
}

//----------------------------------------------------------------------
// OpenFileTable::RemoveAllFiles
//  Remove all files from the table
//----------------------------------------------------------------------
bool OpenFileTable::RemoveAllFiles()
{
    for (int i = 0; i < OpenFileTableSize; i++)
    {
        // Remove the file from the table or decrement the use count
        if (table[i]->GetUseCount() > 1)
        {
            table[i]->SetUseCount(0);
            delete table[i];
            table[i] = NULL;
        }
    }
    count = 0;
    return TRUE;
}

//----------------------------------------------------------------------
// OpenFileTable::TouchWriteableFiles
//  Touch all writeable files
//----------------------------------------------------------------------
void OpenFileTable::TouchWriteableFiles(char* modif)
{
    for (int i = 0; i < OpenFileTableSize; i++)
    {
        if (table[i]->GetUseCount() > 0 && table[i]->IsWriteable())
        {
            table[i]->GetOpenFile()->WriteAt(modif, strlen(modif), 0);
        }
    }
}


int FileSystem::Read(FileHandle* file, char *into, int numBytes) {	
	return file->Read(into,numBytes);
}

int FileSystem::Write(FileHandle* file, char *from, int numBytes) {		
	return file->Write(from,numBytes);
}

int FileSystem::ReadAt(FileHandle* file, char *into, int numBytes,int position) {
	return file->ReadAt(into,numBytes,position);
}

int FileSystem::WriteAt(FileHandle* file, char *from, int numBytes,int position) {
	return file->WriteAt(from,numBytes,position);
}


void FileSystem::Close (FileHandle* file){
    openFileTable->RemoveFile(file);  
}
void FileSystem::CloseAll(){
    openFileTable->RemoveAllFiles();
}

void FileSystem::TouchOpenedFiles(char * modif){
    openFileTable->TouchWriteableFiles(modif);
}


//IFT320: Print currentDir info for debugging
void FileSystem::CdInfo()
{
    printf("###--- Current directory info --- ###\n");
    Directory* cdir = new Directory(NumDirEntries);
    OpenFile* cdirFile = new OpenFile(cdSector);
    printf("\tsector: %d\n\n", cdSector);
    cdir->FetchFrom(cdirFile);
    cdir->List();
    printf("###--- End of current directory info --- ###\n");
    delete cdir;
    delete cdirFile;
}

//----------------------------------------------------------------------
// FileSystem::ChangeDirectory
// Fonction de changement de repertoire
// Doit etre implementee pour la partie A.
//----------------------------------------------------------------------
bool FileSystem::ChangeDirectory(char* name)
{
    bool success;

    // Get the current directory
    Directory* cdir = new Directory(NumDirEntries);
    OpenFile* cdFile = new OpenFile(cdSector);
    cdir->FetchFrom(cdFile);

    // FindDirectory assumes that '.' and '..' are entries when dir created
    int targetDirSector = cdir->FindDirectory(name);
    
    // Check if the target dir exists and change to it
    if (targetDirSector == -1)
    {
        success = FALSE; // target dir does not exists
    }
    else
    {
        printf("Changing to directory %s (sector %d)\n", name, targetDirSector);
        // Make the target dir the current dir
        cdSector = targetDirSector;
        success = TRUE;
    }
    delete cdFile;
    delete cdir;
    return success;    
}


//----------------------------------------------------------------------
// FileSystem::CreateDirectory
// Fonction de creation de repertoire
// Doit etre implementee pour la partie A.
//----------------------------------------------------------------------
bool FileSystem::CreateDirectory(char* name)
{
    return Create(name, DirectoryFileSize, TRUE);
}



//----------------------------------------------------------------------
// FileSystem::Create
// 	Create a file in the Nachos file system (similar to UNIX create).
//	Since we can't increase the size of files dynamically, we have
//	to give Create the initial size of the file.
//
//	The steps to create a file are:
//	  Make sure the file doesn't already exist
//        Allocate a sector for the file header
// 	  Allocate space on disk for the data blocks for the file
//	  Add the name to the directory
//	  Store the new file header on disk 
//	  Flush the changes to the bitmap and the directory back to disk
//
//	Return TRUE if everything goes ok, otherwise, return FALSE.
//
// 	Create fails if:
//   		file is already in directory
//	 	no free space for file header
//	 	no free entry for file in directory
//	 	no free space for data blocks for the file 
//
// 	Note that this implementation assumes there is no concurrent access
//	to the file system!
//
//	"name" -- name of file to be created
//	"initialSize" -- size of file to be created
// "isDirectory" -- true if the file is a directory
//----------------------------------------------------------------------
bool FileSystem::Create(char *name, int initialSize, bool isDirectory)
{
    Directory *directory;
    BitMap *freeMap;
    FileHeader *hdr;
    int sector;
    bool success;

    DEBUG('f', "Creating file/dir %s, size %d, isDirectory %d\n", name, initialSize, isDirectory);

    // Get the current directory
    directory = new Directory(NumDirEntries);
    OpenFile* currentDirFile = new OpenFile(cdSector);
	directory->FetchFrom(currentDirFile);

    // Check if the directory exists
    if (directory->Find(name) != -1)
    {
        success = FALSE; // file is already in directory
    }
    else if  (directory->FindDirectory(name) != -1)
    {
        success = FALSE;  // new directory is already in dir
    }
    else
    {
        freeMap = new BitMap(NumSectors);
        freeMap->FetchFrom(freeMapFile);
        sector = freeMap->Find(); // find a sector to hold the file header
        if (sector == -1)
            success = FALSE; // no free block for file header
        else if (!directory->Add(name, sector, isDirectory))
            success = FALSE; // no space in directory
        else
        {
            hdr = new FileHeader;
            // Allocate space according to fileSize or directory size 
            if (!hdr->Allocate(freeMap, initialSize))
                success = FALSE; // no space on disk for data
            else
            {
                success = TRUE;
                // everthing worked, flush all changes back to disk
                hdr->WriteBack(sector);
                directory->WriteBack(currentDirFile);
                freeMap->WriteBack(freeMapFile);

                // Add a "." and ".." entry for a new directory
                if (isDirectory)
                {
                    Directory *newDir = new Directory(NumDirEntries);
                    OpenFile *newDirFile = new OpenFile(sector);
                    newDir->Add(".", sector, TRUE);
                    newDir->Add("..", cdSector, TRUE);
                    newDir->WriteBack(newDirFile);
                    delete newDir;
                    delete newDirFile;
                    printf("Created directory %s (sector %d)\n", name, sector);
                }
                else
                {
                    printf("Created file %s (sector %d, size %d bytes)\n", name, sector, initialSize);
                }
            }
            delete hdr;
        }
        delete freeMap;
    }
    delete directory;
    delete currentDirFile;
    return success;
}

//----------------------------------------------------------------------
// FileSystem::Open
// 	Open a file for reading and writing.  
//	To open a file:
//	  Find the location of the file's header, using the directory 
//	  Bring the header into memory
//
//	"name" -- the text name of the file to be opened
//----------------------------------------------------------------------

FileHandle* FileSystem::Open(char *name)
{ 
    Directory* cdir = new Directory(NumDirEntries);
    OpenFile* cdirFile = new OpenFile(cdSector);
    OpenFile* openFile = NULL;
    int sector;	
	

    DEBUG('f', "Opening file %s\n", name);
	
    cdir->FetchFrom(cdirFile);
    sector = cdir->Find(name); 
    if (sector >= 0 && cdir->FindDirectory(name) == -1) {		
		openFile = new OpenFile(sector);	// filename was found in directory 	
        // Safety check for bad sector
        if (openFile == NULL)
        {
            delete cdir;
            delete cdirFile;
            return NULL;
        }
    }
    else
    {
        delete cdir;
        delete cdirFile;
        return NULL;  // file not found 
    }

    // Check if the file is already open
    int fileIndex = openFileTable->FindFile(sector);
    if (fileIndex != -1)
    {
        FileHandle* fileHandle = openFileTable->GetFile(fileIndex);
        fileHandle->IncrementUseCount();
        delete cdir;
        delete cdirFile;
        return fileHandle;
    }
    
    // Add the file to the open file table
    FileHandle* fileHandle = new FileHandle(name, TRUE, openFile);
    if (!openFileTable->AddFile(fileHandle))
    {
        delete cdir;
        delete cdirFile;
        return NULL;  // Table is full
    }
	
	delete cdir;
    delete cdirFile;
    return fileHandle;				// return NULL if not found
}

//----------------------------------------------------------------------
// FileSystem::Remove
// 	Delete a file from the file system.  This requires:
//	    Remove it from the directory
//	    Delete the space for its header
//	    Delete the space for its data blocks
//	    Write changes to directory, bitmap back to disk
//
//	Return TRUE if the file was deleted, FALSE if the file wasn't
//	in the file system.
//
//	"name" -- the text name of the file to be removed
//----------------------------------------------------------------------

bool FileSystem::Remove(char *name)
{ 
	//IFT320: partie A
	
    Directory *cdir;
    OpenFile *cdirFile;
    BitMap *freeMap;
    FileHeader *fileHdr;
    bool isDirectory;
    int sector;
    
    // Get the current directory
    cdir = new Directory(NumDirEntries);
	cdirFile = new OpenFile(cdSector);
    cdir->FetchFrom(cdirFile);
    
    // Check if the file or dir exists
    sector = cdir->Find(name);
    if (sector == -1) 
    {
        delete cdir;
        delete cdirFile;
        return FALSE;			 // file or dir not found 
    }
    isDirectory = FALSE;
    if (cdir->FindDirectory(name) != -1)
    {
        isDirectory = TRUE;
    }

    // Get the file header and freemap
	fileHdr = new FileHeader;
    fileHdr->FetchFrom(sector);
	
    freeMap = new BitMap(NumSectors);
    freeMap->FetchFrom(freeMapFile);

    // Remove dir only if empty
    if (isDirectory)
    {
        Directory *targetDir = new Directory(NumDirEntries);
        OpenFile *targetDirFile = new OpenFile(sector);
        targetDir->FetchFrom(targetDirFile);
        if (!targetDir->IsEmpty())
        {
            delete cdir;
            delete cdirFile;
            delete fileHdr;
            delete freeMap;
            delete targetDir;
            delete targetDirFile;
            return FALSE; // dir not empty
        }
        // cleanup  unneeded objects
        delete targetDir;
        delete targetDirFile;
    }

    // Free space/rm file/dir
    fileHdr->Deallocate(freeMap);  		// remove data blocks
    freeMap->Clear(sector);			// remove header block
    cdir->Remove(name);

    // Write changes back to disk
    freeMap->WriteBack(freeMapFile);		// flush to disk
    cdir->WriteBack(cdirFile);        // flush to disk
    delete fileHdr;
    delete freeMap;
    delete cdir;
    delete cdirFile;
    return TRUE;
}

//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the current directory.
//----------------------------------------------------------------------

void
FileSystem::List()
{
    // Get the current directory
    Directory* cdir = new Directory(NumDirEntries);
    OpenFile* cdirFile = new OpenFile(cdSector);
	
    cdir->FetchFrom(cdirFile);
    cdir->List();
    delete cdir;
}

//----------------------------------------------------------------------
// FileSystem::Print
// 	Print everything about the file system:
//	  the contents of the bitmap
//	  the contents of the directory
//	  for each file in the directory,
//	      the contents of the file header
//	      the data in the file
//----------------------------------------------------------------------

void
FileSystem::Print()
{
    FileHeader* bitHdr = new FileHeader;
    FileHeader* rdir = new FileHeader;
    FileHeader* cdirHdr = new FileHeader;
    BitMap* freeMap = new BitMap(NumSectors);
    Directory* cdir = new Directory(NumDirEntries);
    OpenFile* cdirFile = new OpenFile(cdSector);


    printf("Bit map file header:\n");
    bitHdr->FetchFrom(FreeMapSector);
    bitHdr->Print();

    printf("Root dir file header:\n");
    rdir->FetchFrom(DirectorySector);
    rdir->Print();

    printf("Current Directory file header:\n");
    cdirHdr->FetchFrom(cdSector);
    cdirHdr->Print();

    freeMap->FetchFrom(freeMapFile);
    freeMap->Print();
	
    cdir->FetchFrom(cdirFile);
    cdir->Print();

    delete bitHdr;
    delete cdirHdr;
    delete freeMap;
    delete cdir;
} 
