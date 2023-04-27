# RP2040 Pseudo Atomic
[![Standard](https://img.shields.io/badge/C%2B%2B-20-C%2B%2B20?logo=c%2B%2B&color=00599C&style=flat)](https://isocpp.org/std/the-standard)
[![MCU](https://img.shields.io/badge/MCU-RP2040-RP2040?logo=raspberrypi&color=46ae4b&style=flat)](https://www.raspberrypi.com/products/rp2040)
![GitHub](https://img.shields.io/github/license/TSprech/RP2040_Pseudo_Atomic?color=%23D22128&logo=apache)

*Finally use atomic variables on the RP2040 / Pico!*

This library is a header only implementation of atomic variables for the RP2040. It is designed to be simple, fast, and similar in usage to the existing std::atomic library.

### Table of Contents
- [RP2040 Pseudo Atomic](#rp2040-pseudo-atomic)
  - [Motivation](#motivation)
      - [Why *Pseudo* Atomic](#why-pseudo-atomic)
  - [The One Important Function](#the-one-important-function)
  - [Examples](#examples)
    - [Minimum](#minimum)
    - [Data Read and Write](#data-read-and-write)
  - [Implementation](#implementation)
  - [Future Improvements](#future-improvements)
  - [License](#license)

## Motivation
The default sync Pico API provides a mechanism for having atomic safety during a section of code, called `critical_section`. While this is a great feature, it has some limitations:
- Large sections of atomic code are wrapped in a single `cricital_section` guard, which can cause the other core to unproductively wait its turn
- If individual variable access is wrapped in a guard, the code length explodes as each access requires acquiring and releasing the `critical_section`
  - It can also be error-prone to keep track of all the critical section acquisitions and releases and may be difficult to debug if there is a mismatch

The solution is to emulate the behavior of std::atomic as much as possible. This would be accomplished by:
- No manual management of critical sections
- Locking the other core out for the shortest time possible
  - This means the other core should wait a negligible amount of time before it is able to access the variable which the other core is currently using
- Work with the basic standard types and conform to the same safety requirements of std::atomic (no copying, no moving, etc.)
- Require as little setup as possible

### Why *Pseudo* Atomic
The *pseudo* in the name comes from the fact that this library does not use the traditional methods used by std::atomic. The RP2040, and the Cortex M0+ in general, does not have native support for atomic variables. As such, this pseudo version tries to reach the same atomic functionally, but using only the limited synchronization facilities available.

## The One Important Function
Unlike std::atomic, you have to call a single function to setup the Pseudo Atomic library. `patom::PseudoAtomicInit()` MUST be called at the start of your program to claim a spinlock for the patom variables. Without this, the library will still act normally but you will not have type safety.**

** *This is NOT checked during runtime to reduce the amount of time spent in library functions. As such it is the programmer's responsibility to ensure the initialization function is called.*

## Examples
### Minimum
```c++
#include "RP2040Atomic.hpp"

using namespace patom::types; // Optional to expose the patomic_int, patomic_float, etc. types

int main() {
  patom::PseudoAtomicInit(); // Make sure you call this!
}
```

### Data Read and Write
```c++
patomic_int a;
patomic_int b;

int main1() {
  while(true) {
    auto a_val = a.Load(); // This atomically reads the value of a
    ++a_val; // Increments the temporary copy of a's value
    auto b_val = b.Load();
    a = a_val; // This atomically updates the value of a
    ++b_val;
    b = b_val;
  }
}

int main() {
  patom::PseudoAtomicInit(); // Make sure you call this!
  
  multicore_launch_core1(main1);

  while(true) {
    auto a_val = a.Load();
    auto b_val = b.Load();
    ++a_val;
    ++b_val;
    a = a_val;
    b = b_val;
  }
}
```
*Note that this example does not promise it will be exactly incremented by 1 every time, as the incrementing is not atomic, it just ensures that garbage data (such as reading garbage bytes because the other core is writing) is prevented.*

## Implementation
The core mechanic being leveraged by this library is the critical_section API exposed by the Pico SDK. By having a single critical section shared across all the patomic variables, it ensures that both cores cannot read or write to the same variable at the same time.

The library wraps only the single read and single write operation of a variable in the critical section. As such, only a single operation occurs while the other core is locked out of the variable. As such, in the rare event that both cores attempt to access the same variable at the same time, the hardware handles the lockout and simultaneous access issues are avoided. However, due to how little time each core holds onto the spin lock, it is difficult to make the two cores attempt simultaneous access even purposefully.

I would also highly recommend looking at the implementation in the header file, it is only a handful of lines of logic and designed to be simple and concise.

## Future Improvements
- [ ] Atomic arithmetic operators
- [ ] Pre C++20 support
- [ ] More complex examples
- [ ] A function that can be called to query if a spinlock was successfully claimed
- [ ] Somehow write unit tests (still figuring out best approach)

## License
[Apache-2.0](https://choosealicense.com/licenses/apache-2.0/)

This software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.