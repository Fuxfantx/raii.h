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
} RAII_RC_H_RC_PTR_INSTANCE_DATA_STRUCTURE;

typedef struct {
	const RAII_RC_H_RC_PTR_INSTANCE_DATA_STRUCTURE RAII_RC_H_RC_PTR_INSTANCE;
} RAII_RC_H_RC_PTR_OPAQUE_DATA_STRUCTURE;

static inline RAII_RC_H_RC_PTR_OPAQUE_DATA_STRUCTURE RAII_RC_H_RC_PTR_NEW(const uint16_t size) {
	const RAII_RC_H_RC_PTR_INSTANCE_DATA_STRUCTURE i = {0, RAII_RC_H_FREE, 0, size};
	const RAII_RC_H_RC_PTR_OPAQUE_DATA_STRUCTURE p = {i};
	return p;
}

static inline RAII_RC_H_RC_PTR_OPAQUE_DATA_STRUCTURE RAII_RC_H_RC_PTR_NEW_WITH_FINAL(const uint16_t size, const RAII_H_FINAL final) {
	const RAII_RC_H_RC_PTR_INSTANCE_DATA_STRUCTURE i = {0, final ? final : RAII_RC_H_FREE, 0, size};
	const RAII_RC_H_RC_PTR_OPAQUE_DATA_STRUCTURE p = {i};
	return p;
}


/* Step 1: Create a Handle. */
#ifndef RcPtr
#define RcPtr(name, size)  const RAII_RC_H_RC_PTR_OPAQUE_DATA_STRUCTURE name = RAII_RC_H_RC_PTR_NEW( size );
#endif

#ifndef RcPtrf
#define RcPtrf(name, size, final)  const RAII_RC_H_RC_PTR_OPAQUE_DATA_STRUCTURE name = RAII_RC_H_RC_PTR_NEW_WITH_FINAL( size , final );
#endif


/* Step 2: Use the Ref(handle, ptype, name) scope macro, or Ref & Deref the handle manually. */
#ifndef Ref
#define Ref(handle, ptype, name)  TSCOPE(RAII_RC_H_RC_PTR_OPAQUE_DATA_STRUCTURE*, RAII_RC_H_CURRENT_RCPTR_HANDLE, (RcPtrRef(& handle ),& handle), RcPtrDeref)         \
	for( ptype name = handle .RAII_RC_H_RC_PTR_INSTANCE.RAII_RC_H_POINTER, *RAII_RC_H_CURRENT_EXITER=( ptype )1; RAII_RC_H_CURRENT_EXITER; RAII_RC_H_CURRENT_EXITER=0 )
#endif

static inline void* RcPtrRef(RAII_RC_H_RC_PTR_OPAQUE_DATA_STRUCTURE* const handle) {
	if(handle) {
		if( ! handle->RAII_RC_H_RC_PTR_INSTANCE.RAII_RC_H_REF_COUNT )
			((RAII_RC_H_RC_PTR_INSTANCE_DATA_STRUCTURE*)handle)->RAII_RC_H_POINTER = RAII_RC_H_MALLOC( handle->RAII_RC_H_RC_PTR_INSTANCE.RAII_RC_H_ALLOC_SIZE );
		++((RAII_RC_H_RC_PTR_INSTANCE_DATA_STRUCTURE*)handle)->RAII_RC_H_REF_COUNT;
		return handle->RAII_RC_H_RC_PTR_INSTANCE.RAII_RC_H_POINTER;
	}
	return 0;
}

static inline void RcPtrDeref(RAII_RC_H_RC_PTR_OPAQUE_DATA_STRUCTURE* const handle) {
	if(handle) {
		if( handle->RAII_RC_H_RC_PTR_INSTANCE.RAII_RC_H_REF_COUNT )
			--((RAII_RC_H_RC_PTR_INSTANCE_DATA_STRUCTURE*)handle)->RAII_RC_H_REF_COUNT;
		if( handle->RAII_RC_H_RC_PTR_INSTANCE.RAII_RC_H_REF_COUNT==0 && handle->RAII_RC_H_RC_PTR_INSTANCE.RAII_RC_H_POINTER ) {
			((RAII_RC_H_RC_PTR_INSTANCE_DATA_STRUCTURE*)handle)->RAII_RC_H_FINAL( handle->RAII_RC_H_RC_PTR_INSTANCE.RAII_RC_H_POINTER );
			((RAII_RC_H_RC_PTR_INSTANCE_DATA_STRUCTURE*)handle)->RAII_RC_H_POINTER = 0;
		}
	}
}


/* Step 3: You may want to use the toInit scope macro within the Ref scope macro to ensure the RcPtr initialized correctly. */
#ifndef toInit
#define toInit  if( RAII_RC_H_CURRENT_RCPTR_HANDLE->RAII_RC_H_RC_PTR_INSTANCE.RAII_RC_H_REF_COUNT==1 )
#endif

#endif   // RAII_RC_H