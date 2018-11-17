cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 14)

# ---BEGIN USE CLANG

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER "clang.exe")
set(CMAKE_CXX_COMPILER "clang++.exe")
# target i686-pc-windows-gnu for 32bit x86_64-pc-windows-gnu for64
set(CLANG_COVERAGE_COMPILE_FLAGS "-target i686-pc-windows-gnu")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_COVERAGE_COMPILE_FLAGS}")
# ---END USE CLANG
#[[# ---BEGIN USE BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.68.0 COMPONENTS *boost libraries here*)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(progname file1.cxx file2.cxx)
    target_link_libraries(progname ${Boost_LIBRARIES})
endif()
# ---END USE BOOST]]

set(MINGW32LIB D:/Dev/env/mingw/mingw32v6/i686-w64-mingw32/lib/)
set(GO D:/Dev/env/golang/go.1.10x32/bin/go.exe)
set(RELEASE_BIN
        ${PROJECT_SOURCE_DIR}/cef_lib/Release/d3dcompiler_43.dll
        ${PROJECT_SOURCE_DIR}/cef_lib/Release/d3dcompiler_47.dll
        ${PROJECT_SOURCE_DIR}/cef_lib/Release/libcef.dll
        ${PROJECT_SOURCE_DIR}/cef_lib/Release/libEGL.dll
        ${PROJECT_SOURCE_DIR}/cef_lib/Release/libGLESv2.dll
        ${PROJECT_SOURCE_DIR}/cef_lib/Release/natives_blob.bin
        ${PROJECT_SOURCE_DIR}/cef_lib/Release/snapshot_blob.bin
        ${PROJECT_SOURCE_DIR}/cef_lib/Release/widevinecdmadapter.dll
        )
set(DEBUG_BIN
        ${PROJECT_SOURCE_DIR}/cef_lib/Debug/d3dcompiler_43.dll
        ${PROJECT_SOURCE_DIR}/cef_lib/Debug/d3dcompiler_47.dll
        ${PROJECT_SOURCE_DIR}/cef_lib/Debug/libcef.dll
        ${PROJECT_SOURCE_DIR}/cef_lib/Debug/libEGL.dll
        ${PROJECT_SOURCE_DIR}/cef_lib/Debug/libGLESv2.dll
        ${PROJECT_SOURCE_DIR}/cef_lib/Debug/natives_blob.bin
        ${PROJECT_SOURCE_DIR}/cef_lib/Debug/snapshot_blob.bin
        ${PROJECT_SOURCE_DIR}/cef_lib/Debug/widevinecdmadapter.dll
        )
set(RESOURCE_SRC
        ${PROJECT_SOURCE_DIR}/cef_lib/Resources)
set(TEST_HTML
        ${PROJECT_SOURCE_DIR}/ext/index.html)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -m32)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -m32)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc ")

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
    set(GO_OUT "${PROJECT_SOURCE_DIR}/goserver/lib/debug/goserver.a")
    include_directories(${PROJECT_SOURCE_DIR}/goserver/lib/debug)
ELSE ()
    #    set(CMAKE_EXE_LINKER_FLAGS    ${CMAKE_EXE_LINKER_FLAGS} -mwindows --subsystem,windows)
    set(GO_LDFALG -ldflags "-s -w ")
    set(GO_OUT "${PROJECT_SOURCE_DIR}/goserver/lib/release/goserver.a")
    include_directories(${PROJECT_SOURCE_DIR}/goserver/lib/release)
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -mwindows)
ENDIF ()

include_directories(${PROJECT_SOURCE_DIR}/cef_lib)
include_directories(${PROJECT_SOURCE_DIR}/application/)
include_directories(${PROJECT_SOURCE_DIR}/application/lib/plog/include/)
include_directories(${PROJECT_SOURCE_DIR}/application/lib/fmt/include/)
include_directories(${PROJECT_SOURCE_DIR}/application/lib/mingw-std-threads/)
add_subdirectory(${PROJECT_SOURCE_DIR}/cef_lib/libcef_dll)
# BUG direct use fmt
#add_subdirectory(${PROJECT_SOURCE_DIR}/application/lib/fmt)
set(EXTRA_LIBS ${EXTRA_LIBS} libcef_dll_wrapper)

set(GO D:/Dev/env/golang/go.1.10x32/bin/go.exe)
set(GO_SRC "${PROJECT_SOURCE_DIR}/goserver")
set(GO_PATH "%GOPATH%\;${PROJECT_SOURCE_DIR}/goserver")

add_custom_target(goserver
        COMMAND set GOPATH=${GO_PATH}&& cd ${GO_SRC}/src/srv && ${GO} build -o ${GO_OUT} -i ${GO_LDFALG} -buildmode=c-archive .
        COMMAND  ${CMAKE_COMMAND} -E cmake_echo_color --cyan "mannul build new golib is over!"
        )


link_libraries(${GO_OUT} ${MINGW32LIB}libwinmm.a ${MINGW32LIB}libws2_32.a)
add_executable(cef
        # src of fmtlib
        application/lib/fmt/src/format.cc
        application/lib/fmt/src/posix.cc
        # config
        application/config.cc
        # logger
        application/logger.cc
        # enternce
        application/main_win.cc
        # CefApp
        application/app.cc
        # CefClient
        application/client.cc
        application/client_win.cc
        # res
        application/res/app.rc
        application/md5.hpp
        application/httpClient.hpp
        application/helper_win.cc
        application/WindowController.cc
        application/go_server.cc
        application/helpers.cpp
        application/downloadBox.hpp
        application/v8handler.cc application/singleton.h)

target_link_libraries(cef ${EXTRA_LIBS})
