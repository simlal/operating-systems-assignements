#include "machine.h"
#include "system.h"

void CopyFromUser(char * dest, char * source, int longueur)
{
	int i = 0;
	int tmp;
	int code;
	if (longueur >= 0)
		for (i=0 ; i < longueur ; i++) {		
				while (!machine->ReadMem((int)source+i, 1, &tmp))
				{
					// printf("Raising PageFault inside read op because cannot translate vaddr\n");
					machine->RaiseException(PageFaultException, (int)source+i);
				}
				
				dest[i] = (char)tmp;				
		}
	else
	{	
		do {
			while (!machine->ReadMem((int) source+i, 1, &tmp))
			{
				// printf("Raising PageFault inside read op because cannot translate vaddr\n");
				machine->RaiseException(PageFaultException, (int)source+i);
			}
			
			dest[i] = (char)tmp;			
			i++;
		}while (dest[i-1] != 0);
	}
}

void CopyToUser(char * dest, char * source, int longueur)
{
	int code;
	for (int i=0; i < longueur; i++){
			while (!machine->WriteMem((int)dest+i, 1, (int)source[i]))
			{
				// printf("Raising PageFault inside write op because cannot translate vaddr\n");
				machine->RaiseException(PageFaultException, (int)source+i);
			}
			
		}
	
}
