#include <utility>

#define RETURN(...) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

/* C++17 std::invoke */

template <typename F, typename... As>
auto invoke(F &&f, As &&... as)
    RETURN(std::forward<F>(f)(std::forward<As>(as)...));

template <typename B, typename T, typename D>
auto invoke(T B::*pmv, D &&d)
    RETURN(std::forward<D>(d).*pmv);

template <typename Pmv, typename Ptr>
auto invoke(Pmv pmv, Ptr &&ptr)
    RETURN((*std::forward<Ptr>(ptr)).*pmv);

template <typename B, typename T, typename D, typename... As>
auto invoke(T B::*pmf, D &&d, As &&... as)
    RETURN((std::forward<D>(d).*pmf)(std::forward<As>(as)...));

template <typename Pmf, typename Ptr, typename... As>
auto invoke(Pmf pmf, Ptr &&ptr, As &&... as)
    RETURN(((*std::forward<Ptr>(ptr)).*pmf)(std::forward<As>(as)...));

#undef RETURN
