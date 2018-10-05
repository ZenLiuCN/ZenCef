cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 14 )
include_directories(${PROJECT_SOURCE_DIR}/cef_lib)
include_directories(${PROJECT_SOURCE_DIR}/goserver)
add_subdirectory(${PROJECT_SOURCE_DIR}/cef_lib/libcef_dll)
#[[if (CMAKE_BUILD_TYPE_DEBUG)
#    link_libraries(${PROJECT_SOURCE_DIR}/cef_lib/Debug/libcef.lib ${PROJECT_SOURCE_DIR}/cef_lib/Debug/libcef_dll_wrapper.a)
    link_libraries(${PROJECT_SOURCE_DIR}/cef_lib/Debug/libcef.lib )
else ()
#    link_libraries(${PROJECT_SOURCE_DIR}/cef_lib/Release/libcef.lib ${PROJECT_SOURCE_DIR}/cef_lib/Release/libcef_dll_wrapper.a)
    link_libraries(${PROJECT_SOURCE_DIR}/cef_lib/Release/libcef.lib )
endif ()]]
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -m32 -municode -mwindows -Wl,-subsystem,windows)
set (EXTRA_LIBS ${EXTRA_LIBS} libcef_dll_wrapper)
link_libraries(${PROJECT_SOURCE_DIR}/goserver/goserver.a D:/Dev/env/mingw/mingw32/i686-w64-mingw32/lib/libwinmm.a D:/Dev/env/mingw/mingw32/i686-w64-mingw32/lib/libws2_32.a)
add_executable(cef
        cefsimple_win.cc
        simple_app.cc
        simple_app.h
        simple_handler.cc
        simple_handler.h
        resource.h
        cefsimple.rc
        )

target_link_libraries(cef ${EXTRA_LIBS})