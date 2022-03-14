# 40行代码实现C++非继承动态多态(duck type)

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
using Draw = interface<void(), [](auto &&obj) { obj.draw(); }>
// 定义一个duck type，由若干个interface构成
using ShapeDuck = duck<Draw>;

void draw_shape(ShapeDuck shape) {
    shape.invoke<Draw>();
}
```

如果觉得上述 ``shape.invoke<Draw>()``的调用方式过于啰嗦，也可以手动起函数别名，以实现 ``shape.draw()``的调用方式，如下例所述。

```cpp
// 定义一个interface，类似virtual function
struct Draw : public interface<void(), [](auto &&obj) { obj.draw(); }> {
    // 定义别名
    const interface::invoke_t &draw = interface::invoke;
};
// 定义一个duck type，由若干个interface构成
using ShapeDuck = duck<Draw>;

void draw_shape(ShapeDuck shape) {
    shape.draw();
}
```

---

与虚函数进行对比：

* duck不需要虚函数与继承，便于用于第三方库。如std::array和std::vector都有operator[]，但本身并不存在虚函数和继承关系。
* duck使得虚表和对象数据分离，不会影响到对象数据的二进制分布。
* duck采用std::function实现，效率相对虚函数略低，同时“虚表”为动态生成，也需要消耗一定时间。
* duck内部直接保存整个“虚表”，而非“虚表”指针，故duck对象的拷贝效率略低。

---

原理简介：使用std::function擦除类型信息，形成“虚表”。“虚表”在构造duck对象时动态创建。

注意：duck内部是对实际对象的**引用**，务必保证**实际对象的生命周期大于duck**。

---

TODO

* [ ] 优化调用效率和拷贝效率。
* [ ] 完善concept和static_assert，优化编译报错信息。
