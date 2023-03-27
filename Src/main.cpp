#include <cstdio>

#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "pico/stdio.h"
#include "pico/sync.h"

/**
 * @brief A pseudo-atomic class which buffers writing to prevent read/write conflicts.
 * @tparam T The type, two instances of T will be constructed in the class.
 */
class PseudoAtomic {
  class PAReader;

 public:
  /**
   * @brief Initializes the spinlock used for synchronization.
   */
  auto Initialize() -> void {
    critical_section_init(&ct_);
  }

  /**
   * @returns A reference of type T which is used to modify the write buffer.
   */
  auto Write(auto& var, auto value) -> void {
    critical_section_enter_blocking(&ct_);
    var = value;
    critical_section_exit(&ct_);
  }

  /**
   * @returns A PAReader object which can be used to access the read buffer.
   * @note The PAReader object locks the semaphore when constructed and unlocks it upon destruction. This means that the lifetime of the return variable determines how long the reader locks to prevent swapping.
   */
  [[nodiscard]]
  auto Read(auto& var) {
    critical_section_enter_blocking(&ct_);
    auto value = var;
    critical_section_exit(&ct_);
    return value;
  }

 private:
  critical_section_t ct_{}; // Critical section used for protecting reading and swapping
};

PseudoAtomic ps{};
struct {
  struct {
    int a;
    int b;
  } Inner;
  int c;
} Data;

void main1() {
  do {
    auto psw = ps.Read(Data.Inner.a);
    psw = psw + 1;
    ps.Write(Data.Inner.a, psw);
    ps.Write(Data.Inner.b, psw);
  } while (true);
}

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  gpio_put(PICO_DEFAULT_LED_PIN, true);
  sleep_ms(250);
  gpio_put(PICO_DEFAULT_LED_PIN, false);
  sleep_ms(250);

  ps.Initialize();

  stdio_init_all();
  multicore_launch_core1(main1);

  do {
    {
      auto psr = ps.Reader();
      printf("PS: %i\n", psr.Get());
    }
    sleep_ms(200);
  } while (true);
}
