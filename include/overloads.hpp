#pragma once

namespace utils::misc {

template <typename... Ts> struct overloads : Ts... { using Ts::operator()...; };
template <typename... Ts> overloads(Ts...) -> overloads<Ts...>;

template <typename... Ts> struct compose : Ts... {};
template <typename... Ts> compose(Ts...) -> compose<Ts...>;

} // namespace utils::misc