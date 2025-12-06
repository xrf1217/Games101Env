cmake_minimum_required(VERSION 3.28)
project(Games101Env)

set(CMAKE_CXX_STANDARD 17)

# --- 1. 路径设置 (Eigen) ---
set(EIGEN_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/eigen-3.4.0")

# =========================================================
# --- 2. 配置 OpenCV 子模块 (这里是改动最大的地方) ---
# =========================================================
# 我们不再用 find_package，而是直接编译 lib/opencv 下的源码
message(STATUS "正在配置内置 OpenCV 源码，首次编译时间较长，请耐心等待...")

# 2.1 性能优化：强制关闭不需要的模块 (非常重要，否则编译要半小时)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)    # 静态编译，生成独立的 .exe
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)           # 关掉测试
set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)      # 关掉性能测试
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)        # 关掉示例
set(BUILD_opencv_python3 OFF CACHE BOOL "" FORCE)  # 关掉 Python
set(BUILD_opencv_java OFF CACHE BOOL "" FORCE)     # 关掉 Java
set(BUILD_opencv_apps OFF CACHE BOOL "" FORCE)     # 关掉工具

# 2.2 只编译 Games101 需要的核心模块
# 这行代码能显著减少编译时间
set(BUILD_LIST core highgui imgproc imgcodecs)

# 2.3 添加子目录 (开始处理源码)
# 这会去读取你刚才 git submodule 下载的那个文件夹
add_subdirectory(lib/opencv)

# =========================================================

# --- 3. 定义源文件 ---
set(CORE_SOURCES
        src/Lecture1To2.cpp
        src/Lecture1Import/rasterizer1.cpp
        src/Lecture1Import/Triangle1.cpp
        src/Lecture3.cpp
        src/Lecture2Import/rasterizer2.cpp
        src/Lecture2Import/Triangle2.cpp
        src/Lecture3Import/rasterizer3.cpp
        src/Lecture3Import/Triangle3.cpp
        src/Lecture3Import/Texture.cpp
)

# --- 4. 创建静态库 (MyLibrary) ---
add_library(MyLibrary STATIC ${CORE_SOURCES})

# 配置头文件路径
target_include_directories(MyLibrary PUBLIC
        ${EIGEN_INCLUDE_DIR}
        "${CMAKE_SOURCE_DIR}/src"
        # 注意：这里改成了指向子模块的 include 目录
        "${CMAKE_SOURCE_DIR}/lib/opencv/include"
)

# 链接 OpenCV
# 注意：源码编译模式下，我们要链接具体的模块名，而不是 ${OpenCV_LIBS}
target_link_libraries(MyLibrary PUBLIC
        opencv_core
        opencv_highgui
        opencv_imgproc
        opencv_imgcodecs
)

# --- 5. 创建可执行文件 ---
add_executable(Games101Env src/main.cpp)

# --- 6. 链接 ---
target_link_libraries(Games101Env PRIVATE MyLibrary)