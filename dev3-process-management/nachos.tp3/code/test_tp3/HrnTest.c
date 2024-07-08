/* 
 * Test pour Priorite dynamique.
 * Tous les programmes devraient arriver a terminer malgre la boucle 
 * infinie qui est tres prioritaire
 * etre visibles, SubPriorityTest2 n'aura jamais l'UCT.
 * Ce test doit etre execute avec l'algorithme de planification HRN (3). 
 * Les valeurs de priorite les plus basses ont la plus haute priorite.
 */

#include "syscall.h"

int
main()
{  
	
	Write("1-Programme parent debut\n",26,ConsoleOutput);	
	Exec("../test_tp3/SubHrnTest",10);
	Exec("../test_tp3/Bouclalinfini",20);	
	Exec("../test_tp3/SubHrnTest2",30);
	Exec("../test_tp3/SubHrnTest3",40);
	
	Write("2-Programme parent fin\n",24,ConsoleOutput);
	
  	Exit(0);
    
}
