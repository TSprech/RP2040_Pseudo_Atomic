//
// Created by treys on 2023/03/10 20:42.
//

#ifndef FMTTOUART_HPP
#define FMTTOUART_HPP

#include "fmt/core.h"
#include "fmt/format.h"
#include "hardware/uart.h"
#include "pico/platform.h"

inline auto UARTPrint(std::string_view str) -> void {
  if (get_core_num() == 0) {
    for (auto letter : str) uart_putc(uart0, letter);
  } else {
    for (auto letter : str) uart_putc(uart1, letter);
  }
}

//template <typename... T>
//inline auto FMTDebug(fmt::format_string<T...> fmt, T&&... args) -> void {
//  fmt::memory_buffer buffer;
//  fmt::vformat_to(std::back_inserter(buffer), fmt, fmt::make_format_args(args...));
////  UARTPrint(((get_core_num() == 0) ? uart0 : uart1), std::string_view{buffer.data(), buffer.size()});
//  UARTPrint(uart1, std::string_view{buffer.data(), buffer.size()});
//}

inline auto UARTVPrint(fmt::string_view format_str, fmt::format_args args) -> void {
  fmt::memory_buffer buffer;
  fmt::detail::vformat_to(buffer, format_str, args);
  UARTPrint({buffer.data(), buffer.size()});
}

template <typename... T>
inline auto FMTDebug(fmt::format_string<T...> fmt, T&&... args) -> void {
  const auto& vargs = fmt::make_format_args(args...);
  UARTVPrint(fmt, vargs);
  //  return UARTVPrint(((get_core_num() == 0) ? uart0 : uart1), fmt, vargs); // get_core_num crashes the program for some reason
//  if (get_core_num() == 0) {
//    UARTVPrint(uart0, fmt, vargs);
//  } else {
//    UARTVPrint(uart1, fmt, vargs);
//  }
}

//template <typename... T>
//inline auto FMTDebug(fmt::format_string<T...> fmt, T&&... args) {
//  fmt::memory_buffer buf;
//  fmt::vformat_to(std::back_inserter(buf), fmt, fmt::make_format_args(args...));
//
//  //  if (get_core_num() == 0) {  // Check which core called for the debug and output to the appropriate UART
//  //    //  for (auto* data = buf.begin(); data != end; ++data) {
//  //    for (auto i = 0; i < buf.size(); ++i) {
//  //      uart_putc(uart0, buf[i]);
//  //    }
//  //  } else {
//  for (auto i = 0; i < buf.size(); ++i) {
//    uart_putc(uart1, buf[i]);
//  }
//  //  }
//}

#endif  // FMTTOUART_HPP
