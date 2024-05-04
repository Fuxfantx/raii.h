#ifndef RAII_H
#define RAII_H
#include <stdlib.h>

// Typedefs
typedef void (*RAII_H_FINAL)(void*);
typedef struct __RHL { void** final_which; RAII_H_FINAL final; }   RAII_H_LAYER;
typedef struct __RHC { RAII_H_LAYER layer; struct __RHC* parent; } RAII_H_CHAIN;

// Utils
static RAII_H_CHAIN* RAII_H_CHAIN_CURRENT = 0;
inline void* RAII_H_GET_EXITER( void** final_which, const RAII_H_FINAL final ) {
    RAII_H_CHAIN* const latest = malloc(sizeof(RAII_H_CHAIN));
    latest->layer.final_which = final_which;        latest->layer.final = final;
    latest->parent = RAII_H_CHAIN_CURRENT;
    RAII_H_CHAIN_CURRENT = latest;
    return (void*)1;
}
inline void* RAII_H_FINAL_ONCE() {
    (*RAII_H_CHAIN_CURRENT->layer.final)(*RAII_H_CHAIN_CURRENT->layer.final_which);
    RAII_H_CHAIN* const RAII_H_TEMPORARY_PARENT = RAII_H_CHAIN_CURRENT->parent;
    free(RAII_H_CHAIN_CURRENT);
    RAII_H_CHAIN_CURRENT = RAII_H_TEMPORARY_PARENT;
    return 0;
}

// RAII Implementations
// To make this library easy to comprehend, destruction behaviors are FORMER to the return statement;
// CACHE WHAT YOU NEED TO RETURN BEFORE.
#ifndef RETURN  // Return with Recursive Destruction
#define RETURN  while(RAII_H_CHAIN_CURRENT) RAII_H_FINAL_ONCE(); return
#endif

#ifndef TRAII
#define TRAII(type, name, final)  for( type * name,*RAII_H_CURRENT_EXITER=( type *)RAII_H_GET_EXITER((void**)& name ,(RAII_H_FINAL) final );RAII_H_CURRENT_EXITER;RAII_H_CURRENT_EXITER=RAII_H_FINAL_ONCE() )
#endif

#ifndef TSCOPE
#define TSCOPE(type, name, final, init, ...)  for( type * name = init ( __VA_ARGS__ ),*RAII_H_CURRENT_EXITER=( type *)RAII_H_GET_EXITER((void**)& name ,(RAII_H_FINAL) final );RAII_H_CURRENT_EXITER;RAII_H_CURRENT_EXITER=RAII_H_FINAL_ONCE() )
#endif

#endif