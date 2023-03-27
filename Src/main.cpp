#include <concepts>
#include <cstdint>
#include <cstdio>

#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "pico/stdio.h"
#include "pico/sync.h"
#include "pico/time.h"

template <typename T>
concept atomic_t = std::is_trivially_copyable_v<T> && std::is_copy_constructible_v<T> && std::is_move_constructible_v<T> && std::is_copy_assignable_v<T> && std::is_move_assignable_v<T>;

critical_section_t ct{};

template <atomic_t T>
class PseudoAtomic {
 public:
  /**
   * @brief Initializes the spinlock used for synchronization.
   */
  auto Initialize() -> void {
    critical_section_init(&ct_);
  }

  auto operator=(T t) -> PseudoAtomic<T>& {
    critical_section_enter_blocking(&ct_);
    t_ = t;
    critical_section_exit(&ct_);
    return *this;
  }

  auto Load() -> T {
    critical_section_enter_blocking(&ct_);
    auto t = t_;
    critical_section_exit(&ct_);
    return t;
  }

  PseudoAtomic() = default;                       // Required to still allow object to be created
  PseudoAtomic(const PseudoAtomic<T>&) = delete;  // Remove copy construction
  PseudoAtomic(PseudoAtomic<T>&&) = delete;       // Remove move construction

  PseudoAtomic<T>& operator=(const PseudoAtomic<T>&) = delete;  // Remove copy assignment
  PseudoAtomic<T>& operator=(PseudoAtomic<T>&&) = delete;       // Remove move assignment

  inline static critical_section_t ct_ = ct;  // Critical section used for protecting reading and swapping
 private:
  T t_;
};

using patomic_bool = PseudoAtomic<bool>;
using patomic_char = PseudoAtomic<char>;
using patomic_schar = PseudoAtomic<signed char>;
using patomic_uchar = PseudoAtomic<unsigned char>;
using patomic_short = PseudoAtomic<short>;
using patomic_ushort = PseudoAtomic<unsigned short>;
using patomic_int = PseudoAtomic<int>;
using patomic_uint = PseudoAtomic<unsigned int>;
using patomic_long = PseudoAtomic<long>;
using patomic_ulong = PseudoAtomic<unsigned long>;
using patomic_llong = PseudoAtomic<long long>;
using patomic_ullong = PseudoAtomic<unsigned long long>;
using patomic_char8_t = PseudoAtomic<char8_t>;
using patomic_char16_t = PseudoAtomic<char16_t>;
using patomic_char32_t = PseudoAtomic<char32_t>;
using patomic_wchar_t = PseudoAtomic<wchar_t>;
using patomic_int8_t = PseudoAtomic<std::int8_t>;
using patomic_uint8_t = PseudoAtomic<std::uint8_t>;
using patomic_int16_t = PseudoAtomic<std::int16_t>;
using patomic_uint16_t = PseudoAtomic<std::uint16_t>;
using patomic_int32_t = PseudoAtomic<std::int32_t>;
using patomic_uint32_t = PseudoAtomic<std::uint32_t>;
using patomic_int64_t = PseudoAtomic<std::int64_t>;
using patomic_uint64_t = PseudoAtomic<std::uint64_t>;
using patomic_int_least8_t = PseudoAtomic<std::int_least8_t>;
using patomic_uint_least8_t = PseudoAtomic<std::uint_least8_t>;
using patomic_int_least16_t = PseudoAtomic<std::int_least16_t>;
using patomic_uint_least16_t = PseudoAtomic<std::uint_least16_t>;
using patomic_int_least32_t = PseudoAtomic<std::int_least32_t>;
using patomic_uint_least32_t = PseudoAtomic<std::uint_least32_t>;
using patomic_int_least64_t = PseudoAtomic<std::int_least64_t>;
using patomic_uint_least64_t = PseudoAtomic<std::uint_least64_t>;
using patomic_int_fast8_t = PseudoAtomic<std::int_fast8_t>;
using patomic_uint_fast8_t = PseudoAtomic<std::uint_fast8_t>;
using patomic_int_fast16_t = PseudoAtomic<std::int_fast16_t>;
using patomic_uint_fast16_t = PseudoAtomic<std::uint_fast16_t>;
using patomic_int_fast32_t = PseudoAtomic<std::int_fast32_t>;
using patomic_uint_fast32_t = PseudoAtomic<std::uint_fast32_t>;
using patomic_int_fast64_t = PseudoAtomic<std::int_fast64_t>;
using patomic_uint_fast64_t = PseudoAtomic<std::uint_fast64_t>;
using patomic_intptr_t = PseudoAtomic<std::intptr_t>;
using patomic_uintptr_t = PseudoAtomic<std::uintptr_t>;
using patomic_size_t = PseudoAtomic<std::size_t>;
using patomic_ptrdiff_t = PseudoAtomic<std::ptrdiff_t>;
using patomic_intmax_t = PseudoAtomic<std::intmax_t>;
using patomic_uintmax_t = PseudoAtomic<std::uintmax_t>;

//critical_section_t ct{};
//template<> critical_section_t patomic_bool::ct_ = ct;
//template<> critical_section_t patomic_char::ct_ = ct;
//template<> critical_section_t patomic_schar::ct_ = ct;
//template<> critical_section_t patomic_uchar::ct_ = ct;
//template<> critical_section_t patomic_short::ct_ = ct;
//template<> critical_section_t patomic_ushort::ct_ = ct;
//template<> critical_section_t patomic_int::ct_ = ct;
//template<> critical_section_t patomic_uint::ct_ = ct;
//template<> critical_section_t patomic_long::ct_ = ct;
//template<> critical_section_t patomic_ulong::ct_ = ct;
//template<> critical_section_t patomic_llong::ct_ = ct;
//template<> critical_section_t patomic_ullong::ct_ = ct;
//template<> critical_section_t patomic_char8_t::ct_ = ct;
//template<> critical_section_t patomic_char16_t::ct_ = ct;
//template<> critical_section_t patomic_char32_t::ct_ = ct;
//template<> critical_section_t patomic_wchar_t::ct_ = ct;
//template<> critical_section_t patomic_int8_t::ct_ = ct;
//template<> critical_section_t patomic_uint8_t::ct_ = ct;
//template<> critical_section_t patomic_int16_t::ct_ = ct;
//template<> critical_section_t patomic_uint16_t::ct_ = ct;
//template<> critical_section_t patomic_int32_t::ct_ = ct;
//template<> critical_section_t patomic_uint32_t::ct_ = ct;
//template<> critical_section_t patomic_int64_t::ct_ = ct;
//template<> critical_section_t patomic_uint64_t::ct_ = ct;
//template<> critical_section_t patomic_int_least8_t::ct_ = ct;
//template<> critical_section_t patomic_uint_least8_t::ct_ = ct;
//template<> critical_section_t patomic_int_least16_t::ct_ = ct;
//template<> critical_section_t patomic_uint_least16_t::ct_ = ct;
//template<> critical_section_t patomic_int_least32_t::ct_ = ct;
//template<> critical_section_t patomic_uint_least32_t::ct_ = ct;
//template<> critical_section_t patomic_int_least64_t::ct_ = ct;
//template<> critical_section_t patomic_uint_least64_t::ct_ = ct;
//template<> critical_section_t patomic_int_fast8_t::ct_ = ct;
//template<> critical_section_t patomic_uint_fast8_t::ct_ = ct;
//template<> critical_section_t patomic_int_fast16_t::ct_ = ct;
//template<> critical_section_t patomic_uint_fast16_t::ct_ = ct;
//template<> critical_section_t patomic_int_fast32_t::ct_ = ct;
//template<> critical_section_t patomic_uint_fast32_t::ct_ = ct;
//template<> critical_section_t patomic_int_fast64_t::ct_ = ct;
//template<> critical_section_t patomic_uint_fast64_t::ct_ = ct;
//template<> critical_section_t patomic_intptr_t::ct_ = ct;
//template<> critical_section_t patomic_uintptr_t::ct_ = ct;
//template<> critical_section_t patomic_size_t::ct_ = ct;
//template<> critical_section_t patomic_ptrdiff_t::ct_ = ct;
//template<> critical_section_t patomic_intmax_t::ct_ = ct;
//template<> critical_section_t patomic_uintmax_t::ct_ = ct;

auto PseudoAtomicInit() -> void {
  critical_section_init(&ct);
}

struct {
  struct {
    patomic_int a;
    patomic_int b;
  } Inner;
  patomic_int c;
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

  PseudoAtomicInit();

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
