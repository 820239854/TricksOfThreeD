C内联汇编只能支持32位
## 代码升级到新的编译标准
#include <iostream.h>   ==> #include <iostream>

Chap5代码不与t3dlib2.cpp、t3dlib3.cpp同时编译



运行及Debug时设置兼容模式,颜色模式根据情况设为8或16位



![image-20230406153845632](D:\BaiduNetdiskDownload\3d游戏编程大师技巧源代码\TricksOfThreeD\assets\image-20230406153845632.png)



###### *unresolved external symbol _CLSID_DirectMusicLoader*

*unresolved external symbol _GUID_DirectMusicAllTypes*

> 包含GUID引用错误时考虑

1. 添加dxguid.lib 
2. 在文件开头#define INITGUID

> 参考
https://www.cnblogs.com/0odrifto0/p/3598431.html
