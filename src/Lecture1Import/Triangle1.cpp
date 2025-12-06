//
// Created by LEI XU on 4/11/19.
//

#include "Triangle1.hpp"
#include <algorithm>
#include <array>
#include <stdexcept>

Triangle1::Triangle1()
{
    v[0] << 0, 0, 0;
    v[1] << 0, 0, 0;
    v[2] << 0, 0, 0;

    color[0] << 0.0, 0.0, 0.0;
    color[1] << 0.0, 0.0, 0.0;
    color[2] << 0.0, 0.0, 0.0;

    tex_coords[0] << 0.0, 0.0;
    tex_coords[1] << 0.0, 0.0;
    tex_coords[2] << 0.0, 0.0;
}

void Triangle1::setVertex(int ind, Eigen::Vector3f ver) { v[ind] = ver; }

void Triangle1::setNormal(int ind, Eigen::Vector3f n) { normal[ind] = n; }

void Triangle1::setColor(int ind, float r, float g, float b)
{
    if ((r < 0.0) || (r > 255.) || (g < 0.0) || (g > 255.) || (b < 0.0) ||
        (b > 255.)) {
        throw std::runtime_error("Invalid color values");
    }

    color[ind] = Eigen::Vector3f((float)r / 255., (float)g / 255., (float)b / 255.);
    return;
}
void Triangle1::setTexCoord(int ind, float s, float t)
{
    tex_coords[ind] = Eigen::Vector2f(s, t);
}

std::array<Eigen::Vector4f, 3> Triangle1::toVector4() const
{
    std::array<Eigen::Vector4f, 3> res;
    std::transform(std::begin(v), std::end(v), res.begin(), [](auto& vec) {
        return Eigen::Vector4f(vec.x(), vec.y(), vec.z(), 1.f);
    });
    return res;
}
