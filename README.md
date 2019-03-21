基于CS的socket通信

大作业要求：
在Linux下实现一个基于C/S模式的及时通讯系统，要求在以下三种情况下均能正常通信：
1.ClientA在一个子网，CliectB也在同一个子网
2.ClientA在一个子网，CliectB在外网
3.ClientA在一个子网，CliectB在另一个子网

可采用TCP或UDP模型。

扩展功能：
1.实现文件互传
2.实现群组通信

扩展功能并未实现！

USAGE:
gcc xxx.c -o xxx -lpthread 编译两个C文件生成server和client
先启动server，再启动client，client参数是server ip地址
