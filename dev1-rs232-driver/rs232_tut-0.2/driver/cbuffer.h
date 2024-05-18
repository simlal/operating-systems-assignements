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
int cbuffer_is_empty(struct cbuffer* cb);
int cbuffer_is_full(struct cbuffer* cb);
void cbuffer_free(struct cbuffer* cb);
int cbuffer_current_size(struct cbuffer* cb);
int cbuffer_space_left(struct cbuffer* cb);

#endif /* __CBUFFER_H */
