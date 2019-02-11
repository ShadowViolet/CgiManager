# CgiManager
C++Web框架
可以方便的使用C++制作web

Linux下的安装与使用:
首先安装依赖的工具，包括：

- gcc
- g++
- make

然后获取并安装 CgiManager 库

```bash
git@github.com:ShadowViolet/CgiManager.git
cd CgiManager
make
make install
```

在任意位置新建 main.cpp，以基本的 hello world 为例

```c++
#include <CgiManager/CgiManager.h>

int cgiMain()
{
    CController::OutPut("Hello world!");

    return true;
}
```

然后编译并生成可执行文件

```bash

# 编译
g++  main.cpp -lpthread -lcgi_manager -ldl -o index.cgi

即可得到index.cgi 

Windows 下 使用 Visual Studio 编译即可。

1.创建一个空项目

2.新建一个源文件 

3.设置项目属性 附加包含CgiManager 的目录

4.写一个简单的Hello Word 

#include "CgiManager.h"

int cgiMain()
{
    CController::OutPut("Hello world!");

    return true;
}

5.添加CgiManager引用 

6.按F7编译

具体可查看wiki: https://github.com/ShadowViolet/CgiManager/wiki/How-To-Use%3F