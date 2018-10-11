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
