#ifndef RAII_THREAD_SAFE_H
#define RAII_THREAD_SAFE_H
#include <stdlib.h>

// Typedefs
#pragma pack(push)
#pragma pack(1)
    typedef void (*RAII_THREAD_SAFE_H_FINAL)(void*);
    typedef struct {
        struct {void** final_which; RAII_THREAD_SAFE_H_FINAL final;}  layer        ;
        unsigned long long int                                        parent:62    ;
        unsigned long long int                                        break_tag:1  ;
        unsigned long long int                                        tag:1        ;
    } RAII_THREAD_SAFE_H_CHAIN;
#pragma pack(pop)

/* 0. Tell this header what memory allocator & release function to use;
      Use "RAII_THREAD_SAFE_H_MALLOC" instead of "malloc" to express that the memory you allocated will be freed in a RAII way. */
#ifndef RAII_THREAD_SAFE_H_MALLOC
#define RAII_THREAD_SAFE_H_MALLOC  malloc
#endif

#ifndef RAII_THREAD_SAFE_H_FREE
#define RAII_THREAD_SAFE_H_FREE  free
#endif

// Utils
static inline void* RAII_THREAD_SAFE_H_REGISTER( RAII_THREAD_SAFE_H_CHAIN** RAII_THREAD_SAFE_H_CHAIN_CURRENT, void** final_which, const RAII_THREAD_SAFE_H_FINAL final ) {
    RAII_THREAD_SAFE_H_CHAIN* const latest = RAII_THREAD_SAFE_H_MALLOC(sizeof(RAII_THREAD_SAFE_H_CHAIN));
    latest->layer.final_which = final_which, latest->layer.final = final, latest->break_tag = 1, latest->tag = 1;
    latest->parent = (unsigned long long int)*RAII_THREAD_SAFE_H_CHAIN_CURRENT, *RAII_THREAD_SAFE_H_CHAIN_CURRENT = latest;
    return *final_which;
}
static inline unsigned char RAII_THREAD_SAFE_H_TRY_FINAL(RAII_THREAD_SAFE_H_CHAIN** RAII_THREAD_SAFE_H_CHAIN_CURRENT) {
    if( RAII_THREAD_SAFE_H_CHAIN_CURRENT && ++(*RAII_THREAD_SAFE_H_CHAIN_CURRENT)->tag ) {
        (*(*RAII_THREAD_SAFE_H_CHAIN_CURRENT)->layer.final)(*(*RAII_THREAD_SAFE_H_CHAIN_CURRENT)->layer.final_which);
        RAII_THREAD_SAFE_H_CHAIN* const temporary_parent = (RAII_THREAD_SAFE_H_CHAIN*)(*RAII_THREAD_SAFE_H_CHAIN_CURRENT)->parent;
        RAII_THREAD_SAFE_H_FREE(*RAII_THREAD_SAFE_H_CHAIN_CURRENT), *RAII_THREAD_SAFE_H_CHAIN_CURRENT = temporary_parent;
        return 0;
    }
    return 1;
}

/* 1. Before creating the 1st RAII scope, use the "USE_RAII();" clause to properly initialize the RAII handle. */
#ifndef USE_TRAII
#define USE_TRAII()  RAII_THREAD_SAFE_H_CHAIN* RAII_THREAD_SAFE_H_CHAIN_CURRENT = 0;
#endif

/* 2A.Typed ptr with with a scoped destructor */
#ifndef TRAII
#define TRAII(ptype, name, final)                                                                                                                                                                                   \
    for( ptype name =( ptype )RAII_THREAD_SAFE_H_REGISTER(&RAII_THREAD_SAFE_H_CHAIN_CURRENT, (void**)& name ,(RAII_THREAD_SAFE_H_FINAL) final ); RAII_THREAD_SAFE_H_TRY_FINAL(&RAII_THREAD_SAFE_H_CHAIN_CURRENT); )  \
        for(; RAII_THREAD_SAFE_H_CHAIN_CURRENT->break_tag; --RAII_THREAD_SAFE_H_CHAIN_CURRENT->break_tag)
#endif

/* 2B.Typed ptr with scoped constructor & destructor */
#ifndef TSCOPE
#define TSCOPE(ptype, name, init, final)                                                                                                                                                                                   \
    for( ptype name = ( RAII_THREAD_SAFE_H_REGISTER(&RAII_THREAD_SAFE_H_CHAIN_CURRENT, (void**)& name ,(RAII_THREAD_SAFE_H_FINAL) final ), ( init ) ); RAII_THREAD_SAFE_H_TRY_FINAL(&RAII_THREAD_SAFE_H_CHAIN_CURRENT); )  \
        for(; RAII_THREAD_SAFE_H_CHAIN_CURRENT->break_tag; --RAII_THREAD_SAFE_H_CHAIN_CURRENT->break_tag)
#endif

/* 3. Use Treturn instead of "return" to ensure finalizers called correctly. */
// To make this library easy to comprehend, destruction behaviors are FORMER to the return statement;
// CACHE WHAT YOU NEED TO RETURN AHEAD.
#ifndef TRETURN
#define TRETURN  while(RAII_THREAD_SAFE_H_CHAIN_CURRENT) RAII_THREAD_SAFE_H_TRY_FINAL(&RAII_THREAD_SAFE_H_CHAIN_CURRENT); return
#endif

#endif   // RAII_THREAD_SAFE_H