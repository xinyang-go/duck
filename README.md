# 50行代码实现C++非继承动态多态(duck type)

What is duck type?

*如果一个东西像鸭子一样走路和呱呱叫，那么它一定是鸭子。*

---

静态duck type可以使用template实现。在下面这个例子中，凡是实现了draw成员函数的对象，都可以调用draw_shape这个函数。

```cpp
template<typename T>
void draw_shape(T &&shape) {
   shape.draw();
}
```

动态duck type传统方式是使用virtual函数和继承实现。在下面这个例子中，Circle和Rectangle都可以传给draw_shape这个函数。

```cpp
struct Shape {
    void draw() = 0;
};
struct Circle : public Shape {
    void draw() override {
        std::cout << "Circle::draw()" << std::endl;
    }
};
struct Rectangle : public Shape {
    void draw() override {
        std::cout << "Rectangle ::draw()" << std::endl;
    }
};
void draw_shape(Shape &shape) {
    shape.draw();
}
```

---

而本代码的动态duck type实现方式不需要继承。在下面这个例子中，凡是实现了draw成员函数的对象，都可以调用draw_shape这个函数。

```cpp
// 定义一个interface，类似virtual function
struct Draw : public interface<Draw, void()> {
    using interface::interface;
    // 使用模板成员函数指针指定某个成员函数
    template<typename T>
    static constexpr auto value = &T::draw;
};
// 定义一个duck type，由若干个interface构成
using ShapeDuck = duck<Draw>;

void draw_shape(ShapeDuck shape) {
    shape.invoke<Draw>();
}
```

如果觉得上述 ``shape.invoke<Draw>()``的调用方式过于啰嗦，也可以手动起函数别名，以实现 ``shape.draw()``的调用方式，如下例所述。

```cpp
// 定义一个interface，类似virtual function
struct Draw : public interface<Draw, void()> {
    using interface::interface;
    // 使用模板成员函数指针指定某个成员函数
    template<typename T>
    static constexpr auto value = &T::draw;
};
// 定义duck类型
struct ShapeDuck : public duck<Draw> {
    using duck::duck;
    // 手动起函数别名
    void draw() { return duck::invoke<Draw>(); }
};

void draw_shape(ShapeDuck shape) {
    shape.draw();
}
```

---

与虚函数进行对比：

* duck不需要虚函数与继承，便于用于第三方库。如std::array和std::vector都有operator[]，但本身并不存在虚函数和继承关系。
* duck使得虚表和对象数据分离，不会影响到对象数据的二进制分布。
* ~~duck采用std::function实现，效率相对虚函数略低。~~ 已优化，目前O2编译下调用效率与虚函数基本持平。
* duck“虚表”为动态生成，需要消耗一定时间。

---

原理简介：使用 ``uintptr_t``保存对象指针，以擦除类型信息。“虚表”为一个函数指针，在构造duck对象时动态创建。整个duck内部有N+1个指针数据，其中N为interface的个数。

注意：

* duck内部是对实际对象的**引用**，务必保证**实际对象的生命周期大于duck**。
* 如果在duck初始化前调用invoke会导致程序崩溃。
* 需要C++20支持。

---

TODO

* [X] 优化调用效率。
* [ ] 完善concept和static_assert，优化编译报错信息。
* [ ] 提供统一的静态多态和动态多态接口。
