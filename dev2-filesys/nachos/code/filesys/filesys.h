// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.  This is provided in case the
//	multiprogramming and virtual memory assignments (which make use
//	of the file system) are done before the file system assignment.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"

#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available
				
//IFT320: CETTE DEFINITION EST UTILISEE POUR LES TP 3 et 4.
#define FileHandle OpenFile *
		
class FileSystem {
  public:
    FileSystem(bool format) {}

    bool Create(char *name, int initialSize) { 
	int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1) return FALSE;
			//UNIX close. Shadowing problem with local method, weird syntax.
			::Close(fileDescriptor); 
		return TRUE; 
	}

    OpenFile* Open(char *name) {
	  int fileDescriptor = OpenForReadWrite(name, FALSE);

	  if (fileDescriptor == -1) return NULL;
	  return new OpenFile(fileDescriptor);
      }

    bool Remove(char *name) { return Unlink(name) == 0; }
	int Read(FileHandle file, char *into, int numBytes) {	
		return file->Read(into,numBytes);
	}

	int Write(FileHandle file, char *from, int numBytes) {		
		return file->Write(from,numBytes);
	}

	int ReadAt(FileHandle file, char *into, int numBytes,int position) {
		return file->ReadAt(into,numBytes,position);
	}

	int WriteAt(FileHandle file, char *from, int numBytes,int position) {
		return file->WriteAt(from,numBytes,position);
	}
	void Close (FileHandle file){
		delete file;
	}

};

#else // FILESYS
//IFT320: DEFINITION DE FILESYSTEM UTILISEE POUR LE TP2

// FileHandle wrapper around an OpenFile
class FileHandle {
	private:
		char* name;
		int sector;
		bool writeable;
		OpenFile* file;
		int position;
		int useCount;
		
	public:
		// Constructor/Destructor around an OpenFile
		FileHandle(char* name, bool readOnly, OpenFile* file);
		~FileHandle();
		
		// To pass the read/write calls to the OpenFile
		OpenFile* GetOpenFile();

		// The location of the file on disk
		int GetSector();

		// useCount for number of concurrent users
		int GetUseCount();
		int SetUseCount(int count);
		void IncrementUseCount();
		void DecrementUseCount();

		// OpenFile wrapper for read/write
		int Read(char *into, int numBytes);
		int Write(char *from, int numBytes);
		int ReadAt(char *into, int numBytes, int position);
		int WriteAt(char *from, int numBytes, int position);

		// Writeable status
		bool IsWriteable();
};


#define OpenFileTableSize 10
class OpenFileTable {
	private:
		FileHandle** table;  // pointer to an array of FileHandle pointers
		int count; // Number of files in the table
		
	public:
		OpenFileTable();  // Maximum of 10 files
		~OpenFileTable();
		
		// Modify the count of open files
		void IncrementCount();
		void DecrementCount();

		// Files and table access
		int FindFile(int sector);
		FileHandle* GetFile(int index);

		// Add a file to the table
		bool AddFile(FileHandle* file);

		// Remove a file from the table
		bool RemoveFile(FileHandle* file);

		// Remove all files
		bool RemoveAllFiles();

		// Touch all writeable files
		void TouchWriteableFiles(char* modif);
};

class FileSystem {
  public:
    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.

    bool Create(char *name, int initialSize, bool isDirectory);  	
					// Create a file (UNIX creat)
					
	

    bool Remove(char *name);  		// Delete a file (UNIX unlink)

    void List();			// List all the files in the file system

    void Print();			// List all the files and their contents
	
	//IFT320: Services du systeme de fichiers.
	
	FileHandle* Open(char *name); 		
	bool ChangeDirectory(char* name);
	bool CreateDirectory(char* name);
	/**
	 *! I used obj instead of pointer to avoid compiler error:
	 *! /userprog/addrspace.cc:113:50: error: no matching function for call to
	 *! ‘FileSystem::ReadAt(FileHandle&, char*, int&, int&)’
	 */
	int Read(FileHandle* file, char *into, int numBytes);
	int Write(FileHandle* file, char *from, int numBytes);
	int ReadAt(FileHandle* file, char *into, int numBytes,int position);
	int WriteAt(FileHandle* file, char *from, int numBytes,int position);

	void Close (FileHandle* file);
	void CloseAll();
	void TouchOpenedFiles(char * modif);
	void CdInfo(); // Print cd info for debugging
	

  private:
	OpenFile* freeMapFile;		// Bit map of free disk blocks,
					// represented as a file
	
	OpenFile* directoryFile;		// "Root" directory -- list of 
					// file names, represented as a file

	// Sector num of the currently opened directory
	int cdSector;

	// OpenFileTable
	OpenFileTable* openFileTable;
};


#endif // FILESYS

#endif // FS_H
