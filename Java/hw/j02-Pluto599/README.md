[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/VhyNaHej)
# J02 - 231220105 刘佳璇

## 1. 写下对代码工作原理的理解
```java
SteganographyClassLoader loader = new SteganographyClassLoader(
new URL("https://i.postimg.cc/02PpNtdJ/hulu-Bubble-Sorter.png"));
```
- 创建隐写类加载器实例，传入隐写了BubbleSorter类的字节码编码的图片URL。
```java
Class c = loader.loadClass("hulu.BubbleSorter");
```
- 调用`loadClass`方法加载`hulu.BubbleSorter`类，返回对应的`Class`对象。当Java虚拟机找不到该类时，会调用`SteganographyClassLoader`的`findClass`方法，从图片中提取字节码并将其转换为Java类对象。
```java
Sorter sorter = (Sorter) c.newInstance();
```
- 使用`newInstance`方法创建`hulu.BubbleSorter`类的实例，并将其赋值给`Sorter`类型的变量`sorter`。
```java
sorter.load(bros);
sorter.sort();

sortSteps = this.parsePlan(sorter.getPlan());
```
- 使用`BubbleSort`算法对葫芦娃数组进行排序，并记录排序过程中记录的步骤。

## 2. 实验过程与结果
- 添加`hulu/InsertSorter.java`
- 修改`class SteganographyFactory`的`main`方法
```java
public static void main(String[] args) throws IOException {

	SteganographyFactory.getSteganography("hulu/InsertSorter.java", "resources/bubble.jpeg");

}
```
- 运行`SteganographyFactory`的`main`方法，生成隐写术图`hulu.InsertSorter.png`，并上传至`postimg.cc`
- 修改`World()`构造函数，用classloader加载InsertSorter，得到排序结果。
```java
SteganographyClassLoader loader = new SteganographyClassLoader(
			new URL("https://i.postimg.cc/50vcc4fk/hulu-Insert-Sorter.png"));

	Class c = loader.loadClass("hulu.InsertSorter");
```

## 3. GitHub 运行失败的原因与解决方法
- 原因：在GitHub运行时无法访问外部网络，导致无法下载隐写术图片，从而无法加载`InsertSorter`类
- 解决方法：修改`World()`构造函数，从本地加载生成的隐写术图片`hulu.InsertSorter.png`
```java
// SteganographyClassLoader loader = new SteganographyClassLoader(
//         new URL("https://i.postimg.cc/50vcc4fk/hulu-Insert-Sorter.png"));
URL imgUrl = World.class.getClassLoader().getResource("hulu.InsertSorter.png");
SteganographyClassLoader loader = new SteganographyClassLoader(imgUrl);
```