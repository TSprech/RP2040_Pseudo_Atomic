#include <cstdio>

#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "pico/stdio.h"
#include "pico/sync.h"

template <typename T>
class PseudoAtomic {
  class PAReader;

 public:
  auto Initialize() -> void {
    critical_section_init(&ct_);
  }

  auto Writer() -> T& {
    return (state) ? a : b;
  }

  auto Reader() -> const PAReader {
    return (state) ? PAReader{b, ct_} : PAReader{a, ct_};
  }

  auto Swap() -> void {
    critical_section_enter_blocking(&ct_);
    state = !state;
    critical_section_exit(&ct_);
  }

 private:
  critical_section_t ct_{};
  bool state = false;
  T a = 0;
  T b = 0;

  class PAReader {
   public:
    PAReader(const T& var, critical_section_t& ct)
        : ct_(ct), underlying_(var) {
      critical_section_enter_blocking(&ct_);
    }

    auto Get() const -> const T& {
      return underlying_;
    }

    ~PAReader() {
      critical_section_exit(&ct_);
    }

   private:
    critical_section_t& ct_;
    const T& underlying_;
  };
};

PseudoAtomic<int> ps{};

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
