/* 
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"

int
main()
{
//	char tampon[30];
//	OpenFileId fich;
	 OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
   int i;
    Exec("../test/mon_test1",10,0);
    Exec("../test/mon_test2",0,0);
   //Exec("../test/test_err1",0);
   //Exec("../test/test_err2",0);
  	 Exec("../test/test_err3",0,0);
  
	//Create("toto");
   //fich = Open("toto");
   //Write("ceci est un test", 17, fich);
   for(i=1; i<10;i++)
  	 Write("CECI EST UN TEST\n", 18, output);
   //Read(tampon, 17,fich);
   Exit(3);
    
}
