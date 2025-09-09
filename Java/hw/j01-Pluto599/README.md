[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/otcw4w2y)

# J01 - 231220105 刘佳璇

## 面向对象思想

### 封装

封装是指将数据和操作数据的方法绑定在一起，并隐藏对象的内部实现细节，只暴露必要的接口。能够保护数据不被外界随意修改。比如：

- `Tile`类中的`xPos`和`yPos`属性被封装，外部通过`getxPos`方法和`getyPos`方法获取，通过`setxPos`方法和`setyPos`方法修改
- `Thing`类中的`tile`属性被封装，外部通过`getX`方法和`getY`方法获取`tile`的`xPos`和`yPos`属性

### 继承

继承是指子类可以继承父类的属性和方法，并可以扩展或重写。比如：

- `Grass`类继承`Thing`类
- `Creature`类继承`Thing`类，添加了`moveTo`方法

- `Hulu`类继承`Creature`类，重写了`toString`方法，添加了`swap`等方法

### 多态

多态是指同一个方法调用，表现出不同的行为。比如：

- `BubbleSorter`类和`InsertSorter`类分别实现了`Sorter`接口，重写不同的了`sort`方法。程序执行时根据传入的参数实现不同的排序。
