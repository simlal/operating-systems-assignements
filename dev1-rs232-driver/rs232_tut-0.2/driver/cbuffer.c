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
/**
 * Initialize a circular buffer
 * @param size size of the buffer to initialize
*/
struct cbuffer* cbuffer_init(int size)
{
    // Allocate memory for cbuffer struct
    struct cbuffer *cb = malloc(sizeof(struct cbuffer));

    // Allocate mem for the cbuff itself
    cb->cbuff = malloc(size * sizeof(u8));

    // Initialize head, tail and size
    cb->size = size;
    cb->head = 0;
    cb->tail = 0;

    // Fill with empty space
    for (int i=0; i < cb->size; i++) {
        cb->cbuff[i] = 0x20;
    }

    return cb;
}
/**
 * print cbuffer info for debugging
 * @param cb ptr to cbuffer
*/
void cbuffer_info(struct cbuffer* cb)
{
    printf("\n########--------cbuffer_info()--------########\n");
    printf("cb->size: %i\n", cb->size);
    printf("cb->head: %i\n", cb->head);
    printf("cb->tail: %i\n", cb->tail);
    printf("Current element count: %i\n", cbuffer_current_size(cb));
    printf("# Space left: %i\n", cbuffer_space_left(cb));

    // full/empty status
    if (cbuffer_is_empty(cb)) {
        printf("***cbuffer is empty***\n");
    }
    if (cbuffer_is_full(cb)) {
        printf("***cbuffer is full***\n");
    }
    
    printf("\nCurrent content:\n");
    if (cb->cbuff == NULL) {    // avoid nullptr excepts
        printf("NO CONTENT. cbuff ptr is NULL");
        return;
    }

    for (int i=0; i < cb->size; i++) {
        printf("cb->cbuff[%i]: %c\n", i, cb->cbuff[i]);
    } 
    printf("########--------END OF INFO--------########\n\n");
    return;
}

/**
 * Enqueue 1 byte to head of the buffer
 * @param cb ptr to the cbuffer
 * @param data new data to enqueue on the buffer
 * @returns 1 if buffer is full (no changes), 0 if data added.
*/
int cbuffer_enqueue(struct cbuffer* cb, u8 data)
{
    // Do not enqueue when cbuffer is full
    if (cbuffer_is_full(cb)) {
        return 1;
    }

    // Add data and move the head
    cb->cbuff[cb->head] = data;
    cb->head = (cb->head + 1) % cb->size;
    return 0;
}

/**
 * Dequeue 1 byte from the tail of the buffer
 * @param cb ptr to the cbuffer
 * @param data ptr to data store
*/
int cbuffer_dequeue(struct cbuffer* cb, u8* data)
{
    // Do not dequeue when cbuff is empty
    if (cbuffer_is_empty(cb)) {
        return 1;
    }

    // Extract data and move the tail
    *data = cb->cbuff[cb->tail];
    cb->cbuff[cb->tail] = 0xFF;    // REPLACE DATA FOR DEBUGGING
    cb->tail = (cb->tail + 1) % cb->size;
    return 0;
}

/**
 * Buffer is empty head and tail at same position
 * @param cb ptr to the cbuffer
*/
bool cbuffer_is_empty(struct cbuffer* cb)
{
    return cb->head == cb->tail;
}

/**
 * Buffer is full when head is 1 index ahead of tail
 * @param cb ptr to the cbuffer
*/
bool cbuffer_is_full(struct cbuffer* cb)
{
    int next_to_head;
    next_to_head = (cb->head + 1) % cb->size;
    return next_to_head == cb->tail;
}

/**
 * Free the buff char array and cbuffer struct
 * @param cb ptr to the cbuffer
*/
void cbuffer_free(struct cbuffer* cb)
{
    // Freeing cbuffer char array
    free(cb->cbuff);
    cb->cbuff = NULL;
    // Free the cbuffer struct
    free(cb);
}

/**
 * Return the current size of the cbuffer
 * @param cb ptr to the cbuffer
*/
int cbuffer_current_size(struct cbuffer* cb)
{
    return (cb->head - cb->tail + cb->size) % cb->size;
}

/**
 * space left before buffer is full
 * @param cb ptr to the cbuffer
*/
int cbuffer_space_left(struct cbuffer* cb)
{
    if (cbuffer_is_empty(cb)) {
        return cb->size;
    } else if (cbuffer_is_full) {
        return 0;
    } else {
        return cb->size - cbuffer_current_size(cb) - 1;
    }
}

//IFT320 : Codez le corps de toutes les fonctions d'acc�s au tampon circulaire.
//IFT320 : Vous devriez avoir inscrit la signature de ces fonctions dans le fichier 'cbuffer.h'
//IFT320 : <Initialiser>,<Enfiler>, <D�filer>, <EstVide>, etc.
