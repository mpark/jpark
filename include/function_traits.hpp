#include <functional>
#include <type_traits>

#include <meta/meta.hpp>

template <typename T, typename = void>
struct function_traits {};

template <typename T>
struct function_traits<T, meta::void_<decltype(&T::operator())>>
    : function_traits<decltype(&T::operator())> {};

template <typename R, typename... Args>
struct function_traits<R (Args...)> {
  using type = R (Args...);
  using return_type = R;

  template <std::size_t I>
  using argument_type = meta::at_c<meta::list<Args...>, I>;

  static constexpr std::size_t arity = sizeof...(Args);
};

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> : function_traits<R (Args...)> {};

template <typename R, typename T, typename... Args>
struct function_traits<R (T::*)(Args...) const>
    : function_traits<R (Args...)> {};

template <typename R, typename T, typename... Args>
struct function_traits<R (T::*)(Args...)> : function_traits<R (Args...)> {};

template <typename T>
struct function_traits<std::function<T>> : function_traits<T> {};
