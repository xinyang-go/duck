
#include "duck.hpp"
#include <iostream>
#include <chrono>

struct Compute {
    virtual int calc(int) = 0;
};

struct Plus final : public Compute {
    virtual int calc(int x) override {
        return value += x;
    }

    int value = 0;
};

struct Minus final : public Compute {
    virtual int calc(int x) override {
        return value -= x;
    }

    int value = 0;
};

struct Calc : public interface<int(int), [](auto &&obj, int x) { return obj.calc(x); }> {};
using ComputeDuck = duck<Calc>;

int duck_compute(ComputeDuck compute, int x) {
    return compute.invoke<Calc>(x);
}

int virtual_compute(Compute &compute, int x) {
    return compute.calc(x);
}

void benchmark_duck(int n) {
    Plus plus;
    Minus minus;
    auto t1 = std::chrono::steady_clock::now();
    for(int i=0; i<n; i++) {
        duck_compute(plus, i);
        duck_compute(minus, i);
    }
    auto t2 = std::chrono::steady_clock::now();
    double ms = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count() / 1e3;
    int result = plus.value - minus.value;
    std::cout << "duck of n=" << n << ", timing=" << ms << "ms"
            << ", result=" << result << std::endl;
}

void benchmark_virtual(int n) {
    Plus plus;
    Minus minus;
    auto t1 = std::chrono::steady_clock::now();
    for(int i=0; i<n; i++) {
        virtual_compute(plus, i);
        virtual_compute(minus, i);
    }
    auto t2 = std::chrono::steady_clock::now();
    double ms = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count() / 1e3;
    int result = plus.value - minus.value;
    std::cout << "virtual of n=" << n << ", timing=" << ms << "ms"
              << ", result=" << result << std::endl;
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
