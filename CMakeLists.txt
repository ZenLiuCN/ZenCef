cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 14)
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
        ${PROJECT_SOURCE_DIR}/ext/test.html)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -m32 -municode -mwindows)
set(EXTRA_LIBS ${EXTRA_LIBS} libcef_dll_wrapper)
link_libraries(${PROJECT_SOURCE_DIR}/goserver/goserver.a D:/Dev/env/mingw/mingw32/i686-w64-mingw32/lib/libwinmm.a D:/Dev/env/mingw/mingw32/i686-w64-mingw32/lib/libws2_32.a)
add_executable(cef
        main_win.cc
        app.cc
        app.h
        life_span_handler.cc
        life_span_handler_win.cc
        life_span_handler.h
        resource.h
        app.rc
        helper_win.cpp helper_win.h debug.h)

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
