#include "Lecture1To2.h"


using namespace std;

constexpr double MY_PI = 3.1415926;
namespace Utils {

    Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos) {
        Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

        Eigen::Matrix4f translate;
        translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
                -eye_pos[2], 0, 0, 0, 1;

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
        persp_to_ortho(2, 2) = 2.0f / (n - f);
        persp_to_ortho(0, 3) = -(r + l) / (r - l);
        persp_to_ortho(1, 3) = -(t + b) / (t - b);
        persp_to_ortho(2, 3) = -(n + f) / (n - f);

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

}
namespace Lecture00 {
    void OnLecture00() {
        // Eigen::Matrix3d 推荐的初始化方式是不带参数
        Eigen::Matrix3d move;

        // std::acos(-1) 计算 PI
        double angle = 30.0 / 180.0 * acos(-1);

        // 使用逗号初始化器进行赋值
        move << 1.0, 0.0, 1.0,
                0.0, 1.0, 2.0,
                0.0, 0.0, 1.0;

        Eigen::Matrix3d rotate;
        rotate << cos(angle), -sin(angle), 0.0,
                sin(angle), cos(angle), 0.0,
                0.0, 0.0, 1.0;

        Eigen::Vector3d p;
        p << 2, 1, 1;

//    cout << "Rotation Matrix:\n" << rotate << endl << endl;
//
//    Eigen::Matrix3d M = rotate * move;
//    Eigen::Vector3d p_transformed = M * p;
//
//    cout << "Combined Transformation Matrix:\n" << M << endl << endl;
//    cout << "Transformed Vector:\n" << p_transformed << endl;
        Eigen::MatrixXf son(4, 4);
        son << 2.0, 1.0, 1.0, 1.0,
                0.0, 1.0, 0.0, 0.0,
                1.0, 1.0, 2.0, 1.0,
                0.0, 0.0, 0.0, 1.0;
        Eigen::MatrixXf dir(4, 1);
        dir << 1.0, 0.0, 1.0, 1.0;
        Eigen::MatrixXf parentDir;
        parentDir = son * dir;
        cout << parentDir;

    }
}
namespace Lecture01 {

    void OnLecture01(int argc, const char **argv) {
        float angle = 0;
        bool command_line = false;
        std::string filename = "output.png";

        float rangle = 0;
        Eigen::Vector3f axis;
        axis << 1.0, 0.0, 0.0;
        int is_rotation = 0;


        if (argc >= 3) {
            command_line = true;
            angle = std::stof(argv[2]); // -r by default
            if (argc == 4) {
                filename = std::string(argv[3]);
            }
        }

        rst::rasterizer1 r(700, 700);

        Eigen::Vector3f eye_pos = {0, 0, 5};

        std::vector<Eigen::Vector3f> pos{{2,  0, -2},
                                         {0,  2, -2},
                                         {-2, 0, -2}};

        std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

        auto pos_id = r.load_positions(pos);
        auto ind_id = r.load_indices(ind);

        int key = 0;
        int frame_count = 0;

        if (command_line) {
            r.clear(rst::Buffers::Color | rst::Buffers::Depth);

            Eigen::Matrix4f m = Utils::get_rotation(axis, rangle) * Utils::get_model_matrix(angle);
            r.set_model(m);
            r.set_view(Utils::get_view_matrix(eye_pos));
            r.set_projection(Utils::get_projection_matrix(45, 1, 0.1, 50));

            r.draw(pos_id, ind_id, rst::Primitive::Triangle);
            cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
            image.convertTo(image, CV_8UC3, 1.0f);

            cv::imwrite(filename, image);

            return;
        }

        while (key != 27) {
            r.clear(rst::Buffers::Color | rst::Buffers::Depth);

            Eigen::Matrix4f m = Utils::get_rotation(axis, rangle) * Utils::get_model_matrix(angle);
            r.set_model(m);
            r.set_view(Utils::get_view_matrix(eye_pos));
            r.set_projection(Utils::get_projection_matrix(45, 1, 0.1, 50));
            r.draw(pos_id, ind_id, rst::Primitive::Triangle);

            cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
            image.convertTo(image, CV_8UC3, 1.0f);
            cv::imshow("image", image);
            key = cv::waitKey(10);

            std::cout << "frame count: " << frame_count++ << '\n';
            if (key == 'r') {
                is_rotation ^= 1;
            }
            if (key == 'a') {
                if (is_rotation) {
                    rangle += 10;
                } else {
                    angle += 10;
                }
            } else if (key == 'd') {
                if (is_rotation) {
                    rangle -= 10;
                } else {
                    angle -= 10;
                }
            }
        }
    }
}
namespace Lecture02 {

    void OnLecture02(int argc, const char **argv) {
        float angle = 0;
        bool command_line = false;
        std::string filename = "output.png";

        if (argc == 2) {
            command_line = true;
            filename = std::string(argv[1]);
        }

        rst2::rasterizer2 r(700, 700);

        Eigen::Vector3f eye_pos = {0, 0, 5};


        std::vector<Eigen::Vector3f> pos
                {
                        {2,   0,   -2},
                        {0,   2,   -2},
                        {-2,  0,   -2},
                        {3.5, -1,  -5},
                        {2.5, 1.5, -5},
                        {-1,  0.5, -5}
                };

        std::vector<Eigen::Vector3i> ind
                {
                        {0, 1, 2},
                        {3, 4, 5}
                };

        std::vector<Eigen::Vector3f> cols
                {
                        {217.0, 238.0, 185.0},
                        {217.0, 238.0, 185.0},
                        {217.0, 238.0, 185.0},
                        {185.0, 217.0, 238.0},
                        {185.0, 217.0, 238.0},
                        {185.0, 217.0, 238.0}
                };

        auto pos_id = r.load_positions(pos);
        auto ind_id = r.load_indices(ind);
        auto col_id = r.load_colors(cols);

        int key = 0;
        int frame_count = 0;

        if (command_line) {
            r.clear(rst2::Buffers2::Color | rst2::Buffers2::Depth);

            r.set_model(Utils::get_model_matrix(angle));
            r.set_view(Utils::get_view_matrix(eye_pos));
            r.set_projection(Utils::get_projection_matrix(45, 1, 0.1, 50));

            r.draw(pos_id, ind_id, col_id, rst2::Primitive2::Triangle);
            cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
            image.convertTo(image, CV_8UC3, 1.0f);
            cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

            cv::imwrite(filename, image);

            return;
        }

        while (key != 27) {
            r.clear(rst2::Buffers2::Color | rst2::Buffers2::Depth);

            r.set_model(Utils::get_model_matrix(angle));
            r.set_view(Utils::get_view_matrix(eye_pos));
            r.set_projection(Utils::get_projection_matrix(45, 1, 0.1, 50));

            r.draw(pos_id, ind_id, col_id, rst2::Primitive2::Triangle);

            cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
            image.convertTo(image, CV_8UC3, 1.0f);
            cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
            cv::imshow("image", image);
            key = cv::waitKey(10);

            std::cout << "frame count: " << frame_count++ << '\n';
        }
    }
}
