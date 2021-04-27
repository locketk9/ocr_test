#ifndef __ZHANGSUEN_H__
#define __ZHANGSUEN_H__

#include <set>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <iomanip>

#include "ocr.h"

namespace zs
{

typedef std::pair<int, int> Point;
typedef unsigned char uchar_t;

int num_one_pixel_neighbours(const ch_vec& image, const Point& point, int cx);

int num_zero_pixel_neighbours(const ch_vec& image, const Point& point, int cx);

int connectivity(const ch_vec& image, const Point& point, int cx);

int yokoi_connectivity(const ch_vec& image, const Point& point, int cx);

void delete_pixels(const ch_vec& image, const std::set<Point>& points, int cx);

void remove_staircases(ch_vec& image, int cx, int cy);

void zhangsuen_thin(ch_vec& image, int x, int y, int cx, int cy);
void zhangsuen_thin(ch_vec& image, const imgRECT rect, int x, int y, int cx, int cy);

void thin(ch_vec& img, const imgRECT &rect, const imgRECT &seg, bool need_boundary_smoothing=false,
          bool need_acute_angle_emphasis=false, bool destair=false);

void boundary_smooth(ch_vec& image, int cx, int cy);

void acute_angle_emphasis(ch_vec& image, int cx, int cy);

bool match(const ch_vec& image, const std::vector<Point>& points,
           const std::vector<uchar_t>& values);

bool match_templates(const ch_vec& image, const Point& point, int k, int cx, int cy);

};

#endif