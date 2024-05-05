# raii.h

Try to use C with scoped constructor & destructor.

Implemented with heap memory & inline data structures, making it easier to comprehend.

```c
#include <raii.h>

typedef struct {int x;} some_struct;
some_struct* some_struct_init(int, int, int);
void some_struct_final(some_struct*);

int main(int argc, char** argv) {
    // Typed ptr with with a scoped destructor
    TRAII(some_struct*, b, some_struct_final) {
        b = some_struct_init(1, 2, 3);
        // Safe to use the "continue;" or the "break;" clause to jump out.
        continue;
        b->x = 1;
    }

    // Typed ptr with scoped constructor & destructor
    TSCOPE(some_struct*, c, some_struct_init(1,2,3), some_struct_final) {
        c->x = 0;
        // Use RETURN instead of return to make destructors called correctly;
        // Note that destructors are called BEFORE the return clause
        int sth_to_return = c->x;
        RETURN sth_to_return;
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
