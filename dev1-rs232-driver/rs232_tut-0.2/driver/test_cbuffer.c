#include <stdio.h>
#include "cbuffer.h"

int main() {
    test_enqueue();
}

void test_enqueue() {
    struct cbuffer *cb = cbuffer_init(5);
    printf("size of cb: %i\n", cb->size);

    u8* data;
    int data_length = 20;
    data = malloc(data_length * sizeof(u8));
    strcpy(data, "abcdefg");


    for (int i=0; i < 7; i++) {
        printf("i=%i\n", i);
        
        u8 to_enqueue = data[i];
        int enqueue_status;
        enqueue_status = cbuffer_enqueue(cb, to_enqueue);
        if (enqueue_status == 0) {
            printf("Enqueued '%c' to cb\n", to_enqueue);
        } else {
            printf("cb is full!\n");
        }
    }
    free(data);
}