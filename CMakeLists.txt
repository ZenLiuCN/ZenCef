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

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -m32 )
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -m32 )


IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
ELSE()
#    set(CMAKE_EXE_LINKER_FLAGS    ${CMAKE_EXE_LINKER_FLAGS} -mwindows --subsystem,windows)
    set(CMAKE_EXE_LINKER_FLAGS    ${CMAKE_EXE_LINKER_FLAGS} -mwindows)
ENDIF ()
include_directories(${PROJECT_SOURCE_DIR}/cef_lib)
include_directories(${PROJECT_SOURCE_DIR}/goserver)
include_directories(${PROJECT_SOURCE_DIR}/application/)
#[[# https://github.com/uNetworking/uWebSockets
include_directories(${PROJECT_SOURCE_DIR}/libs/src/)]]
add_subdirectory(${PROJECT_SOURCE_DIR}/cef_lib/libcef_dll)


set(EXTRA_LIBS ${EXTRA_LIBS} libcef_dll_wrapper)
link_libraries(${PROJECT_SOURCE_DIR}/goserver/goserver.a ${MINGW32LIB}libwinmm.a ${MINGW32LIB}libws2_32.a)
add_executable(cef
        application/main_win.cc
        application/app.cc application/app.h
        application/client.cc application/client_win.cc application/client.h
        application/resource.h
        application/res/app.rc
        application/helper_win.cc application/helper_win.h
        application/debug.h
#        application/WsServer.hpp
        application/go_server.cc application/go_server.h
        application/WindowSchemeHandler.cc application/WindowSchemeHandler.h application/helpers.cpp application/helpers.h application/WebSocketSchemeHandler.cc application/WebSocketSchemeHandler.h application/JsActionHandler.h application/JsActionHandler.cpp)

target_link_libraries(cef ${EXTRA_LIBS})

#[[if (CMAKE_BUILD_TYPE_DEBUG)
    foreach(FILE ${DEBUG_BIN})
        add_custom_command(
                TARGET cef
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${FILE} ${EXECUTABLE_OUTPUT_PATH}
        )
    endforeach(FILE)
else ()
    add_custom_command(
            TARGET cef
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${RESOURCE_SRC} ${EXECUTABLE_OUTPUT_PATH}
            COMMAND ${CMAKE_COMMAND} ARGS -E copy ${RELEASE_BIN} ${TEST_HTML} ${EXECUTABLE_OUTPUT_PATH}
    )
endif ()]]
