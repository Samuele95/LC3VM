
#include <stdio.h>
#include "lc3programqueue.h"

static queue_adt* progqueue;
static int head;
static int tail;
static uint16_t res;


queue_adt* allocate_queue(int s) {
    progqueue = (queue_adt*) malloc(sizeof(queue_adt));
    progqueue->queue = (uint16_t*) malloc(sizeof(uint16_t)*s);
    progqueue->size = s;
    return progqueue;
}

void free_queue() {
    free(progqueue->queue);
    free(progqueue);
}

void convert_queue_to_file(FILE* fout) {
    if (fwrite(progqueue->queue, sizeof(uint16_t), progqueue->size, fout) < progqueue->size) {
        perror("Error in compiling the program. Didn't compile the whole program. Abort");
        exit(EXIT_FAILURE);
    }
}


void enqueue(uint16_t f) {
    if (is_full()) {
        perror("Can't insert an element in a full queue. Exiting to avoid a queue overflow.");
        exit(EXIT_FAILURE);
    }
    progqueue->queue[tail] = f;
    tail = (tail+1) % progqueue->size;
}

void enqueue_array(uint16_t* rstring, int l) {
    for (int i = 0; i < l; enqueue(rstring[i]), i++);
}

uint16_t dequeue(void) {
    uint16_t result;
    if (is_empty()) {
        perror("Can't get an element from an empty queue. Exiting to avoid queue underflow.");
        exit(EXIT_FAILURE);
    }
    result = progqueue->queue[head];
    head = (head+1) % progqueue->size;
    return result;
}

void printqueue(FILE* ptr) {
    while (!is_empty()) {
       res = dequeue();
       printf("%x\n", res);
       fwrite(&res, sizeof(uint16_t), 1, ptr); 
    }
}

int is_empty() {
    return head == tail;
}

int is_full() {
    return head == tail+1;
}
