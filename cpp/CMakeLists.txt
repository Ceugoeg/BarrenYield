cmake_minimum_required(VERSION 3.10)
project(barren_yield)

# 1. 强制 C++17
set(CMAKE_CXX_STANDARD 17)

# 2. 定义平台宏
add_definitions(-DLINUX_ENABLED -DUNIX_ENABLED -D_LINUX)

# 3. 补全头文件路径
include_directories(include)                 # 代码
include_directories(godot-cpp/include)       # Godot C++ 绑定
include_directories(godot-cpp/gen/include)   # Godot 生成的绑定
include_directories(godot-cpp/gdextension)   # GDExtensionVariantPtr 的定义

# 4. 扫描文件
file(GLOB_RECURSE MY_SOURCES "src/*.cpp")
file(GLOB_RECURSE MY_HEADERS "include/*.h")

# 5. 创建假目标
add_library(${PROJECT_NAME} SHARED ${MY_SOURCES} ${MY_HEADERS}
        src/core/ResourceRegistry.cpp
        include/core/ResourceRegistry.h
        src/utils/JsonLoader.cpp
        include/utils/JsonLoader.h
        src/economy/Inventory.cpp
        include/economy/Inventory.h
        src/core/GameLogger.cpp
        include/core/GameLogger.h)