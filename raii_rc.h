#ifndef RAII_RC_H
#define RAII_RC_H

#include "raii.h"
#include <stdint.h>

#define RAII_RC_H_MALLOC  malloc
#define RAII_RC_H_FREE  free

typedef void* RcPtrHandle;
typedef struct {
	void*			RAII_RC_H_POINTER		;
	RAII_H_FINAL	RAII_RC_H_FINAL         ;
	uint64_t		RAII_RC_H_REF_COUNT:48  ;
	uint16_t		RAII_RC_H_ALLOC_SIZE;   // Max 65535 Bytes.
} RAII_RC_H_RC_PTR_INSTANCE;


static inline RcPtrHandle RAII_RC_H_REF(const RcPtrHandle handle) {
	if(handle) {
		if( ! ((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_REF_COUNT )
			((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_POINTER = RAII_RC_H_MALLOC( ((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_ALLOC_SIZE );
		++((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_REF_COUNT;
	}
	return handle;
}

static inline void RAII_RC_H_DEREF(const RcPtrHandle handle) {
	if(handle) {
		if( ((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_REF_COUNT )
			--((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_REF_COUNT;
		if( ((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_REF_COUNT==0 && ((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_POINTER ) {
			((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_FINAL( ((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_POINTER );
			((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_POINTER = 0;
		}
	}
}


/* Step 1: Create a Handle. */
static inline RcPtrHandle RcPtr(const uint16_t size) {
	RAII_RC_H_RC_PTR_INSTANCE* const handle = RAII_RC_H_MALLOC( sizeof(RAII_RC_H_RC_PTR_INSTANCE) );
	handle->RAII_RC_H_POINTER = 0;
	handle->RAII_RC_H_REF_COUNT = 0;
	handle->RAII_RC_H_ALLOC_SIZE = size;
	handle->RAII_RC_H_FINAL = RAII_RC_H_FREE ;
	return handle;
}

static inline RcPtrHandle RcPtrWithFinal(const uint16_t size, const RAII_H_FINAL final) {
	RAII_RC_H_RC_PTR_INSTANCE* const handle = RcPtr(size);
	if(final)
		handle->RAII_RC_H_FINAL = final;
	return handle;
}

/* Step 2: Use the RcPtrRef(handle, ptype, name) scope macro. */
#ifndef RcPtrRef
#define RcPtrRef(handle, ptype, name, init)  TSCOPE(RcPtrHandle, RAII_RC_H_CURRENT_RCPTR_HANDLE, RAII_RC_H_REF( handle ), RAII_RC_H_DEREF)    \
	for( ptype name = handle ? ((RAII_RC_H_RC_PTR_INSTANCE*) handle )->RAII_RC_H_POINTER : 0, *RAII_RC_H_CURRENT_EXITER=( ptype )1; RAII_RC_H_CURRENT_EXITER; RAII_RC_H_CURRENT_EXITER=0 )
#endif

/* Step 3: You may want to use the toInit(statement) macro to ensure the RcPtr initialized correctly. */
#ifndef toInit
#define toInit(statement)  if(RAII_RC_H_CURRENT_RCPTR_HANDLE && ((RAII_RC_H_RC_PTR_INSTANCE*)RAII_RC_H_CURRENT_RCPTR_HANDLE)->RAII_RC_H_REF_COUNT==1 ) { statement ;}
#endif

/* Step 4: Attempt to Free the Handle.
 *         Call RcPtrFree(&some_rcptr_handle), then check if some_rcptr_handle==0. */
static inline void RcPtrFree(RcPtrHandle* const handle) {
	// Since the memory allocation is deferred to the referring with RAII_RC_H_REF_COUNT == 0,
	// The memory leakage will be 24 Bytes when forgetting to free a RcPtr, which should be acceptable.
	if( ((RAII_RC_H_RC_PTR_INSTANCE*)*handle)->RAII_RC_H_REF_COUNT ) return;
	RAII_RC_H_FREE(*handle);		 *handle = 0;
}

#endif   // RAII_RC_H