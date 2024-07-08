/* 
 * Test pour round robin.
 * Les priorites sont normalement ignorees.
 * Les affichages devraient apparaitre en alternance apres 1 et 2(3-4-5, 3-4-5, ...)
 * Ce test doit etre execute avec l'algorithme de planification Round Robin (1).
 * Ce devrait fonctionner meme si addrspace.cc n'est pas modifie.
 * Les valeurs de priorite les plus basses ont la plus haute priorite.
 */

#include "syscall.h"

int
main()
{  
	
	Write("1-Programme parent debut\n",26,ConsoleOutput);
	
	Exec("../test_tp3/SubRobinTest",10);
	Exec("../test_tp3/SubRobinTest2",20);
	Exec("../test_tp3/SubRobinTest3",30);
	
	
	Write("2-Programme parent fin\n",24,ConsoleOutput);
	
  	Exit(0);
    
}
