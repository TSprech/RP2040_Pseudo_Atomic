#include "hardware/gpio.h"

#include "pico/multicore.h"
#include "pico/stdio.h"
#include "pico/sync.h"

#include <cstdio>

critical_section_t critical;

class PsuedoAtomic {
 public:
  class PAReader {
   public:
    PAReader(const int& var) : underlying(var) {
      printf("Acquiring Spinlock\n");
      critical_section_enter_blocking(&critical);
    }

    auto Get() const -> const int& {
      return underlying;
    }

    ~PAReader() {
      printf("Releasing Spinlock\n");
      critical_section_exit(&critical);
    }

   private:
    const int& underlying;
  };

  auto Writer() -> int& {
    return (state) ? a : b;
  }

  auto Reader() const -> const PAReader {
    return (state) ? PAReader{b} : PAReader{a};
  }

  auto Swap() -> void {
    critical_section_enter_blocking(&critical);
    state = !state;
    critical_section_exit(&critical);
  }
 private:
  bool state = false;
  int a = 0;
  int b = 0;
};

PsuedoAtomic ps{};

void main1() {
  do {
    auto& psw = ps.Writer();
    psw = psw + 1;
    ps.Swap();
  } while (true);
}

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  gpio_put(PICO_DEFAULT_LED_PIN, true);
  sleep_ms(250);
  gpio_put(PICO_DEFAULT_LED_PIN, false);
  sleep_ms(250);

  critical_section_init(&critical);

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
