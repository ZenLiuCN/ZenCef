## WHAT
this is a project build top on cef3 
1. contains c++ app (from cef3 offical example)
2. contains embed go websocket server(expose window action to js)

## NOTE
already test on windows 7 x64 ,And it may only support windows 

## HOW
1. get mingw64 for 32bit
1. get cmake 3.12+
1. get go 1.8+
1. get cef3 binary form [binary]()
1. build go lib via `./goserver/build.bat`
1. build app via cmake 