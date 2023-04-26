#include <concepts>
#include <cstdint>
#include <cstdio>

#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "pico/stdio.h"

#include "RP2040Atomic.hpp"
#include "hardware/uart.h"
#include "pico/time.h"

using namespace patom::types;

struct {
  struct {
    patomic_int a;
    patomic_int b;
  } Inner;
  patomic_int c;
} Data;

constexpr auto DEBUG_PIN_C0 = 26;
constexpr auto DEBUG_PIN_C1 = 27;

void main1() {
  do {
//    gpio_put(DEBUG_PIN_C1, true);
//    if (is_spin_locked(patomic_int::ct_.spin_lock)) {
//      puts("C1 X");
//    }
//    gpio_put(DEBUG_PIN_C1, false);

//    gpio_put(DEBUG_PIN_C1, true);
    auto a_val = Data.Inner.a.Load();
//    gpio_put(DEBUG_PIN_C1, false);
    ++a_val;
//    gpio_put(DEBUG_PIN_C1, true);
    auto c_val = Data.c.Load();
//    gpio_put(DEBUG_PIN_C1, false);
//    gpio_put(DEBUG_PIN_C1, true);
    Data.Inner.a = a_val;
//    gpio_put(DEBUG_PIN_C1, false);
    ++c_val;
//    gpio_put(DEBUG_PIN_C1, true);
    Data.c = c_val;
//    gpio_put(DEBUG_PIN_C1, false);
  } while (true);
}

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_init(DEBUG_PIN_C0);
  gpio_init(DEBUG_PIN_C1);
  gpio_set_dir(DEBUG_PIN_C0, GPIO_OUT);
  gpio_set_dir(DEBUG_PIN_C1, GPIO_OUT);

  stdio_init_all();
  sleep_ms(5000);

  printf("Lock: %u\n", patom::types::patomic_int::ct_.spin_lock);

  patom::PseudoAtomicInit();
  printf("Lock Init: %u\n", patom::types::patomic_int::ct_.spin_lock);
  auto new_atom = patomic_bool{};
  printf("Lock New: %u\n", new_atom.ct_.spin_lock);
  critical_section_t crit;
  critical_section_init(&crit);
  printf("Lock Crit: %u\n", crit.spin_lock);
  gpio_set_function(4, GPIO_FUNC_UART);  // Set these to whatever pins are used for core 1 UART
  gpio_set_function(5, GPIO_FUNC_UART);
  uart_init(uart1, 115200);  // This UART is used for debug messages from core 1

  gpio_set_function(0, GPIO_FUNC_UART);  // Set these to whatever pins are used for core 1 UART
  gpio_set_function(1, GPIO_FUNC_UART);
  uart_init(uart0, 115200);  // This UART is used for debug messages from core 1

  gpio_put(PICO_DEFAULT_LED_PIN, true);
  sleep_ms(250);
  gpio_put(PICO_DEFAULT_LED_PIN, false);
  sleep_ms(250);

  multicore_launch_core1(main1);

  do {
//    gpio_put(DEBUG_PIN_C0, true);
//    if (is_spin_locked(patomic_int::ct_.spin_lock)) {
//      puts("C0 X");
//    }
//    gpio_put(DEBUG_PIN_C0, false);

//    gpio_put(DEBUG_PIN_C0, true);
    auto a_val = Data.Inner.a.Load();
//    gpio_put(DEBUG_PIN_C0, false);
//    gpio_put(DEBUG_PIN_C0, true);
    auto b_val = Data.Inner.b.Load();
//    gpio_put(DEBUG_PIN_C0, false);
//    gpio_put(DEBUG_PIN_C0, true);
    auto c_val = Data.c.Load();
//    gpio_put(DEBUG_PIN_C0, false);
    ++a_val;
    ++b_val;
    ++c_val;
//    gpio_put(DEBUG_PIN_C0, true);
    Data.Inner.a = a_val;
//    gpio_put(DEBUG_PIN_C0, false);
//    gpio_put(DEBUG_PIN_C0, true);
    Data.Inner.b = b_val;
//    gpio_put(DEBUG_PIN_C0, false);
//    gpio_put(DEBUG_PIN_C0, true);
    Data.c = c_val;
//    gpio_put(DEBUG_PIN_C0, false);
  } while (true);
}
