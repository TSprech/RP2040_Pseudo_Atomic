#include <concepts>
#include <cstdint>
#include <cstdio>

#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "pico/stdio.h"

#include "RP2040Atomic.hpp"

struct {
  struct {
    patom::patomic_int a;
    patom::patomic_int b;
  } Inner;
  patom::patomic_int c;
} Data;

void main1() {
  do {
    auto a_val = Data.Inner.a.Load();
    ++a_val;
    Data.Inner.a = a_val;
  } while (true);
}

#define DEBUG_PIN 26

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_init(DEBUG_PIN);
  gpio_set_dir(DEBUG_PIN, GPIO_OUT);

  patom::PseudoAtomicInit();

  gpio_put(PICO_DEFAULT_LED_PIN, true);
  sleep_ms(250);
  gpio_put(PICO_DEFAULT_LED_PIN, false);
  sleep_ms(250);

  stdio_init_all();
  multicore_launch_core1(main1);

  do {
      gpio_put(DEBUG_PIN, true);
      auto a_val = Data.Inner.a.Load();
      gpio_put(DEBUG_PIN, false);
      sleep_us(100);
      printf("a: %i\n", a_val);
  } while (true);
}
