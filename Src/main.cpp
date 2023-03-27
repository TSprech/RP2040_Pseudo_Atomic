#include <cstdio>

#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "pico/stdio.h"
#include "pico/sync.h"

/**
 * @brief A pseudo-atomic class which buffers writing to prevent read/write conflicts.
 * @tparam T The type, two instances of T will be constructed in the class.
 */
template <typename T>
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
  [[nodiscard]]
  auto Writer() -> T& {
    return (state) ? t0 : t1; // Determines which of the variables to give to the writer core (the other will be given to the reader)
  }

  /**
   * @returns A PAReader object which can be used to access the read buffer.
   * @note The PAReader object locks the semaphore when constructed and unlocks it upon destruction. This means that the lifetime of the return variable determines how long the reader locks to prevent swapping.
   */
  [[nodiscard]]
  auto Reader() -> PAReader {
    return PAReader{(state) ? t1 : t0, ct_}; // This logic is opposite to the writer to ensure they access opposite variables
  }

  /**
   * @brief Swaps the read and write buffers if the reader is not locking the spinlock.
   * @note This should be called by the writer as only it knows when it is done writing, and the spinlock protects the reader.
   * @remarks The swap function is used as it provides a fast way to update the read buffer as the reader is only prevented from reading during this short method.
   */
  auto Swap() -> void {
    critical_section_enter_blocking(&ct_); // Lock the reader from reading (or wait for the reader to be done then lock)
    state = !state; // Swap which reference will be given to the writer and reader
    critical_section_exit(&ct_); // Allow the reader to read again
  }

 private:
  critical_section_t ct_{}; // Critical section used for protecting reading and swapping
  bool state = false; // Determines which of the below references is returned to the reader and writer
  T t0 = 0; // One variable which can be used for reading or writing
  T t1 = 0; // The other variable which can be used for reading or writing

  /**
   * @todo Remove copy construction <probably>
   */
  class PAReader {
   public:
    /**
     * @brief Construction locks the spinlock, preventing the read variable from being swapped while reading occurs.
     * @param var The internal variable which is currently designated for the reader.
     * @param ct The critical section which will be entered upon construction and exited upon destruction.
     */
    PAReader(const T& var, critical_section_t& ct)
        : ct_(ct), underlying_(var) {
      critical_section_enter_blocking(&ct_);
    }

    /**
     * @returns The reference which can be used to read the current reader data.
     */
    [[nodiscard]]
    auto Get() const -> const T& {
      return underlying_;
    }

    /**
     * @brief Destruction should release the critical section and allow @ref Swap to happen.
     */
    ~PAReader() {
      critical_section_exit(&ct_);
    }

   private:
    critical_section_t& ct_; // The critical section entered when reading
    const T& underlying_; // The variable being read from.
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
