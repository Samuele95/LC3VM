#include <stdlib.h>
#include <string.h>
#include "lc3symboltable.h"

static nlist_t* hashtab[HASHSIZE];

unsigned hash(char* s) {
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

nlist_t* lookup(char *s) {
    nlist_t* np;
    for (np = hashtab[hash(s)]; np != NULL; np = np->next) {
        if (strcmp(s, np->name) == 0) return np;
    }
    return NULL;
}

nlist_t* install(char* name, nelem_t* defn) {
    nlist_t* np;
    unsigned hashval;

    if ((np = lookup(name)) == NULL) {
        np = (nlist_t*) malloc(sizeof(nlist_t));
        if (np == NULL || (np->name = strdup(name)) == NULL) return NULL;
        hashval = hash(name);
        np->next = hashtab[hashval]; /* will be null if there was no element */
        hashtab[hashval] = np;
    }
    np->defn = defn;
    return np;
}

nelem_t* create_field(uint16_t l, uint16_t* b, uint8_t flag, uint8_t len) {
    nelem_t* field = (nelem_t*) malloc(sizeof (nelem_t));
    field->line = l;
    field->bytes = b;
    field->stringflag = flag;
    field->byteslen = len;
    return field;
}

void delete_field(nelem_t* field) {
    return free(field);
}
