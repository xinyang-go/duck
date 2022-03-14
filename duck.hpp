#ifndef _DUCK_HPP_
#define _DUCK_HPP_

#include <functional>

template<typename F, auto func>
class interface {};

template<typename R, typename ...Args, auto func>
class interface<R(Args...), func> {
public:
    template<typename T>
    interface(T &&obj) : invoke(warp(std::forward<T>(obj))) {}

    template<typename T>
    interface &operator=(T &&obj) {
        invoke = warp(std::forward<T>(obj));
        return *this;
    }

protected:
    using invoke_t = std::function<R(Args...)>;

    invoke_t invoke;

private:
    template<typename T>
    static auto warp(T &&obj) {
        return [&](Args &&...args) {
            return func(std::forward<T>(obj), std::forward<Args>(args)...);
        };
    }
};

template<typename ...Is>
struct duck : public Is... {
    template<typename T>
    duck(T &&obj) : Is(std::forward<T>(obj)) ... {}

    template<typename I, typename ...Args>
    auto invoke(Args &&...args) {
        return I::invoke(std::forward<Args>(args)...);
    }
};

#endif /* _DUCK_HPP_ */
