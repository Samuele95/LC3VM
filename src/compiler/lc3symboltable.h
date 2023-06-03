#include <stdint-gcc.h>
#define HASHSIZE 101

typedef struct nelem {
    uint16_t line;
    int16_t* bytes;
    uint8_t stringflag;
    uint8_t byteslen;
} nelem_t;

typedef struct nlist {
    struct nlist* next;
    char* name;
    nelem_t* defn;
} nlist_t;


unsigned hash(char* s);
nlist_t* lookup(char *s);
nlist_t* install(char* name, nelem_t* defn);
nelem_t* create_field(uint16_t l, uint16_t* b, uint8_t flag, uint8_t len);
void delete_field(nelem_t* field);
