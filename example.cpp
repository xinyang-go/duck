#include <iostream>
#include "duck.hpp"

// 定义Circle类型
struct Circle {
    void print() {
        std::cout << "Circle::print()" << std::endl;
    }
    void draw() {
        std::cout << "Circle::draw()" << std::endl;
    }
};

// 定义Rectangle类型
struct Rectangle {
    void print() {
        std::cout << "Rectangle::print()" << std::endl;
    }
    void draw() {
        std::cout << "Rectangle::draw()" << std::endl;
    }
};

// 定义Print的interface
struct Print : public interface<void(), [](auto &&obj) { obj.print(); }> {
    // 函数别名，使得可以使用duck.print()进行调用
    // 否则需要使用duck.invoke<Print>()进行调用
    const interface::invoke_t &print = interface::invoke;
};

// 定义Draw的interface
struct Draw : public interface<void(), [](auto &&obj) { obj.draw(); }> {
    // 函数别名，使得可以使用duck.draw()进行调用
    // 否则需要使用duck.invoke<Draw>()进行调用
    const interface::invoke_t &draw = interface::invoke;
};

// 定义duck类型
using ShapeDuck = duck<Print, Draw>;

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
    Circle a;
    Rectangle b;
    print_and_draw(a);
    print_and_draw(b);

    return 0;
}
