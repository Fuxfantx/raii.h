# raii.h / raii_thread_safe.h

Try to use C with scoped constructor & destructor.

Implemented with heap memory & inline data structures, making it easier to comprehend.

```c
#include <raii.h>
#include <raii_thread_safe.h>

typedef struct {int x;} some_struct;
some_struct* some_struct_init(int, int, int);
void some_struct_final(some_struct*);

int main(int argc, char** argv) {
    // Ptr with with a scoped destructor
    RAII(some_struct*, a, some_struct_final) {
        a = some_struct_init(1, 2, 3);
        // Safe to use the "continue;" clause to jump out.
        continue;
        a->x = 1;
    }

    // Ptr with scoped constructor & destructor
    SCOPE(some_struct*, b, some_struct_init(1,2,3), some_struct_final) {
        // Also safe to use the "break;" clause to jump out.
        break;
        b->x = 0;
    }

    // You need an additional preparation step to utilize the raii_thread_safe.h
    USE_TRAII();
    TRAII(some_struct*, c, some_struct_final) {
        c = some_struct_init(1, 2, 3);
        c->x = 1;
        TSCOPE(some_struct*, d, some_struct_init(1,2,3), some_struct_final) {
            d->x = 0;
            // Use customized "return" to make destructors called correctly;
            // Note that destructors are called BEFORE the return clause.
            const int sth_to_return = d->x;   // RDETER/RSCOPE -> RETURN
            TRETURN sth_to_return;            // TDETER/TSCOPE -> TRETURN
        }
    }
}
```

# raii_rc.h

Refcount Ptr implementation with stack-value handle & scoped macro which refs & derefs the Ptr automatically.

- *For cross-platform **thread safety** concerns, this requires `stdatomic.h` (**C11**) .*

```c
#include <stdio.h>
#include <raii_rc.h>

typedef struct {int x;} some_struct;
void customized_free(some_struct* tofree) {
    printf("FREE %d\n", tofree->x);
    free(tofree);
}

int main(int argc, char** argv) {
    RcPtrf(my_handle, sizeof(some_struct), customized_free);
    Ref(my_handle, some_struct*, instance) {
        toInit {
            instance->x = 168;
        }
        const int sth_to_return = instance->x;
        RETURN sth_to_return;
    }
}
```
