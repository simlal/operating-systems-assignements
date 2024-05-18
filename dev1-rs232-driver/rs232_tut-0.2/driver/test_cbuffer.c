#include <stdio.h>
#include <string.h>
#include "cbuffer.h"

int main() {
    test_enqueue();
    test_dequeue();
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
    // Free res
    free(data);
    cbuffer_free(cb);
}

void test_dequeue() {

    // cbuff of 10 with tail at [1] and head at [3]
    struct cbuffer* cb = cbuffer_init(8);

    cbuffer_info(cb);

    // Fill buffer with bogus
    printf("Filling buffer with 'helloworld!'...\n");
    u8* data_in;
    data_in = malloc(20 * sizeof(u8));
    strcpy(data_in, "helloworld!");
    int test_len = strlen(data_in);

    for (int i=0; i < test_len; i++) {
        int enq_status = cbuffer_enqueue(cb, data_in[i]);
        if (enq_status != 0) {
            printf("char %c not enqueued, buffer full!\n", data_in[i]);
        }
    }
    cbuffer_info(cb);
    
    u8* last_data;
    last_data = malloc(sizeof(u8));
    int n_dequeue = 3;
    printf("dequeuing %i times...\n", n_dequeue);
    for (int i=0; i < n_dequeue; i++) {
        cbuffer_dequeue(cb, last_data);
    }
    cbuffer_info(cb);
    printf("last_data dequeued: %s\n", last_data);

    // Free resources
    free(data_in);
    free(last_data);
    cbuffer_free(cb);

}
