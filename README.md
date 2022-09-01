# think库

#### 介绍
以前在sourceforge上发布的C语言跨平台开发库，现在转到这里，支持Linux、AIX、HP-UX、Solaris、FreeBSD、MacOSX、Windows等种操作系统，涵盖dbf数据库操作、参数配制（跟boost一样集成了ini文件、命令行选项、环境变量三种来源）操作、进程通讯（共享内存、信号量、锁、共享内存队列等）、网络通讯异步框架（集成了select、poll、epoll、kevent四种技术）及基于think库开发的一些工具（netpeek、udprecv、udpsend、glpeek）与服务（logserver、logsend、logview）。

#### 目录结构说明
##### bin - 编译后的程序
##### etc - 服务配制文件
##### src - 源代码
##### think 库代码
  	think_cfg.c - cfg配制文件操作接口
  	think_utility.c - env、命令行参数等操作接口
  	think_conf.c - 某分割符分割的简单数据操作接口
  	think_thread.c - 线程操作接口封装
  	think_shm.c - 共享内存接口封装
  	think_sem.c - 信号量接口封装
  	think_threadmutex - 线程级锁封装
  	think_mutex.c - 进程级锁封装
    think_net.c - socket通讯基础接口封装
    think_netcenter.c - 网络IO事件异步调度框架
    think_timecenter.c - 时间事件异步调度框架
    think_gl.c - BML语言C接口
    think_db.c - 类似DBF的文件数据库
    think_log.c - 日志接口（支持本地及网络两种存储方式，存储文件均为think_db文件数据库）
    think_shmq.c - 共享内存队列接口
    Makefile - linux下的make文件，下同
    win32 - Windows下的工程文件（含nmake脚本），下同

##### think_dbf - DBF数据库操作接口
##### tools - 基于think库开发的各种工具（日志工具、tcp/udp工具、BML调试工具、配置文件工具、数据库操作工具等）
##### logserver - 日志服务器（使用think_db存储日志信息）
##### Makefile - Linux下的批量编译脚本，直接make就行了
##### win32 - Windows下的批量编译脚本，打开VS命令行make一下就行了

#### 工具用法说明
##### udpsend、udprecv工具用法：https://zhuanlan.zhihu.com/p/358596742
##### netpeek工具用法：https://mp.weixin.qq.com/s?__biz=Mzk0ODI0NDE2Ng==&mid=2247484304&idx=1&sn=6a33754bb39df91f3fa6f67717d27d27&chksm=c36bd839f41c512f46b298a00ff431ef5e9b8cf0e381d0d3a6470319aa2935f20a43243b958c&token=749880663&lang=zh_CN#rd

#### think_cfg配置文件格式说明
##### think_cfg配置文件格式，兼容基本的ini格式，作了很多扩展。
##### 配制文件规则：
- 后者覆盖前者（后面的配制覆盖前面的配制，类似SHELL，后面定义的值会覆盖前面定义的值）
- 别名（值以$符号开头的，将取$后面的配制值，相当于变量，如果觉得名字起得不好，可以再起一个别名）
- 继承（带域的配制将继承全局配制，不带域或域为空即为全局配制，域即用[]括起来的部分，域可以同名，像C++的public成员一样，多写几段public也一样）
- 嵌套（类似C语言的include，无限嵌套）
- 实时赋值（类似shell，后面的配制将根据前面的配制，立即赋值）
- 就近取值（带域的别名取值时，先在域内查找名字，找不到时再找全局的配制，其实和继承一个意思）
- 搜索路径（在指定的一些路径下查找配制文件，类似C语言的头文件查找路径）
- 注释（#后的字符为注释，#在行首且后跟include的除外）
- 宽松格式（行首，行尾，域名前后（不包括中间），#号前后，$号前后，=号前后，配制名前后，配制值前后，这些地方的空白符（空格、TAB符）都将被忽略）

```
例：
配制文件1：global.cfg：
lsnip=127.0.0.1			#此配制不属于任何域，因此为全局配制，类似全局变量，带域的变量称为局部变量，局部变量将覆盖全局变量
logaddr=tcp://127.0.0.1:10000
logflag=debug,print		# debug,normal,info,key,warn,error,fatal
				          # trunc,excl,print,quiet

[specialcfg]			# 这里开始为局部变量
name=krenx
email=krenx@qq.com

配制文件2：logserver.cfg:
#include <global.cfg>           #引入另一个配制文件global.cfg
[logserver]
logname=logserver
logaddr=			# logaddr将为空，而不是tcp://127.0.0.1:10000
#logflag=			# [logserver].logflag将为debug,print
ip=$lsnip			# ip将取值为127.0.0.1
port=10000
port=10008                      #port将取值10008
logpath=../log
```
