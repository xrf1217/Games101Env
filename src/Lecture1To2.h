#ifndef LECTURE1TO2_H
#define LECTURE1TO2_H

#include <iostream>
#include <cmath> // 需要用到 cos() 和 sin()
#include "Lecture1Import/rasterizer1.hpp"
#include "Lecture2Import/rasterizer2.hpp"
#include "opencv_compat.hpp"
// 只包含必要的头文件
#include <Eigen/Dense>

// 使用前置声明，或者只包含必要的 Eigen 头文件
// using namespace std; 应该放在 cpp 文件里

// 函数声明，不包含实现
namespace Lecture00 {
    void OnLecture00();
}
namespace Lecture01 {
    void OnLecture01(int argc, const char **argv);
}
namespace Lecture02 {
    void OnLecture02(int argc, const char **argv);
}
#endif // LECTURE1TO2_H