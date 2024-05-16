#ifdef __KERNEL__ 
# include <linux/slab.h>
#else
# define kmalloc(a,b) malloc(a)
# define kfree(a) free(a)
# define spin_lock(l)
# define spin_unlock(l)
# define spin_lock_irqsave(l, f)
# define spin_unlock_irqrestore(l, f)
# define ENOMEM 1
#endif

#include "cbuffer.h"

//IFT320 :   'cbuffer'  signifie  <Caracter Buffer>
struct cbuffer * cbuffer_init(int size)
{
    // Allocate memory for cbuffer struct
    struct cbuffer *cb = malloc(sizeof(struct cbuffer));

    // Allocate mem for the cbuff itself
    cb->cbuff = malloc(size * sizeof(u8));

    // Initialize head, tail and size
    cb->head = 0;
    cb->tail = 0;
    cb->size = size;

    return cb;
}

//IFT320 : Codez le corps de toutes les fonctions d'acc�s au tampon circulaire.
//IFT320 : Vous devriez avoir inscrit la signature de ces fonctions dans le fichier 'cbuffer.h'
//IFT320 : <Initialiser>,<Enfiler>, <D�filer>, <EstVide>, etc.
