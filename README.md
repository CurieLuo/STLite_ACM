# STLite-ACM

CS1951 assignment, ACM Class, SJTU

## 实现要求

要求实现与 C++ 标准库类似的数据结构，包括迭代器等。框架接口在已本仓库中给出，只需要实现 `.hpp` 文件内所要求的内容即可。

请注意，在本作业中，只允许使用 `cstdio`，`cstring`，`iostream`，`cmath` 四个 C++ 标准库，如需使用其他功能请自行实现。在下发代码中有 `exceptions.hpp` 和 `utility.hpp` 两个辅助文件，你需要使用其中的异常处理和 `sjtu::pair` 完成 STLite 的实现，但不允许修改其中的任何内容。

本学期要求完成三个容器：`sjtu::vector`，`sjtu::priority_queue`，`sjtu::map`.

本作业中的三个容器在接口和复杂度要求上与 STL 基本一致，不同部分详见各自的文件夹。

如果对内容有任何疑问，可以参考 STL，或者询问对应负责助教。

最后，我们会检查内存泄漏。


## 评测方法

测试数据将全部下发（见本仓库），供调试使用。最终测试将会在OJ上进行，并进行code review。

## 分数构成

`vector`：4%

`priority_queue`：5%

`map`：6%

## 截止日期

`vector`：3月5日（第三周周日）18:30前

`priority_queue`：未发布

`map`：未发布

## 迟交惩罚

待定
