/// @file lbp.h
/// @author locketk9@gmail.com, 2020-03-15
/// @desc set of my lbp(local binary pattern) functions

#pragma once

#include <vector>
#include <algorithm>
#include "ocr_func.h"

// defines
using byte = unsigned char;
using vImg = std::vector<byte>;
using vHist = std::vector<int>;
using size_t = std::size_t;

typedef struct stlbp {
	byte c1 : 1;
	byte c2 : 1;
	byte c3 : 1;
	byte c4 : 1;
	byte c5 : 1;
	byte c6 : 1;
	byte c7 : 1;
	byte c8 : 1;
} lbp_cd;
typedef union unlpb {
	lbp_cd t;
	byte b;
} lbp_t;

/// @brief make_lbp
/// @desc no clockwise lbp
///	c1 c2 c3
/// c4    c5
/// c6 c7 c8
vImg& make_lbp(const vImg &src, size_t width, size_t height, vImg &lbp) {
	if (width < 3 || height < 3 || src.size() != width*height) return lbp;

	// init lbp
	lbp.clear();
	lbp.reserve(src.size());
	lbp.resize(src.size(), 0);

	for (int h = 1; h != height-1; ++h) {
		for (int w = 1; w != width-1; ++w) {
			lbp_t cd{ 0 };
			std::size_t pos = w + (h * width);
			byte c = src[pos];
			cd.t.c1 = c > src[pos - width - 1] ? 0 : 1;
			cd.t.c2 = c > src[pos - width] ? 0 : 1;
			cd.t.c3 = c > src[pos - width + 1] ? 0 : 1;
			cd.t.c4 = c > src[pos - 1] ? 0 : 1;
			cd.t.c5 = c > src[pos + 1] ? 0 : 1;
			cd.t.c6 = c > src[pos + width - 1] ? 0 : 1;
			cd.t.c7 = c > src[pos + width] ? 0 : 1;
			cd.t.c8 = c > src[pos + width + 1] ? 0 : 1;
		
			lbp[pos] = cd.b;
		}
	}


	return lbp;
}

/// @brief get_hist
vHist& get_hist(const vImg& src, vHist&hist) {
	hist.clear();
	auto it = std::max_element(std::begin(src), std::end(src));
	int max = src.at(it-std::begin(src)) + 1;
	hist.reserve(max);
	hist.resize(max, 0);
	for (const byte &el : src) {
		++hist[el];
	}
	return hist;
}


/// @brief calc lbp histogram
vHist lbp_hist(const vImg& lbp, size_t width, size_t height
			, size_t r_x=8, size_t r_y=8) {
	if (width % r_x != 0 || height % r_y != 0) return vHist{ 0 };

	// template pattern
	vHist tp;
		
	int lp_w = (width / r_x);
	int lp_h = (height / r_y);

	//tp.reserve(lp_w * lp_h * 255);

	imgRECT rt_src{ 0, 0, static_cast<int>(width), static_cast<int>(height) };
	for (int h = 0; h != lp_h; ++h) {
		for (int w = 0; w != lp_w; ++w) {
			ch_vec dst(r_x*r_y, 0);
			imgRECT rt_dst( w * r_x, h * r_y, r_x, r_y );
			seg_copy(lbp, rt_src, dst, rt_dst);
			vHist hist;
			get_hist(dst, hist);

			// remove 0 hist
			//vHist::iterator nit = std::remove_if(std::begin(hist), std::end(hist)
			//	, [](int i) {return i == 0; });
			//hist.resize(nit - std::begin(hist));

			// add hist
			tp.insert(std::end(tp), std::begin(hist), std::end(hist));
		}
	}

	// remove 0 hist
	//vHist::iterator nit = std::remove_if(std::begin(tp), std::end(tp)
	//	, [](int i) {return i == 0; });
	//tp.resize(nit - std::begin(tp));
	return tp;
}