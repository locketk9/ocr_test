/// @file hog.h
/// @author locketk9@gamil.com, 2020-03-17
/// @desc function set of my hog(histogram oriented gradient) 

#pragma once

#include <vector>
#include <algorithm>
#include <cmath>

#include "./ocr_func.h"

// defines
using byte = unsigned char;
using vImg = std::vector<byte>;
using vHist = std::vector<int>;
using size_t = std::size_t;

/// @brief make_hog
vHist& make_hog(const vImg& src, int cx, int cy
			, vHist& hog, size_t bx = 8, size_t by = 8) {
	if (src.size() % (bx*by) != 0) return hog;

	// init
	size_t xstep = bx / 2, ystep = by / 2;
	size_t xs = cx / xstep - 1, ys = cy / ystep - 1;
	size_t sz = xs * ys * 9; // 9 means gradient angle's count
	
	hog.clear();
	hog.reserve(sz);
	hog.resize(sz, 0);

	imgRECT rt_src(0, 0, cx, cy);
	for (size_t y = 0; y != cy; y += ystep) {
		for (size_t x = 0; x != cx; x += xstep) {
			// calc gradient of each block
			vImg cell(bx*by, 0);
			imgRECT rt_bl(x, y, bx, by);
			seg_copy(src, rt_src, cell, rt_bl);
			vHist hist(180+1, 0), norHist(9+1,0);
			for (size_t ky = 1; ky != by - 1; ++ky) {
				for (size_t kx = 1; kx != bx - 1; ++kx) {
					int cb = kx + (ky * bx);
					int vg = std::abs(cell[cb - bx] - cell[cb + bx]);
					int hg = std::abs(cell[cb - 1] - cell[cb + 1]);

					// gradient magnitude
					double gm = std::sqrt(vg * vg + hg * hg);
					// gradient angle
					double ga = hg != 0 ? std::atan(vg / hg) : 0 ;
					// angle to degrees
					int d = static_cast<int>( (ga >= 0 ? ga : (2 * PI + ga)) * 360 / (2 * PI) );
					// set angle hist
					hist[d] += static_cast<int>( gm );
				}
			}
			// normalize hist, 20 도 단위로 끊는다.
			for (int i = 0; i != 181; ++i) {
				norHist[i/20] += hist[i];
			}
			norHist[8] += norHist[9];

			// add hog
			hog.insert(std::end(hog), std::begin(norHist), std::begin(norHist)+9);
		}
	}

	return hog;
}

