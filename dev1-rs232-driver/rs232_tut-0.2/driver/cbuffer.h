#ifndef __CBUFFER_H
# define __CBUFFER_H
#ifdef __KERNEL__
# include <linux/types.h>
#else
# define u8 unsigned char
#include <stdlib.h>
#endif


//IFT320 :   'cbuffer'  signifie  <Caracter Buffer>

//IFT320 : Compl�tez la d�finition ci-apr�s d'une structure de donn�es supportant les tampons circulaires (Ring buffer).
//IFT320 : Pour votre laboratoire, le tampon circulaire doit �tre accessible par les fonctions d'un pilote s'ex�cutant
//                 en 'Kernel mode' ainsi que par le code d'une ISR.
    struct cbuffer {
        u8 *buf;

    };

//IFT320 : Indiquez la signature de toutes les fonctions d'acc�s au tampon circulaire
//IFT320 : <Initialiser>,<Enfiler>, <D�filer>, <EstVide>, etc.

#endif /* __CBUFFER_H */
