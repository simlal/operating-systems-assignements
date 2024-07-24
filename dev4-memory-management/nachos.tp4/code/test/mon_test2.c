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
	int a,b,c;
 
 	a=0;
 	b=0;
 	c=0;
 	/*Create("toto");
   fich = Open("toto");*/
 
  while(1)
  {   
  		int i;
  		for (i=0;i<1000000; i++)
  		{
			a++;
			b++;
			c = a + b;
		}
		/*Write("A\n", 2, fich);*/
	}
    
}
