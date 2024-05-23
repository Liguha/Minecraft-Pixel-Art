#include <opencv2/opencv.hpp> 
#include <iostream> 
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <Windows.h>
#include "kd_tree.h"
#include "color_generator.h"

using namespace cv;
using namespace std;

vector <string> file_list(string dir)
{
    WIN32_FIND_DATA data;
    wstring wdir(dir.begin(), dir.end());
    wstring filter = wdir + L"\\*";
    HANDLE hFind = FindFirstFile(filter.c_str(), &data);
    vector <string> res;
    int k = 0;

    if (hFind != INVALID_HANDLE_VALUE) 
    {
        do 
        {
            wstring name = data.cFileName;
            if (name == L"." || name == L".." || name == L"desktop.ini")
                continue;
            wstring file = wdir + L"\\" + name;
            res.push_back(string(file.begin(), file.end()));
        } while (FindNextFile(hFind, &data));
        FindClose(hFind);
    }
    return res;
}

block file2block(string name)
{
    Mat img = imread(name, IMREAD_UNCHANGED);
    cvtColor(img, img, COLOR_BGR2BGRA);
    resize(img, img, Size(1, 1));
    block res;
    res.color = img.at<Vec4b>(0, 0);
    string str = name.substr(0, name.length() - 4);
    res.name = "";
    for (int i = str.length() - 1; i >= 0; i--)
    {
        if (str[i] == '\\')
            break;
        res.name = str[i] + res.name;
    }
    return res;
}

int main()
{
    vector <string> block_files = file_list("blocks");
    vector <string> glass_files = file_list("glasses");
    vector <block> blocks, glasses;
    for (int i = 0; i < block_files.size(); i++)
        blocks.push_back(file2block(block_files[i]));
    for (int i = 0; i < glass_files.size(); i++)
        glasses.push_back(file2block(glass_files[i]));

    int n = 3;                      // количество слоёв арта
    color_generator gen;
    gen.set_layers(n);
    gen.set_blocks(blocks);
    gen.set_glasses(glasses);
    gen.init();

    Mat3f img = imread("test.jpg"); // название изображения
    flip(img, img, 1);
    img.convertTo(img, CV_32F, 1. / 255, 0);
    cvtColor(img, img, COLOR_BGR2Lab);
    const int lim = 65000;
    int file_cnt = 1;
    int k = 0;
    ofstream file("image\\draw" + to_string(file_cnt) + ".mcfunction");
    for (int r = 0; r < img.rows; r++)
    {
        for (int c = 0; c < img.cols; c++)
        {
            vector <block> v = gen.make_color(img.at<Vec3f>(r, c));
            for (int i = 0; i < v.size(); i++)
            {
                file << "setblock ~" << r + 1 << " ~" << n - i - 1 << " ~" << c + 1 << " minecraft:" << v[i].name << endl;
                k++;
            }
            if (n != v.size())
            {
                file << "setblock ~" << r + 1 << " ~" << n - v.size() - 1 << " ~" << c + 1 << " minecraft:dirt" << endl;
                k++;
            }
            if (k > lim)
            {
                file_cnt++;
                file << "setblock ~-2 ~ ~ minecraft:command_block 1 0 {Command:\"" << "function image:draw" << file_cnt << "\"}" << endl;
                file << "setblock ~-2 ~1 ~ minecraft:redstone_block";
                file.close();
                file.open("image\\draw" + to_string(file_cnt) + ".mcfunction");
                k = 0;
            }
        }
    }
}
