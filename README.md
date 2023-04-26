# RP2040_Psuedo_Atomic

```c++
#include "RP2040Atomic.hpp"

using namespace patom::types;
int main() {
  patom::PseudoAtomicInit();

}
```

```c++
struct {
  struct {
    patomic_int a;
    patomic_int b;
  } Inner;
  patomic_int c;
} Data;

auto a_val = Data.Inner.a.Load();
auto b_val = Data.Inner.b.Load();
auto c_val = Data.c.Load();
++a_val;
++b_val;
++c_val;
Data.Inner.a = a_val;
Data.Inner.b = b_val;
Data.c = c_val;



    auto a_val = Data.Inner.a.Load();
    ++a_val;
    auto c_val = Data.c.Load();
    Data.Inner.a = a_val;
    ++c_val;
    Data.c = c_val;
```