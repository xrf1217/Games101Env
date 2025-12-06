cmake_minimum_required(VERSION 3.28)
project(Games101Env)

set(CMAKE_CXX_STANDARD 17)

# --- 1. 路径设置 ---
set(EIGEN_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/eigen-3.4.0")

# =========================================================
# --- 2. 配置 OpenCV (轻量极速版) ---
# =========================================================
message(STATUS "配置 OpenCV (轻量极速版)...")

# 强制关闭所有不用的模块 (包括 World 模式也关掉)
set(BUILD_opencv_world OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_python3 OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_java OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_apps OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_videoio OFF CACHE BOOL "" FORCE)
set(OPENCV_FFMPEG_SKIP_DOWNLOAD ON CACHE BOOL "" FORCE)
set(OPENCV_IPP_DOWNLOAD_PATH "" CACHE STRING "" FORCE)
set(OPENCV_DNN_CUDA OFF CACHE BOOL "" FORCE)

# 只编译这 4 个核心 (配合你修改后的 C++ 代码)
set(BUILD_LIST core imgproc imgcodecs highgui)

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

# --- 4. 创建静态库 ---
add_library(MyLibrary STATIC ${CORE_SOURCES})

# 强制依赖顺序
add_dependencies(MyLibrary opencv_core opencv_imgproc opencv_highgui)

# --- 5. 配置头文件路径 ---
target_include_directories(MyLibrary PUBLIC
        ${EIGEN_INCLUDE_DIR}
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/lib/opencv/include"  # 找 opencv2/core.hpp
        "${CMAKE_BINARY_DIR}/lib/opencv"          # 找 opencv_modules.hpp
        "${CMAKE_BINARY_DIR}"

        # 显式加入核心模块路径，确保万无一失
        "${CMAKE_SOURCE_DIR}/lib/opencv/modules/core/include"
        "${CMAKE_SOURCE_DIR}/lib/opencv/modules/imgproc/include"
        "${CMAKE_SOURCE_DIR}/lib/opencv/modules/imgcodecs/include"
        "${CMAKE_SOURCE_DIR}/lib/opencv/modules/highgui/include"
)

# --- 6. 链接 OpenCV ---
target_link_libraries(MyLibrary PUBLIC
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_highgui
)

# --- 7. 创建可执行文件 ---
add_executable(Games101Env src/main.cpp)
target_link_libraries(Games101Env PRIVATE MyLibrary)