// RUN: %clang_analyze_cc1 -analyzer-checker=core -std=c++20 -verify %s
// expected-no-diagnostics

namespace std {
  typedef decltype(sizeof(int)) size_t;
}

namespace android::ftl::details {
  template <typename T> struct StaticString;
  
  template <> struct StaticString<int> {
    static constexpr std::size_t N = 10;
    constexpr StaticString(int) {}
  };

  template <> struct StaticString<const char*> {
    static constexpr std::size_t N = 20;
    constexpr StaticString(const char*) {}
  };

  template <std::size_t M> struct StaticString<const char (&)[M]> {
    static constexpr std::size_t N = M - 1;
    constexpr StaticString(const char (&)[M]) {}
  };
}

namespace android::ftl {
  template <std::size_t N, typename... Ts> struct Concat;

  template <std::size_t N, typename T, typename... Ts>
  struct Concat<N, T, Ts...> : Concat<N + details::StaticString<T>::N, Ts...> {
    constexpr Concat(T v, Ts... args) : Concat<N + details::StaticString<T>::N, Ts...>(args...) {}
  };

  template <std::size_t N>
  struct Concat<N> {
    constexpr Concat() {}
    char buffer[N + 1];
  };

  template <typename... Ts>
  Concat(Ts...) -> Concat<0, Ts...>;
}

void reproduce(int iteration) {
    android::ftl::Concat trace("TimerIteration #", iteration);
    (void)trace;
}
