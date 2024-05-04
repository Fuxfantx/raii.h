#ifndef RAII_RC_H
#define RAII_RC_H

#include "raii.h"
#include <stdint.h>

#define RAII_RC_H_MALLOC  malloc
#define RAII_RC_H_FREE  free

typedef struct {
	void*			RAII_RC_H_POINTER		;
	RAII_H_FINAL	RAII_RC_H_FINAL         ;
	uint64_t		RAII_RC_H_REF_COUNT:48  ;
	uint16_t		RAII_RC_H_ALLOC_SIZE;   // Max 65535 Bytes.
} RAII_RC_H_RC_PTR_INSTANCE;

typedef struct {
	const RAII_RC_H_RC_PTR_INSTANCE instance;
} RcPtr;


static inline RcPtr* RAII_RC_H_REF(RcPtr* const handle) {
	if(handle) {
		if( ! handle->instance.RAII_RC_H_REF_COUNT )
			((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_POINTER = RAII_RC_H_MALLOC( handle->instance.RAII_RC_H_ALLOC_SIZE );
		++((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_REF_COUNT;
	}
	return handle;
}

static inline void RAII_RC_H_DEREF(RcPtr* const handle) {
	if(handle) {
		if( handle->instance.RAII_RC_H_REF_COUNT )
			--((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_REF_COUNT;
		if( handle->instance.RAII_RC_H_REF_COUNT==0 && handle->instance.RAII_RC_H_POINTER ) {
			((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_FINAL( handle->instance.RAII_RC_H_POINTER );
			((RAII_RC_H_RC_PTR_INSTANCE*)handle)->RAII_RC_H_POINTER = 0;
		}
	}
}


/* Step 1: Create a Handle. */
static inline RcPtr RcPtrNew(const uint16_t size) {
	const RAII_RC_H_RC_PTR_INSTANCE i = {0, RAII_RC_H_FREE, 0, size};
	const RcPtr p = {i};
	return p;
}
static inline RcPtr RcPtrWithFinal(const uint16_t size, const RAII_H_FINAL final) {
	const RAII_RC_H_RC_PTR_INSTANCE i = {0, final ? final : RAII_RC_H_FREE, 0, size};
	const RcPtr p = {i};
	return p;
}

/* Step 2: Use the RcPtrRef(handle, ptype, name) scope macro. */
#ifndef RcPtrRef
#define RcPtrRef(handle, ptype, name)  TSCOPE(RcPtr*, RAII_RC_H_CURRENT_RCPTR_HANDLE, RAII_RC_H_REF(& handle ), RAII_RC_H_DEREF)                                      \
	for( ptype name = handle .instance.RAII_RC_H_POINTER, *RAII_RC_H_CURRENT_EXITER=( ptype )1; RAII_RC_H_CURRENT_EXITER; RAII_RC_H_CURRENT_EXITER=0 )
#endif

/* Step 3: You may want to use the toInit scope macro to ensure the RcPtr initialized correctly. */
#ifndef toInit
#define toInit  if( RAII_RC_H_CURRENT_RCPTR_HANDLE->instance.RAII_RC_H_REF_COUNT==1 )
#endif

#endif   // RAII_RC_H