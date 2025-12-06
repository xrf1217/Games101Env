//
// Created by goksu on 4/6/19.
//

#pragma once

#include "Triangle2.hpp"
#include <algorithm>
#include <vector>
#include <map>
#include <Eigen/Dense>

namespace rst2
{
    enum class Buffers2
    {
        Color = 1,
        Depth = 2
    };

    inline Buffers2 operator|(Buffers2 a, Buffers2 b)
    {
        return Buffers2((int)a | (int)b);
    }

    inline Buffers2 operator&(Buffers2 a, Buffers2 b)
    {
        return Buffers2((int)a & (int)b);
    }

    enum class Primitive2
    {
        Line,
        Triangle
    };

    /*
     * For the curious : The draw function takes two buffer id's as its arguments. These two structs
     * make sure that if you mix up with their orders, the compiler won't compile it.
     * Aka : Type safety
     * */
    struct pos_buf_id2
    {
        int pos_id = 0;
    };

    struct ind_buf_id2
    {
        int ind_id = 0;
    };

    struct col_buf_id
    {
        int col_id = 0;
    };

    class rasterizer2
    {
    public:
        rasterizer2(int w, int h);
        pos_buf_id2 load_positions(const std::vector<Eigen::Vector3f>& positions);
        ind_buf_id2 load_indices(const std::vector<Eigen::Vector3i>& indices);
        col_buf_id load_colors(const std::vector<Eigen::Vector3f>& colors);

        void set_model(const Eigen::Matrix4f& m);
        void set_view(const Eigen::Matrix4f& v);
        void set_projection(const Eigen::Matrix4f& p);

        void set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color);

        void clear(Buffers2 buff);

        void draw(pos_buf_id2 pos_buffer, ind_buf_id2 ind_buffer, col_buf_id col_buffer, Primitive2 type);

        std::vector<Eigen::Vector3f>& frame_buffer() { return frame_buf; }

    private:
        void draw_line(Eigen::Vector3f begin, Eigen::Vector3f end);

        void rasterize_triangle(const Triangle2& t);

        // VERTEX SHADER -> MVP -> Clipping -> /.W -> VIEWPORT -> DRAWLINE/DRAWTRI -> FRAGSHADER

    private:
        Eigen::Matrix4f model;
        Eigen::Matrix4f view;
        Eigen::Matrix4f projection;

        std::map<int, std::vector<Eigen::Vector3f>> pos_buf;
        std::map<int, std::vector<Eigen::Vector3i>> ind_buf;
        std::map<int, std::vector<Eigen::Vector3f>> col_buf;

        std::vector<Eigen::Vector3f> frame_buf;

        std::vector<float> depth_buf;
        std::vector<Eigen::Vector3f> frame_sampleBuf;

        std::vector<float> depth_smapleBuf;
        int get_index(int x, int y);

        int width, height;

        int next_id = 0;
        int get_next_id() { return next_id++; }
    };
}
