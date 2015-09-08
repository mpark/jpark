#include <utility>

#include <meta/meta.hpp>

#define RETURN(...) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

/* C++17 std::apply */

template <typename F, typename Args, size_t... Is>
auto apply_impl(F &&f, Args &&args, meta::index_sequence<Is...>)
  RETURN(std::forward<F>(f)(std::get<Is>(std::forward<Args>(args))...))

template <typename F, typename Args>
auto apply(F &&f, Args &&args)
  RETURN(apply_impl(std::forward<F>(f),
                    std::forward<Args>(args),
                    meta::make_index_sequence<
                        std::tuple_size<meta::_t<std::decay<Args>>>::value>()))

#undef RETURN
