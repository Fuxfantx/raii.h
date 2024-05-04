#ifndef RAII_H
#define RAII_H
#include <stdlib.h>

// Typedefs
typedef void (*RAII_H_FINAL)(void*);
typedef struct RAII_H_CHAIN {
    struct { void** final_which; RAII_H_FINAL final; }  layer ;
    struct RAII_H_CHAIN*                                parent;
} RAII_H_CHAIN;

// Utils
static RAII_H_CHAIN* RAII_H_CHAIN_CURRENT = 0;
static inline void* RAII_H_GET_EXITER( void** final_which, const RAII_H_FINAL final ) {
    RAII_H_CHAIN* const latest = malloc(sizeof(RAII_H_CHAIN));
    latest->layer.final_which = final_which, latest->layer.final = final, latest->parent = RAII_H_CHAIN_CURRENT;
    RAII_H_CHAIN_CURRENT = latest;
    return (void*)1;
}
static inline void* RAII_H_FINAL_ONCE() {
    (*RAII_H_CHAIN_CURRENT->layer.final)(*RAII_H_CHAIN_CURRENT->layer.final_which);
    RAII_H_CHAIN* const temporary_parent = RAII_H_CHAIN_CURRENT->parent;
    free(RAII_H_CHAIN_CURRENT), RAII_H_CHAIN_CURRENT = temporary_parent;
    return 0;
}

/* Use "RAII_MALLOC" instead "malloc" to express that the memory you allocated will be freed in a RAII way. */
#ifndef RAII_MALLOC
#define RAII_MALLOC malloc
#endif

/* Return with Recursive Destruction */
// To make this library easy to comprehend,
// Destruction behaviors are FORMER to the return statement;
// CACHE WHAT YOU NEED TO RETURN AHEAD.
#ifndef RETURN
#define RETURN  while(RAII_H_CHAIN_CURRENT) RAII_H_FINAL_ONCE(); return
#endif

/* Typed ptr with with a scoped destructor */
#ifndef TRAII
#define TRAII(type, name, final)  for( type * name , *RAII_H_CURRENT_EXITER=( type *)RAII_H_GET_EXITER((void**)& name ,(RAII_H_FINAL) final ); RAII_H_CURRENT_EXITER; RAII_H_CURRENT_EXITER=RAII_H_FINAL_ONCE() )
#endif

/* Typed ptr with scoped constructor & destructor */
#ifndef TSCOPE
#define TSCOPE(type, name, init, final)  for( type * name = init , *RAII_H_CURRENT_EXITER=( type *)RAII_H_GET_EXITER((void**)& name ,(RAII_H_FINAL) final ); RAII_H_CURRENT_EXITER; RAII_H_CURRENT_EXITER=RAII_H_FINAL_ONCE() )
#endif

#endif