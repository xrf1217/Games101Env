// 轻量 OpenCV 头文件兼容层
// 目的：适配不同项目中对 OpenCV 头的包含方式，避免直接依赖固定路径
// 使用：用此头替换 <opencv2/opencv.hpp> 的直接包含

#pragma once

// 优先使用单头 opencv.hpp（常见于 OpenCV 3/4 的安装包）
#if __has_include(<opencv2/opencv.hpp>)
#include <opencv2/opencv.hpp>

// 其次尝试常用的子模块组合（适用于部分裁剪安装或不同布局）
#elif __has_include(<opencv2/core.hpp>)
#include <opencv2/core.hpp>
#if __has_include(<opencv2/imgcodecs.hpp>)
#include <opencv2/imgcodecs.hpp>
#endif
#if __has_include(<opencv2/highgui.hpp>)
#include <opencv2/highgui.hpp>
#endif
#if __has_include(<opencv2/imgproc.hpp>)
#include <opencv2/imgproc.hpp>
#endif

// OpenCV 2.x 或极端裁剪的安装环境（很少见）
#elif __has_include(<cv.h>)
#include <cv.h>
#include <highgui.h>

#else
#error "无法找到 OpenCV 头文件，请确认已安装并配置包含目录。"
#endif


