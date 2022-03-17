#include <iostream>
#include "duck.hpp"

// 定义Circle类型
struct Circle {
    void print() const {
        std::cout << "Circle[" << x << "]::print()" << std::endl;
    }
    void draw() const {
        std::cout << "Circle[" << x << "]::draw()" << std::endl;
    }

    int x;
};

// 定义Rectangle类型
struct Rectangle {
    void print() {
        std::cout << "Rectangle[" << x << "]::print()" << std::endl;
    }
    void draw() {
        std::cout << "Rectangle[" << x << "]::draw()" << std::endl;
    }

    int x;
};

// 定义Print的interface
struct Print : public interface<Print, void()> {
    using interface::interface;
    // 使用模板成员函数指针指定某个成员函数
    template<typename T>
    static constexpr auto value = &T::print;
};

// 定义Draw的interface
struct Draw : public interface<Draw, void()> {
    using interface::interface;
    // 使用模板成员函数指针指定某个成员函数
    template<typename T>
    static constexpr auto value = &T::draw;
};

// 定义duck类型
struct ShapeDuck : public duck<Print, Draw> {
    using duck::duck;

    // 手动起函数别名
    void print() { return duck::invoke<Print>(); }
    void draw() { return duck::invoke<Draw>(); }
};

// 使用duck类型
void print_and_draw(ShapeDuck shape) {
    // 调用方法一
    shape.invoke<Print>();
    shape.invoke<Draw>();
    // 调用方法二，需要手动起函数别名
    shape.draw();
    shape.print();
}

int main() {
    const Circle a{123};
    Rectangle b{321};
    print_and_draw(a);
    print_and_draw(b);

    ShapeDuck shape;

    shape = a;
    shape.draw();
    shape.print();

    shape = b;
    shape.draw();
    shape.print();

    return 0;
}
