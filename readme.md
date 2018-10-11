## 介绍(for None chinese see [below](https://github.com/ZenLiuCN/zcef/#what))
基于 cef 2623[bin包下载](https://github.com/ZenLiuCN/zcef/releases/tag/v0.pre) 混合go 的桌面H5应用环境
### 特点
1. 内置websocket 支持控制窗口(边框全屏等)
2. 内置http服务器支持本地页面服务
3. 内置websocket 支持本地数据库操作(sqlit3)
4. ...
### 编译环境
1. mingw32 或者mingw64-32
2. go 1.10 32位(支持xp)
3. 依赖的go包[这里](https://github.com/ZenLiuCN/zcef/#go-libs-depend-on)
4. cmake
5. llvm+clang(可选)
### 编译过程
1. 下载cef包放到cef_lib下
2. go get 依赖的go包
3. 首先编译 target goserver (也可手动go编译`go build -o goserver.a --buildmode=c-archive .`)
4. 然后cmake 编译 target cef 

### 应用
1. 窗口控制
  websocket连接 ws://127.0.0.1:65530/win
 ...待完善

## WHAT
this is a project build top on cef3 
1. contains c++ app (from cef3 offical example)
2. contains embed go websocket server(expose window action to js)
3. with MP3,MP4[H.264] supported(just for test)
## WHY
Being working on some desktop solution need on winXP ;

Test use [sumorf/cef](https://github.com/sumorf/cef) is fine,but it use old version of cef3,and when trying to upgrad to cef3.2623 aways have something go wrong (took days).(PS: the websocket server can be use with it).

Finally I try just make a C++ application with go server lib , so the repo came out!

## NOTE
already test on windows 7 x64 ,And it may only support windows 

## HOW
1. get mingw64 for 32bit
1. get cmake 3.12+
1. [optional] get clang(llvm) 
1. get go 1.8+ for 32bit
1. get cef3 binary form [binary](https://github.com/ZenLiuCN/zcef/releases/tag/v0.pre)
1. build go lib via `./goserver/build.bat`
1. build app via cmake 

## go libs depend on
1. [lxn/win](https://github.com/lxn/win) for easy winapi control
1. [gorilla/websocket](https://github.com/gorilla/websocket) for websocket process
1. [mattn/go-sqlite3](https://github.com/mattn/go-sqlite3) for database process

## see some thing like in spring boot [ZenLiuCN/Pandomium-Vue-Spring-template](https://github.com/ZenLiuCN/Pandomium-Vue-Spring-template)
