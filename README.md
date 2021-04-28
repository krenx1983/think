# think库

#### 介绍
以前在sourceforge上发布的跨平台开发库，现在转到这里，支持Linux,AIX,HP-UX,Solaris,FreeBSD,MacOSX,Windows，网络通讯的封装做的还可以，一直在用，支持select、poll、epll、kevent等各种模式

#### 结构说明
src 源代码
  think 库代码
    think_net.c 网络通讯基础代码
    think_netcenter.c 异步IO通讯框架代码
    Makefile linux下的make文件，下同
    win32 Windows下的工程文件（含nmake脚本），下同

  think_dbf DBF数据库操作代码
  tools 基于think库开发的各工具代码
  logserver 日志服务器代码

  Makefile Linux下的批量编译脚本
  win32 Windows下的批量编译脚本
  

