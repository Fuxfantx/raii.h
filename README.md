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
    TRAII(some_struct, b, some_struct_final) {
        b = some_struct_init(1, 2, 3);
        // Safe to use the "continue;" clause to jump out,
        // but UNSAFE to use the "break" clause
        continue;
        b->x = 1;
    }

    // Typed ptr with scoped constructor & destructor
    TSCOPE(some_struct, c, some_struct_init(1,2,3), some_struct_final) {
        c->x = 0;
        // Use RETURN instead of return to make destructors called correctly.
        int sth_to_return = c->x;
        RETURN sth_to_return;
    }
}
```
