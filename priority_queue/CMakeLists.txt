cmake_minimum_required (VERSION 3.8)
project(test)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")  # 定义编译类型
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb") # 定义Debug编译参数
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall") # 定义Release编译参数
set(SRC_LIST priority_queue.hpp code.cpp) #修改调试的源文件
add_executable(test code.cpp)