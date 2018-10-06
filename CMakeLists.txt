cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 14)
# USE CLANG
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER "clang.exe")
set(CMAKE_CXX_COMPILER "clang++.exe")
# target i686-pc-windows-gnu for 32bit x86_64-pc-windows-gnu for64
set(CLANG_COVERAGE_COMPILE_FLAGS "-target i686-pc-windows-gnu")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CLANG_COVERAGE_COMPILE_FLAGS}" )

set(MINGW32LIB D:/Dev/env/mingw/mingw32v6/i686-w64-mingw32/lib/)
include_directories(${PROJECT_SOURCE_DIR}/cef_lib)
include_directories(${PROJECT_SOURCE_DIR}/goserver)
add_subdirectory(${PROJECT_SOURCE_DIR}/cef_lib/libcef_dll)
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

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -m32 -mwindows)
set(EXTRA_LIBS ${EXTRA_LIBS} libcef_dll_wrapper)
link_libraries(${PROJECT_SOURCE_DIR}/goserver/goserver.a ${MINGW32LIB}libwinmm.a ${MINGW32LIB}libws2_32.a)
add_executable(cef
        main_win.cc
        app.cc
        app.h
        client.cc
        client_win.cc
        client.h
        resource.h
        app.rc
        helper_win.cpp helper_win.h debug.h go_server.cpp go_server.h keycodes.h WindowSchemeHandler.cpp WindowSchemeHandler.h)

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
