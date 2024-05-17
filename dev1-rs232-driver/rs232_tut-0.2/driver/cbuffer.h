#ifndef __CBUFFER_H
# define __CBUFFER_H
#ifdef __KERNEL__
# include <linux/types.h>
#else
# define u8 unsigned char
#include <stdlib.h>
#include <stdbool.h>
#endif


//IFT320 :   'cbuffer'  signifie  <Caracter Buffer>

//IFT320 : Compl�tez la d�finition ci-apr�s d'une structure de donn�es supportant les tampons circulaires (Ring buffer).
//IFT320 : Pour votre laboratoire, le tampon circulaire doit �tre accessible par les fonctions d'un pilote s'ex�cutant
//                 en 'Kernel mode' ainsi que par le code d'une ISR.

/**
 * Circular buffer for receiving/read and transmitting/write data.
 * We have a ptr to the buffer, the index of the head and tail and the size
*/
struct cbuffer {
    u8* cbuff;
    int size;
    int head;
    int tail;
};

struct cbuffer* cbuffer_init(int size);
void cbuffer_info(struct cbuffer* cb);
int cbuffer_enqueue(struct cbuffer* cb, u8 data);
int cbuffer_dequeue(struct cbuffer* cb, u8* data);
bool cbuffer_is_empty(struct cbuffer* cb);



//IFT320 : Indiquez la signature de toutes les fonctions d'acc�s au tampon circulaire
//IFT320 : <Initialiser>,<Enfiler>, <D�filer>, <EstVide>, etc.

#endif /* __CBUFFER_H */