#ifndef RAII_H
#define RAII_H
#include <stdlib.h>

// Typedefs
#pragma pack(push)
#pragma pack(1)
    typedef void (*RAII_H_FINAL)(void*);
    typedef struct {
        struct { void** final_which; RAII_H_FINAL final; }  layer        ;
        unsigned long long int                              parent:62    ;
        unsigned long long int                              break_tag:1  ;
        unsigned long long int                              tag:1        ;
    } RAII_H_CHAIN;
#pragma pack(pop)

// Utils
static RAII_H_CHAIN* RAII_H_CHAIN_CURRENT = 0;
static inline void* RAII_H_REGISTER( void** final_which, const RAII_H_FINAL final ) {
    RAII_H_CHAIN* const latest = malloc(sizeof(RAII_H_CHAIN));
    latest->layer.final_which = final_which, latest->layer.final = final, latest->break_tag = 1, latest->tag = 1;
    latest->parent = (unsigned long long int)RAII_H_CHAIN_CURRENT, RAII_H_CHAIN_CURRENT = latest;
    return *final_which;
}
static inline unsigned char RAII_H_TRY_FINAL() {
    if( RAII_H_CHAIN_CURRENT && ++RAII_H_CHAIN_CURRENT->tag ) {
        (*RAII_H_CHAIN_CURRENT->layer.final)(*RAII_H_CHAIN_CURRENT->layer.final_which);
        RAII_H_CHAIN* const temporary_parent = (RAII_H_CHAIN*)RAII_H_CHAIN_CURRENT->parent;
        free(RAII_H_CHAIN_CURRENT), RAII_H_CHAIN_CURRENT = temporary_parent;
        return 0;
    }
    return 1;
}

/* Use "RAII_MALLOC" instead of "malloc" to express that the memory you allocated will be freed in a RAII way. */
#ifndef RAII_MALLOC
#define RAII_MALLOC  malloc
#endif

/* Return with Recursive Destruction */
// To make this library easy to comprehend, destruction behaviors are FORMER to the return statement;
// CACHE WHAT YOU NEED TO RETURN AHEAD.
#ifndef RETURN
#define RETURN  while(RAII_H_CHAIN_CURRENT) RAII_H_TRY_FINAL(); return
#endif

/* Typed ptr with with a scoped destructor */
#ifndef RAII
#define RAII(ptype, name, final)                                                                                     \
    for( ptype name =( ptype )RAII_H_REGISTER((void**)& name ,(RAII_H_FINAL) final ); RAII_H_TRY_FINAL(); )            \
        for(; RAII_H_CHAIN_CURRENT->break_tag; --RAII_H_CHAIN_CURRENT->break_tag)
#endif

/* Typed ptr with scoped constructor & destructor */
#ifndef SCOPE
#define SCOPE(ptype, name, init, final)                                                                               \
    for( ptype name = ( RAII_H_REGISTER((void**)& name ,(RAII_H_FINAL) final ), ( init ) ); RAII_H_TRY_FINAL(); )      \
        for(; RAII_H_CHAIN_CURRENT->break_tag; --RAII_H_CHAIN_CURRENT->break_tag)
#endif
#endif   // RAII_H