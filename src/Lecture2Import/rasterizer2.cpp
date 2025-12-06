#include <algorithm>
#include "rasterizer2.hpp"
#include "opencv_compat.hpp"
#include <math.h>
#include <stdexcept>
#include <Eigen/Dense>

using namespace Eigen;

rst2::pos_buf_id2 rst2::rasterizer2::load_positions(const std::vector<Eigen::Vector3f> &positions) {
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst2::ind_buf_id2 rst2::rasterizer2::load_indices(const std::vector<Eigen::Vector3i> &indices) {
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst2::col_buf_id rst2::rasterizer2::load_colors(const std::vector<Eigen::Vector3f> &cols) {
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4_2(const Eigen::Vector3f &v3, float w = 1.0f) {
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(float x, float y, const Vector3f *_v) {
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    Vector2f ab, bc, ca;
    ab = _v[1].head(2) - _v[0].head(2);
    bc = _v[2].head(2) - _v[1].head(2);
    ca = _v[0].head(2) - _v[2].head(2);
    Vector2f ap, bp, cp;
    ap = Vector2f(x, y) - _v[0].head(2);
    bp = Vector2f(x, y) - _v[1].head(2);
    cp = Vector2f(x, y) - _v[2].head(2);
    float c1 = ab.x() * ap.y() - ab.y() * ap.x();
    float c2 = bc.x() * bp.y() - bc.y() * bp.x();
    float c3 = ca.x() * cp.y() - ca.y() * cp.x();
    if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0)) {
        return true;
    }
    if (c1 == 0 || c2 == 0 || c3 == 0) {
        if (c1 * c2 >= 0 & c2 * c3 >= 0) {
            return true;
        }
    }
    return false;

}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f *v) {
    float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) /
               (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() -
                v[2].x() * v[1].y());
    float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) /
               (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() -
                v[0].x() * v[2].y());
    float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) /
               (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() -
                v[1].x() * v[0].y());
    return {c1, c2, c3};
}

void rst2::rasterizer2::draw(pos_buf_id2 pos_buffer, ind_buf_id2 ind_buffer, col_buf_id col_buffer, Primitive2 type) {
    auto &buf = pos_buf[pos_buffer.pos_id];
    auto &ind = ind_buf[ind_buffer.ind_id];
    auto &col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto &i: ind) {
        Triangle2 t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4_2(buf[i[0]], 1.0f),
                mvp * to_vec4_2(buf[i[1]], 1.0f),
                mvp * to_vec4_2(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto &vec: v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto &vert: v) {
            vert.x() = 0.5 * width * (vert.x() + 1.0);
            vert.y() = 0.5 * height * (vert.y() + 1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i) {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst2::rasterizer2::rasterize_triangle(const Triangle2 &t) {
    auto v = t.toVector4();
    Vector3f triangle;

    int max_x = std::ceil(std::max(v[0].x(), std::max(v[1].x(), v[2].x())));
    int max_y = std::ceil(std::max(v[0].y(), std::max(v[1].y(), v[2].y())));
    int min_x = std::ceil(std::min(v[0].x(), std::min(v[1].x(), v[2].x())));
    int min_y = std::ceil(std::min(v[0].y(), std::min(v[1].y(), v[2].y())));
    int pid = 0;
    float z_pixel = 0;//当前像素Z深度
    float dx[4] = {0.25, 0.25, 0.75, 0.75};
    float dy[4] = {0.25, 0.75, 0.75, 0.25};
    for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
            z_pixel = 0x3f3f3f3f;
            pid = get_index(i, j)* 4;
            for (int k = 0; k < 4; k++) {
                if (insideTriangle((float)i + dx[k], (float)j + dy[k], t.v)) {
                    auto [alpha, beta, gamma] = computeBarycentric2D((float)i + dx[k], (float)j + dy[k], t.v);
                    float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated =
                            alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    if (z_interpolated < depth_smapleBuf[pid + k]) {
                        depth_smapleBuf[pid + k] = z_interpolated;
                        frame_sampleBuf[pid + k] = t.getColor();
                    }
                    z_pixel = std::min(z_pixel, depth_smapleBuf[pid + k]);
                }
            }

            Vector3f point = {(float) i, (float) j, z_pixel};
            Vector3f color=(frame_sampleBuf[pid]+frame_sampleBuf[pid+1]+frame_sampleBuf[pid+2]+frame_sampleBuf[pid+3])/4;
            depth_buf[get_index(i,j)]=z_pixel;
            frame_buf[get_index(i,j)]=color;
            set_pixel(point, color);
        }
    }


    // TODO : Find out the bounding box of current triangle.
    // iterate through the pixel and find if the current pixel is inside the triangle

    // If so, use the following code to get the interpolated z value.
    //auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    //float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    //float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    //z_interpolated *= w_reciprocal;

    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
}

void rst2::rasterizer2::set_model(const Eigen::Matrix4f &m) {
    model = m;
}

void rst2::rasterizer2::set_view(const Eigen::Matrix4f &v) {
    view = v;
}

void rst2::rasterizer2::set_projection(const Eigen::Matrix4f &p) {
    projection = p;
}

void rst2::rasterizer2::clear(rst2::Buffers2 buff) {
    if ((buff & rst2::Buffers2::Color) == rst2::Buffers2::Color) {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
        std::fill(frame_sampleBuf.begin(), frame_sampleBuf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst2::Buffers2::Depth) == rst2::Buffers2::Depth) {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
        std::fill(depth_smapleBuf.begin(), depth_smapleBuf.end(), std::numeric_limits<float>::infinity());
    }
}

rst2::rasterizer2::rasterizer2(int w, int h) : width(w), height(h) {
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
    frame_sampleBuf.resize(4 * w * h);
    depth_smapleBuf.resize(4 * w * h);
}

int rst2::rasterizer2::get_index(int x, int y) {
    return (height - 1 - y) * width + x;
}

void rst2::rasterizer2::set_pixel(const Eigen::Vector3f &point, const Eigen::Vector3f &color) {
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height - 1 - point.y()) * width + point.x();
    frame_buf[ind] = color;

}

// clang-format on