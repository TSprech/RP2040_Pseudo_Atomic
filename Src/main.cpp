#include "hardware/gpio.h"

#include "pico/multicore.h"
#include "pico/stdio.h"
#include "pico/sync.h"

#include <cstdio>

critical_section_t critical;
volatile int a;
volatile int b;

void main1() {
  do {
    critical_section_enter_blocking(&critical);
    a = a + 1;
    b = b + 1;
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
    critical_section_enter_blocking(&critical);
    if (a != b) {
      gpio_put(PICO_DEFAULT_LED_PIN, true);
    }
  } while (true);
}
