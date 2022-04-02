#include "duck.hpp"
#include <iostream>
#include <vector>
#include <chrono>

// 定义基类
struct Compute {
    virtual int calc(int) = 0;
};

struct Plus final : public Compute {
    // 重载虚函数
    virtual int calc(int x) override {
        return value += x;
    }
    // 非虚函数(用于duck)
    int calc_(int x) {
        return value += x;
    }

    int value = 0;
};

struct Minus final : public Compute {
    // 重载虚函数
    virtual int calc(int x) override {
        return value -= x;
    }
    // 非虚函数(用于duck)
    int calc_(int x) {
        return value -= x;
    }

    int value = 0;
};

// 定义calc_的interface
struct Calc : public interface<Calc, int(int)> {
    using interface::interface;
    // 使用模板成员函数指针指定某个成员函数
    template<typename T>
    static constexpr auto value = &T::calc_;
};
// 定义duck类型
using ComputeDuck = duck<Calc>;

__attribute_noinline__
void invoke_by_duck(const std::vector<ComputeDuck> &vec) {
    int sz = vec.size();
    for(int i=0; i<sz; i++) {
        vec[i].invoke<Calc>(i);
    }
}

__attribute_noinline__
void invoke_by_virtual(const std::vector<Compute*> &vec) {
    int sz = vec.size();
    auto *data = vec.data();
    for(int i=0; i<sz; i++) {
        data[i]->calc(i);
    }
}

void benchmark_duck(int n) {
    Plus plus;
    Minus minus;
    std::vector<ComputeDuck> vec(n);
    for(int i=0; i<n; i++) {
        if(i%2 == 0) vec[i] = plus;
        else vec[i] = minus;
    }
    auto t1 = std::chrono::steady_clock::now();
    invoke_by_duck(vec);
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "duck n=" << n << ", timing="
        << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count() / 1e3
        << "ms" << std::endl;
}

void benchmark_virtual(int n) {
    Plus plus;
    Minus minus;
    std::vector<Compute*> vec(n);
    for(int i=0; i<n; i++) {
        if(i%2 == 0) vec[i] = &plus;
        else vec[i] = &minus;
    }
    auto t1 = std::chrono::steady_clock::now();
    invoke_by_virtual(vec);
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "virtual n=" << n << ", timing="
        << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count() / 1e3
        << "ms" << std::endl;
}

int main() {
    benchmark_duck(100000);
    benchmark_duck(1000000);
    benchmark_duck(10000000);
    benchmark_duck(100000000);
    
    benchmark_virtual(100000);
    benchmark_virtual(1000000);
    benchmark_virtual(10000000);
    benchmark_virtual(100000000);
}
