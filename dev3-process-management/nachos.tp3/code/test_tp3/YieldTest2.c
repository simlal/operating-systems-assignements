/* 
 * Programme parent pour Yield test plus approfondi.
 * Ne peut fonctionner que si deux processus peuvent partager 
 * correctement la memoire.
 * Devrait sortir Dude! et Sweet! en alternance.
 */

#include "syscall.h"

int
main()
{  
	
	int i;	
	Exec("../test_tp3/SubYieldTest2",10);
	
	for(i=0;i<10;i++){
		Write("Dude!\n",7,ConsoleOutput);
		Yield();
	}	
  	Exit(0);
    
}
