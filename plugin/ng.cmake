# HuntingInSkyrimNative.dll on alandtse's CommonLibSSE-NG; included by E:/WorkSpace/ng-build/CMakeLists.txt (see ng_plugin there)
ng_plugin(TARGET HuntingInSkyrimNative NAME HuntingInSkyrimNative VERSION 1.1.0 ROOT "${CMAKE_CURRENT_LIST_DIR}"
    SOURCES src/main.cpp src/Settings.cpp src/AutoStart.cpp src/Menu.cpp
    INCLUDES "${CMAKE_CURRENT_LIST_DIR}/src" "${CMAKE_CURRENT_LIST_DIR}/include"
    PCH "${CMAKE_CURRENT_LIST_DIR}/src/PCH.h")
