#include <stdlib.h>

typedef struct queue {
    uint16_t* queue;
    uint16_t size;
} queue_adt;

queue_adt* allocate_queue(int size);
void free_queue();
void convert_queue_to_file(FILE* fout);
void enqueue(uint16_t f);
void enqueue_array(uint16_t* rstring, int l);
uint16_t dequeue(void);
void printqueue(FILE*);
int is_empty();
int is_full();
