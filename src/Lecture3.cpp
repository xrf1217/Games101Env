#include "Lecture3.h"
#include <Eigen/Dense>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <filesystem>
#include "Lecture3Import/global.hpp"
#include "Lecture3Import/rasterizer3.hpp"
#include "Lecture3Import/Triangle3.hpp"
#include "Lecture3Import/Shader.hpp"
#include "Lecture3Import/Texture.hpp"
#include "Lecture3Import/OBJ_Loader.h"

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos) {
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0],
            0, 1, 0, -eye_pos[1],
            0, 0, 1, -eye_pos[2],
            0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle) {
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    model(0, 0) = cos(rotation_angle / 180 * MY_PI);
    model(0, 1) = -sin(rotation_angle / 180 * MY_PI);
    model(1, 0) = sin(rotation_angle / 180 * MY_PI);
    model(1, 1) = cos(rotation_angle / 180 * MY_PI);
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar) {
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    // 将 eye_fov 从度数转换为弧度
    float eye_fov_rad = eye_fov * M_PI / 180.0f;

    // 计算透视矩阵的元素
    float n = zNear;
    float f = zFar;
    float t = std::tan(eye_fov_rad / 2.0f) * abs(n);
    float b = -t;
    float r = t * aspect_ratio;
    float l = -r;

    // 创建一个正交投影矩阵
    Eigen::Matrix4f persp_to_ortho = Eigen::Matrix4f::Identity();

    persp_to_ortho(0, 0) = 2.0f / (r - l);
    persp_to_ortho(1, 1) = 2.0f / (t - b);
    persp_to_ortho(2, 2) = 2.0f / (f - n);
    persp_to_ortho(0, 3) = -(r + l) / (r - l);
    persp_to_ortho(1, 3) = -(t + b) / (t - b);
    persp_to_ortho(2, 3) = -(f + n) / (f - n);

    // 创建一个透视变换矩阵，用于将透视空间中的点转换为正交空间
    Eigen::Matrix4f M_persp = Eigen::Matrix4f::Zero();
    M_persp(0, 0) = n;
    M_persp(1, 1) = n;
    M_persp(2, 2) = n + f;
    M_persp(2, 3) = -n * f;
    M_persp(3, 2) = 1;
    projection = persp_to_ortho * M_persp;
    return projection;
}

Eigen::Matrix4f get_rotation(Eigen::Vector3f axis, float angle) {
    float a = angle / 180 * MY_PI;
    Eigen::Matrix4f I, N, Rotation;
    Eigen::Vector4f n;
    Eigen::RowVector4f nt;
    n << axis.x(), axis.y(), axis.z(), 0;
    nt << axis.x(), axis.y(), axis.z(), 0;
    I = Eigen::Matrix4f::Identity();
    N << 0, -n.z(), n.y(), 0,
            n.z(), 0, -n.x(), 0,
            -n.y(), n.x(), 0, 0,
            0, 0, 0, 1;
    Rotation = cos(a) * I + (1 - cos(a)) * n * nt + sin(a) * N;
    Rotation(3, 3) = 1;
    return Rotation;
}


namespace Lecture3 {
    Eigen::Vector3f vertex_shader(const vertex_shader_payload &payload) {
        return payload.position;
    }

    Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload &payload) {
        Eigen::Vector3f return_color =
                (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
        Eigen::Vector3f result;
        result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
        return result;
    }

    static Eigen::Vector3f reflect(const Eigen::Vector3f &vec, const Eigen::Vector3f &axis) {
        auto costheta = vec.dot(axis);
        return (2 * costheta * axis - vec).normalized();
    }

    struct light {
        Eigen::Vector3f position;
        Eigen::Vector3f intensity;
    };

    Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload &payload) {
        Eigen::Vector3f return_color = {0, 0, 0};
        if (payload.texture) {
            // TODO: Get the texture value at the texture coordinates of the current fragment

        }
        Eigen::Vector3f texture_color;
        texture_color << return_color.x(), return_color.y(), return_color.z();

        Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
        Eigen::Vector3f kd = texture_color / 255.f;
        Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

        auto l1 = light{{20,  20,  20},
                        {500, 500, 500}};
        auto l2 = light{{-20, 20,  0},
                        {500, 500, 500}};

        std::vector<light> lights = {l1, l2};
        Eigen::Vector3f amb_light_intensity{10, 10, 10};
        Eigen::Vector3f eye_pos{0, 0, 10};

        float p = 150;

        Eigen::Vector3f color = texture_color;
        Eigen::Vector3f point = payload.view_pos;
        Eigen::Vector3f normal = payload.normal;

        Eigen::Vector3f result_color = {0, 0, 0};

        for (auto &light: lights) {
            // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular*
            // components are. Then, accumulate that result on the *result_color* object.

        }

        return result_color * 255.f;
    }

    Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload &payload) {
        Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
        Eigen::Vector3f kd = payload.color;
        Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

        auto l1 = light{{20,  20,  20},
                        {500, 500, 500}};
        auto l2 = light{{-20, 20,  0},
                        {500, 500, 500}};

        std::vector<light> lights = {l1, l2};
        Eigen::Vector3f amb_light_intensity{10, 10, 10};
        Eigen::Vector3f eye_pos{0, 0, 10};

        float p = 150;

        Eigen::Vector3f color = payload.color;
        Eigen::Vector3f point = payload.view_pos;
        Eigen::Vector3f normal = payload.normal;

        Eigen::Vector3f result_color = {0, 0, 0};
        for (auto &light: lights) {
            // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular*
            // components are. Then, accumulate that result on the *result_color* object.

        }

        return result_color * 255.f;
    }


    Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload &payload) {

        Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
        Eigen::Vector3f kd = payload.color;
        Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

        auto l1 = light{{20,  20,  20},
                        {500, 500, 500}};
        auto l2 = light{{-20, 20,  0},
                        {500, 500, 500}};

        std::vector<light> lights = {l1, l2};
        Eigen::Vector3f amb_light_intensity{10, 10, 10};
        Eigen::Vector3f eye_pos{0, 0, 10};

        float p = 150;

        Eigen::Vector3f color = payload.color;
        Eigen::Vector3f point = payload.view_pos;
        Eigen::Vector3f normal = payload.normal;

        float kh = 0.2, kn = 0.1;

        // TODO: Implement displacement mapping here
        // Let n = normal = (x, y, z)
        // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
        // Vector b = n cross product t
        // Matrix TBN = [t b n]
        // dU = kh * kn * (h(u+1/w,v)-h(u,v))
        // dV = kh * kn * (h(u,v+1/h)-h(u,v))
        // Vector ln = (-dU, -dV, 1)
        // Position p = p + kn * n * h(u,v)
        // Normal n = normalize(TBN * ln)


        Eigen::Vector3f result_color = {0, 0, 0};

        for (auto &light: lights) {
            // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular*
            // components are. Then, accumulate that result on the *result_color* object.


        }

        return result_color * 255.f;
    }


    Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload &payload) {

        Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
        Eigen::Vector3f kd = payload.color;
        Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

        auto l1 = light{{20,  20,  20},
                        {500, 500, 500}};
        auto l2 = light{{-20, 20,  0},
                        {500, 500, 500}};

        std::vector<light> lights = {l1, l2};
        Eigen::Vector3f amb_light_intensity{10, 10, 10};
        Eigen::Vector3f eye_pos{0, 0, 10};

        float p = 150;

        Eigen::Vector3f color = payload.color;
        Eigen::Vector3f point = payload.view_pos;
        Eigen::Vector3f normal = payload.normal;


        float kh = 0.2, kn = 0.1;

        // TODO: Implement bump mapping here
        // Let n = normal = (x, y, z)
        // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
        // Vector b = n cross product t
        // Matrix TBN = [t b n]
        // dU = kh * kn * (h(u+1/w,v)-h(u,v))
        // dV = kh * kn * (h(u,v+1/h)-h(u,v))
        // Vector ln = (-dU, -dV, 1)
        // Normal n = normalize(TBN * ln)


        Eigen::Vector3f result_color = {0, 0, 0};
        result_color = normal;

        return result_color * 255.f;
    }

///笔记——https://blog.csdn.net/Q_pril/article/details/123598746——————————————————————————
    void OnLecture3(int argc, const char **argv) {
        //记录组成三维图形的所有小三角形
        std::vector<Triangle3 *> TriangleList;

        float angle = 100;
        bool command_line = false;

        std::string filename = "output.png";
        objl::Loader Loader;
        std::string obj_path = "../res/models/spot/";

        std::cout << "cwd: " << std::filesystem::current_path().string() << std::endl;
        std::cout << "obj: " << (obj_path + "spot_triangulated_good.obj") << std::endl;

        // Load .obj File，加载模型
        bool loadout = Loader.LoadFile("../res/models/spot/spot_triangulated_good.obj");
        if (!loadout) {
            std::cerr << "Failed to load OBJ file. Check path above." << std::endl;
        }
        //对于图形中的每个面（即一个小三角形）的三个点记录在一起
        for (auto mesh: Loader.LoadedMeshes) {
            for (int i = 0; i < mesh.Vertices.size(); i += 3) {
                Triangle3 *t = new Triangle3();
                //用Triangle类，记录一个小三角形的三个顶点的信息：
                //setVertex顶点位置，setNormal顶点的法线，setTexCoord顶点对应的纹理
                for (int j = 0; j < 3; j++) {
                    t->setVertex(j, Vector4f(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y,
                                             mesh.Vertices[i + j].Position.Z, 1.0));
                    t->setNormal(j, Vector3f(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y,
                                             mesh.Vertices[i + j].Normal.Z));
                    t->setTexCoord(j, Vector2f(mesh.Vertices[i + j].TextureCoordinate.X,
                                               mesh.Vertices[i + j].TextureCoordinate.Y));
                }
                //每三个顶点构成的一个小三角形放入TriangleList
                TriangleList.push_back(t);
            }
        }

        //初始化光栅化对象，定义屏幕长宽
        rst3::rasterizer r(700, 700);

        //记录纹理到对象，注意rasterizer.hpp类有属性 optional<Texture> texture
        auto texture_path = "hmap.jpg";
        std::cout << "tex: " << (obj_path + texture_path) << std::endl;
        r.set_texture(Texture(obj_path + texture_path));

        //记录片元处理方式，类似于“赋值函数”，现默认方式是phong
        std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = phong_fragment_shader;

        int selectModel = 2;
        //处理传入的参数，注意在此处根据调用方式不同，修改了rasterizer对象的片元处理方式！
        if (argc >= 2 || selectModel > 0) {
            command_line = true;
            //filename = std::string(argv[1]);

            if (selectModel == 1 || argc == 3 && std::string(argv[2]) == "texture") {
                std::cout << "Rasterizing using the texture shader\n";
                active_shader = texture_fragment_shader;
                texture_path = "spot_texture.png";
                r.set_texture(Texture(obj_path + texture_path));
            } else if (selectModel == 2 || argc == 3 && std::string(argv[2]) == "normal") {
                std::cout << "Rasterizing using the normal shader\n";
                active_shader = normal_fragment_shader;
            } else if (selectModel == 3 || argc == 3 && std::string(argv[2]) == "phong") {
                std::cout << "Rasterizing using the phong shader\n";
                active_shader = phong_fragment_shader;
            } else if (selectModel == 4 || argc == 3 && std::string(argv[2]) == "bump") {
                std::cout << "Rasterizing using the bump shader\n";
                active_shader = bump_fragment_shader;
            } else if (selectModel == 5 || argc == 3 && std::string(argv[2]) == "displacement") {
                std::cout << "Rasterizing using the bump shader\n";
                active_shader = displacement_fragment_shader;
            }
        }
        //人眼所在位置
        Eigen::Vector3f eye_pos = {0, 0, 10};

        //设置顶点着色方式，获取顶点位置
        r.set_vertex_shader(vertex_shader);
        //设置片元着色方式，根据调用方式不同已赋值到active_shade
        r.set_fragment_shader(active_shader);

        int key = 0;//修改这个值，可以选择输出图片或动态旋转渲染的模型
        int frame_count = 0;

        if (command_line) {
            //清空两个缓冲区，在最后处理遮挡显示情况时发挥作用
            r.clear(rst3::Buffers::Color | rst3::Buffers::Depth);
            //分别设置MVP矩阵，用于对点操作，实现将三维的点映射到平面
            r.set_model(get_model_matrix(angle));
            r.set_view(get_view_matrix(eye_pos));
            r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

            if (TriangleList.empty()) {
                std::cerr << "TriangleList is empty. OBJ mesh may not have loaded correctly." << std::endl;
            }
            //光栅化、片元处理
            r.draw(TriangleList);
            cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
            image.convertTo(image, CV_8UC3, 1.0f);
            cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

            cv::imwrite(filename, image);
        }
        while (key != 27) {
            r.clear(rst3::Buffers::Color | rst3::Buffers::Depth);

            r.set_model(get_model_matrix(angle));
            r.set_view(get_view_matrix(eye_pos));
            r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

            //r.draw(pos_id, ind_id, col_id, rst3::Primitive::Triangle3);
            r.draw(TriangleList);
            cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
            image.convertTo(image, CV_8UC3, 1.0f);
            cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

            cv::imshow("image", image);
            cv::imwrite(filename, image);
            key = cv::waitKey(10);

            if (key == 'a') {
                angle -= 10;
            } else if (key == 'd') {
                angle += 10;
            }
        }
    }
}