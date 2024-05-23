#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <string>
#include "kd_tree.h"

struct block
{
    cv::Vec4b color;
    std::string name;
};

struct minecraft_color
{
    cv::Vec3f cie;
    std::vector <block> blocks;
};

struct color_generator
{
private:
    int layers;
    vector <block> blocks;
    vector <block> glasses;
    std::map <vec_k <float, 3>, minecraft_color> colors;
    kd_tree <float, 3> points;

public:
    void set_layers(int);
    void set_blocks(const std::vector <block>&);
    void set_glasses(const std::vector <block>&);
    void init();
    std::vector <block> make_color(cv::Vec3f);
};

cv::Vec3f bgr2cie(cv::Vec3b);
cv::Vec4b merge_colors(cv::Vec4b, cv::Vec4b);