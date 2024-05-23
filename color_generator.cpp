#include <vector>
#include <map>
#include "color_generator.h"

using namespace cv;
using namespace std;
using vec_3 = vec_k <float, 3>;

Vec3f bgr2cie(Vec3b bgr)
{
    Vec3f col(bgr[0] / 255., bgr[1] / 255., bgr[2] / 255.);
    Mat3f mat(col);
    cvtColor(mat, mat, COLOR_BGR2Lab);
    return mat.at<Vec3f>(0);
}

Vec4b merge_colors(Vec4b up, Vec4b down)
{
    double a0, a1;
    double bgr0[3], bgr1[3];
    for (int i = 0; i < 3; i++)
        bgr0[i] = up[i] / 255.;
    for (int i = 0; i < 3; i++)
        bgr1[i] = down[i] / 255.;
    a0 = up[3] / 255.;
    a1 = down[3] / 255.;
    double bgr01[3];
    double a01 = (1 - a0) * a1 + a0;
    for (int i = 0; i < 3; i++)
        bgr01[i] = ((1 - a0) * a1 * bgr1[i] + a0 * bgr0[i]) / a01;
    Vec4b res;
    for (int i = 0; i < 3; i++)
        res[i] = round(255 * bgr01[i]);
    res[3] = round(255 * a01);
    return res;
}

void color_generator::set_layers(int n)
{
    layers = n;
}

void color_generator::set_blocks(const vector <block>& v)
{
    blocks = v;
}

void color_generator::set_glasses(const vector <block>& v)
{
    glasses = v;
}

vector <vector <block>> combine_glases(int n, const vector <block>& glasses)
{
    if (n == 0)
        return { {} };
    vector <vector <block>> comb = combine_glases(n - 1, glasses);
    int k = comb.size();
    for (int i = 0; i < glasses.size(); i++)
    {
        for (int j = 0; j < k; j++)
        {
            vector <block> cur = comb[j];
            if (cur.size() > 0 && cur.back().name == glasses[i].name)
                continue;
            cur.push_back(glasses[i]);
            comb.push_back(cur);
        }
    }
    return comb;
}

void color_generator::init()
{
    for (int i = 0; i < blocks.size(); i++)
    {
        Vec3f cie = bgr2cie({ blocks[i].color[0], blocks[i].color[1], blocks[i].color[2] });
        vec_3 pt = vec_3({ cie[0], cie[1], cie[2] });
        minecraft_color col;
        col.cie = cie;
        col.blocks = { blocks[i] };
        colors[pt] = col;
    }
    vector <vector <block>> comb = combine_glases(layers - 1, glasses);
    for (int i = 0; i < blocks.size(); i++)
    {
        for (int j = 0; j < comb.size(); j++)
        {
            if (comb[j].size() == 0)
                continue;
            Vec4b bgra = comb[j][0].color;
            for (int k = 1; k < comb[j].size(); k++)
                bgra = merge_colors(bgra, comb[j][k].color);
            bgra = merge_colors(bgra, blocks[i].color);
            Vec3f cie = bgr2cie({ bgra[0], bgra[1], bgra[2] });
            vec_3 pt = vec_3({ cie[0], cie[1], cie[2] });
            minecraft_color col;
            col.cie = cie;
            col.blocks = comb[j];
            col.blocks.push_back(blocks[i]);
            colors[pt] = col;
        }
    }
    vector <vec_3> pts;
    for (auto it = colors.begin(); it != colors.end(); it++)
        pts.push_back(it->first);
    points = kd_tree <float, 3>(pts);
}

vector <block> color_generator::make_color(Vec3f cie)
{
    vec_3 cur = vec_3({ cie[0], cie[1], cie[2] });
    vec_3 res = points.nearest(cur);
    return colors[res].blocks;
}