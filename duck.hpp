#ifndef _DUCK_HPP_
#define _DUCK_HPP_

#include <cstdint>
#include <utility>

template<typename, typename>
class interface {};

template<typename F, typename R, typename ...Args>
class interface<F, R(Args...)> {
public:
    template<typename T, std::enable_if_t<!std::is_same_v<interface, std::decay_t<T>>, bool> = true>
    interface(T &&obj) : invoke(dispatch<std::remove_reference_t<T>>) {}

    interface(const interface& other) = default;

    interface() = default;

protected:
    using invoker_t = R(*)(uintptr_t, Args...);
    invoker_t invoke = nullptr;

private:
    template<typename T>
    static R dispatch(uintptr_t p_obj, Args ...args) {
        auto p_func = F::template value<T>;
        return (reinterpret_cast<T*>(p_obj)->*p_func)(std::forward<Args>(args)...);
    }
};

template<typename ...Is>
class duck : public Is... {
public:
    template<typename T, std::enable_if_t<!std::is_same_v<duck, std::decay_t<T>>, bool> = true>
    duck(T &&obj) : Is(std::forward<T>(obj)) ..., p_obj(reinterpret_cast<uintptr_t>(&obj)) {}

    duck(const duck& other) = default;

    duck() = default;

    template<typename I, typename ...Args>
    auto invoke(Args &&...args) const {
        return I::invoke(p_obj, std::forward<Args>(args)...);
    }

    operator bool() { return p_obj != 0; }

    bool empty() { return p_obj == 0; }

private:
    uintptr_t p_obj = 0;
};

#endif /* _DUCK_HPP_ */
