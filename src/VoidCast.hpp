#pragma once
#include <variant>

template <typename T>
struct VoidCast {
    using type = T;
};

#define SPECIALIZE(t) \
template <> \
struct VoidCast<t> { \
    using type = std::monostate; \
}

SPECIALIZE(void);
SPECIALIZE(const void);
SPECIALIZE(volatile void);
SPECIALIZE(const volatile void);

#undef SPECIALIZE

template <typename T>
using VoidCastT = typename VoidCast<T>::type;

template <typename CFunc, typename... TArgs>
decltype(auto) safeInvoke(CFunc&& func, TArgs&&... args) {
    using TRet = std::invoke_result_t<CFunc, TArgs...>;
    
    if constexpr (std::is_void_v<TRet>) {
        std::invoke(
            std::forward<CFunc>(func),
            std::forward<TArgs>(args)...
        );
        return std::monostate();
    } else {
        return std::invoke(
            std::forward<CFunc>(func),
            std::forward<TArgs>(args)...
        );
    }
}
