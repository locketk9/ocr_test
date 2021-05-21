/// @file orb.h
/// @author locketk9@gmail.com, 2020-03-20
/// @desc set of function orb(Oriented FAST and Rotated BRIEF)
///		  harris source = Author: alexanderb

#pragma once

//#define _USE_MATH_DEFINES

#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <limits>

//#include "./ocr_def.h"
//#include "./ocr_func.h"
#include "./ocr.h"

// defines
using byte = unsigned char;
using vImg = std::vector<byte>;
using vHist = std::vector<int>;
using size_t = std::size_t;
using vPt = std::vector<Point>;

using descriptor_t = vImg;

using vdb_t = std::vector<double>;
/*
template <typename T>
struct image {
	std::vector<T> img;
	size_t cx{ 0 };
	size_t cy{ 0 };
	image() = default;
	image(size_t _cx, size_t _cy, T init) : img(_cx* _cy, init) {
		cx = _cx;
		cy = _cy;
	}

	image& operator = (const image& rhs) {
		img = rhs.img;
		cx = rhs.cx;
		cy = rhs.cy;
		return *this;
	}
};

using image_t = image<double>;
using img_t = image<byte>;

typedef struct derivatives {
	image_t Ix; 
	image_t Iy;
	image_t Ixy;

	derivatives& operator = (const derivatives& rhs) {
		Ix = rhs.Ix; 
		Iy = rhs.Iy; 
		Ixy = rhs.Ixy; 

		return *this;
	}
} derivatives_t;
//using derivatives_t = std::vector<derivatives>;

*/

/// @author skanti
namespace BRIEF {

#define N_DIM_BINARYDESCRIPTOR 256
#define SIZE_BITS_HAMING 64

	//std::vector<float> g_angles(25600);
	vdb_t g_angles(25600);

	void create_synthetic_data(vdb_t& angles) {
		auto const_pi = []() { return std::atan(1) * 4; };
		double M_PI = const_pi();

		std::mt19937 mt(std::time(0));
		std::uniform_real_distribution<float> u_dist(0, 1);
		for (int i = 0; i < angles.size(); i++) {
			angles[i] = u_dist(mt) * M_PI;
		}
	}

	// image.data, HEIGHT_IMAGE, WIDTH_IMAGE, N_CHANNELS, STRIDE_IMAGE,(BRIEF::Feature*) corners.data(), angles.data(), n_features
	descriptor_t compute(const vImg& src // image_src
		, const int height_image
		, const int width_image
		, const int n_channels
		, const int stride_image
		, const vPtd& features
		, const vdb_t& angles
		, const bool be_max=true
		) {

		const int n_features = features.size();

		const size_t ds = be_max ? max(64 * 4, n_features*4) : n_features * 4;
		descriptor_t bd(ds, 0);
		int ps = min(n_features, 16);
		//descriptor_t bd(64 * 4, 0);
		//int ps = 16;
		
		int n_rows = 4;

		// <- maximal range of pattern box: 25/2 = 12, sqrt(12*12 + 12*12) = 17
		int diag_length_pattern = 17;

		alignas(32) char gaussian_bit_pattern_31_x_a[256] = { 8,4,-11,7,2,1,-2,-13,-13,10,-13,-11,7,-4,-13,-9,12,-3,-6,11,4,5,3,-8,-2,-13,-7,-4,-10,5,5,1,9,4,2,-4,-8,4,0,-13,-3,-6,8,0,7,-13,10,-6,10,-13,-13,3,5,-1,3,2,-13,-13,-13,-7,6,-9,-2,-12,3,-7,-3,2,-11,-1,5,-4,-9,-12,10,7,-7,-4,7,-7,-13,-3,7,-13,1,2,-4,-1,7,1,9,-1,-13,7,12,6,5,2,3,2,9,-8,-11,1,6,2,6,3,7,-11,-10,-5,-10,8,4,-10,4,-2,-5,7,-9,-5,8,-9,1,7,-2,11,-12,3,5,0,-9,0,-1,5,3,-13,-5,-4,6,-7,-13,1,4,-2,2,-2,4,-6,-3,7,4,-13,7,7,-7,-8,-13,2,10,-6,8,2,-11,-12,-11,5,-2,-1,-13,-10,-3,2,-9,-4,-4,-6,6,-13,11,7,-1,-4,-7,-13,-7,-8,-5,-13,1,1,9,5,-1,-9,-1,-13,8,2,7,-10,-10,4,3,-4,5,4,-9,0,-12,3,-10,8,-8,2,10,6,-7,-3,-1,-3,-8,4,2,6,3,11,-3,4,2,-10,-13,-13,6,0,-13,-9,-13,5,2,-1,9,11,3,-1,3,-13,5,8,7,-10,7,9,7,-1 };
		alignas(32) char gaussian_bit_pattern_31_y_a[256] = { -3,2,9,-12,-13,-7,-10,-13,-3,4,-8,7,7,-5,2,0,-6,6,-13,-13,7,-3,-7,-7,11,12,3,2,-12,-12,-6,0,11,7,-1,-12,-5,11,-8,-2,-2,9,12,9,-5,-6,7,-3,-9,8,0,3,7,7,-10,-4,0,-7,3,12,-10,-1,-5,5,-10,-7,-2,9,-13,6,-3,-13,-6,-10,2,12,-13,9,-1,6,11,7,-8,-7,-3,-6,3,-13,1,-1,1,-9,-13,7,-5,3,-13,-12,8,6,-12,4,12,12,-9,3,3,-3,8,-5,11,-8,5,-1,-6,12,-2,0,-8,-6,-13,-13,-8,-11,-8,-4,1,-6,-9,7,5,-4,12,7,2,11,5,-4,9,-7,5,6,6,-10,1,-2,-12,-13,1,-10,-13,5,-2,9,1,-8,-4,11,6,4,-5,-5,-3,-12,-2,-13,0,-3,-13,-8,-11,-2,9,-3,-13,6,12,-11,-3,11,11,-5,12,-8,1,-12,-2,5,-1,7,5,0,12,-8,11,-3,-10,1,-11,-13,-13,-10,-8,-6,12,2,-13,-13,9,3,1,2,-10,-13,-12,2,6,8,10,-9,-13,-7,-2,2,-5,-9,-1,-1,0,-11,-4,-6,7,12,0,-1,3,8,-6,-9,7,-6,5,-3,0,4,-6,0,8,9,-4,4,3,-7,0,-6 };
		alignas(32) char gaussian_bit_pattern_31_x_b[256] = { 9,7,-8,12,2,1,-2,-11,-12,11,-8,-9,12,-3,-12,-7,12,-2,-4,12,5,10,6,-6,-1,-8,-5,-3,-6,6,7,4,11,4,4,-2,-7,9,1,-8,-2,-4,10,1,11,-11,12,-6,12,-8,-8,7,10,1,5,3,-13,-12,-11,-4,12,-7,0,-7,8,-4,-1,5,-5,0,5,-4,-9,-8,12,12,-6,-3,12,-5,-12,-2,12,-11,12,3,-2,1,8,3,12,-1,-10,10,12,7,6,2,4,12,10,-7,-4,2,7,3,11,8,9,-6,-5,-3,-9,12,6,-8,6,-2,-5,10,-8,-5,9,-9,1,9,-1,12,-6,7,10,2,-5,2,1,7,6,-8,-3,-3,8,-6,-5,3,8,2,12,0,9,-3,-1,12,5,-9,8,7,-7,-7,-12,3,12,-6,9,2,-10,-7,-10,11,-1,0,-12,-10,-2,3,-4,-3,-2,-4,6,-5,12,12,0,-3,-6,-8,-6,-6,-4,-8,5,10,10,10,1,-6,1,-8,10,3,12,-5,-8,8,8,-3,10,5,-4,3,-6,4,-10,12,-6,3,11,8,-6,-3,-1,-3,-8,12,3,11,7,12,-3,4,2,-8,-11,-11,11,1,-9,-6,-8,8,3,-1,11,12,3,0,4,-10,12,9,8,-10,12,10,12,0 };
		alignas(32) char gaussian_bit_pattern_31_y_b[256] = { 5,-12,2,-13,12,6,-4,-8,-9,9,-9,12,6,0,-3,5,-1,12,-8,-8,1,-3,12,-2,-10,10,-3,7,11,-7,-1,-5,-13,12,4,7,-10,12,-13,2,3,-9,7,3,-10,0,1,12,-4,-12,-4,8,-7,-12,6,-10,5,12,8,7,8,-6,12,5,-13,5,-7,-11,-13,-1,2,12,6,-4,-3,12,5,4,2,1,5,-6,-7,-12,12,0,-13,9,-6,12,6,3,5,12,9,11,10,3,-6,-13,3,9,-6,-8,-4,-2,0,-8,3,-4,10,12,0,-6,-11,7,7,12,2,12,-8,-2,-13,0,-2,1,-4,-11,4,12,8,8,-13,12,7,-9,-8,9,-3,-12,0,12,-2,10,-4,-13,12,-6,3,-5,1,-11,-7,-5,6,6,1,-8,-8,9,3,7,-8,8,3,-9,-5,8,12,9,-5,11,-13,2,0,-10,-7,9,11,5,6,-2,7,-2,7,-13,-8,-9,5,10,-13,-13,-1,-9,-13,2,12,-10,-6,-6,-9,-7,-13,5,-13,-3,-12,-1,3,-9,1,-8,9,12,-5,7,-8,-12,5,9,5,4,3,12,11,-13,12,4,6,12,1,1,1,-13,-13,4,-2,-3,-2,10,-9,-1,-2,-8,5,10,5,5,11,-6,-12,9,4,-2,-2,-11 };


		for (int j = 0; j != features.size(); ++j) {
			//if ((features[j].x <= diag_length_pattern) || features[j].x >= (width_image - diag_length_pattern)
			//	|| (features[j].y <= diag_length_pattern) || features[j].y >= (height_image - diag_length_pattern))
			//	continue; // return bd;
			float cos_angle = std::cos(angles[j]);
			float sin_angle = std::sin(angles[j]);
			const auto image_center = src.begin() + static_cast<int>(features[j % features.size()].y) * stride_image + static_cast<int>(features[j%features.size()].x) * n_channels;
			int ic_pos = +static_cast<int>(features[j % features.size()].y) * stride_image + static_cast<int>(features[j % features.size()].x) * n_channels;
			// N_DIM_BINARYDESCRIPTOR / SIZE_BITS_HAMING = 4
			alignas(16) std::vector<int32_t> ia_x(ds, 0);
			alignas(16) std::vector<int32_t> ia_y(ds, 0);
			alignas(16) std::vector<int32_t> ib_x(ds, 0);
			alignas(16) std::vector<int32_t> ib_y(ds, 0);

			for (int i = 0; i != ds; ++i) {
				ia_x[i] = std::round((gaussian_bit_pattern_31_x_a[i % 256] * cos_angle - gaussian_bit_pattern_31_y_a[i % 256] * sin_angle));
				ia_y[i] = std::round((gaussian_bit_pattern_31_x_a[i % 256] * sin_angle + gaussian_bit_pattern_31_y_a[i % 256] * cos_angle));
				ib_x[i] = std::round((gaussian_bit_pattern_31_x_b[i % 256] * cos_angle - gaussian_bit_pattern_31_y_b[i % 256] * sin_angle));
				ib_y[i] = std::round((gaussian_bit_pattern_31_x_b[i % 256] * sin_angle + gaussian_bit_pattern_31_y_b[i % 256] * cos_angle));
			}

			int gv_w = ds / 8;
//#define GET_VALUE(i, j) (*(image_center + ia_y[i*gv_w + j]*stride_image + ia_x[i*gv_w + j]) < *(image_center + ib_y[i*gv_w + j]*stride_image + ib_x[i*gv_w + j])) << j
			auto GET_VALUE = [&](int i, int j) {
				int32_t ayv = ia_y[i * gv_w + j];
				int32_t axv = ia_x[i * gv_w + j];
				int av_pos = ic_pos + (ayv * stride_image + axv);
				av_pos = av_pos < 0 ? 0 : (av_pos > src.size()-1 ? src.size() - 1 : av_pos);
				byte a = src[av_pos];
				int32_t byv = ib_y[i * gv_w + j];
				int32_t bxv = ib_x[i * gv_w + j];
				int bv_pos = ic_pos + (byv * stride_image + bxv);
				bv_pos = bv_pos < 0 ? 0 : (bv_pos > src.size() - 1 ? src.size() - 1 : bv_pos);
				byte b = src[bv_pos];

				//return (*(image_center + ia_y[i * gv_w + j] * stride_image + ia_x[i * gv_w + j]) < *(image_center + ib_y[i * gv_w + j] * stride_image + ib_x[i * gv_w + j])) << j;
				return (a < b) << j;
			};

			alignas(32) int32_t f[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

			f[0] |= GET_VALUE(0, 0);
			f[0] |= GET_VALUE(0, 1);
			f[0] |= GET_VALUE(0, 2);
			f[0] |= GET_VALUE(0, 3);
			f[0] |= GET_VALUE(0, 4);
			f[0] |= GET_VALUE(0, 5);
			f[0] |= GET_VALUE(0, 6);
			f[0] |= GET_VALUE(0, 7);
			f[0] |= GET_VALUE(0, 8);
			f[0] |= GET_VALUE(0, 9);
			f[0] |= GET_VALUE(0, 10);
			f[0] |= GET_VALUE(0, 11);
			f[0] |= GET_VALUE(0, 12);
			f[0] |= GET_VALUE(0, 13);
			f[0] |= GET_VALUE(0, 14);
			f[0] |= GET_VALUE(0, 15);
			f[0] |= GET_VALUE(0, 16);
			f[0] |= GET_VALUE(0, 17);
			f[0] |= GET_VALUE(0, 18);
			f[0] |= GET_VALUE(0, 19);
			f[0] |= GET_VALUE(0, 20);
			f[0] |= GET_VALUE(0, 21);
			f[0] |= GET_VALUE(0, 22);
			f[0] |= GET_VALUE(0, 23);
			f[0] |= GET_VALUE(0, 24);
			f[0] |= GET_VALUE(0, 25);
			f[0] |= GET_VALUE(0, 26);
			f[0] |= GET_VALUE(0, 27);
			f[0] |= GET_VALUE(0, 28);
			f[0] |= GET_VALUE(0, 29);
			f[0] |= GET_VALUE(0, 30);
			f[0] |= GET_VALUE(0, 31);

			f[1] |= GET_VALUE(1, 0);
			f[1] |= GET_VALUE(1, 1);
			f[1] |= GET_VALUE(1, 2);
			f[1] |= GET_VALUE(1, 3);
			f[1] |= GET_VALUE(1, 4);
			f[1] |= GET_VALUE(1, 5);
			f[1] |= GET_VALUE(1, 6);
			f[1] |= GET_VALUE(1, 7);
			f[1] |= GET_VALUE(1, 8);
			f[1] |= GET_VALUE(1, 9);
			f[1] |= GET_VALUE(1, 10);
			f[1] |= GET_VALUE(1, 11);
			f[1] |= GET_VALUE(1, 12);
			f[1] |= GET_VALUE(1, 13);
			f[1] |= GET_VALUE(1, 14);
			f[1] |= GET_VALUE(1, 15);
			f[1] |= GET_VALUE(1, 16);
			f[1] |= GET_VALUE(1, 17);
			f[1] |= GET_VALUE(1, 18);
			f[1] |= GET_VALUE(1, 19);
			f[1] |= GET_VALUE(1, 20);
			f[1] |= GET_VALUE(1, 21);
			f[1] |= GET_VALUE(1, 22);
			f[1] |= GET_VALUE(1, 23);
			f[1] |= GET_VALUE(1, 24);
			f[1] |= GET_VALUE(1, 25);
			f[1] |= GET_VALUE(1, 26);
			f[1] |= GET_VALUE(1, 27);
			f[1] |= GET_VALUE(1, 28);
			f[1] |= GET_VALUE(1, 29);
			f[1] |= GET_VALUE(1, 30);
			f[1] |= GET_VALUE(1, 31);

			f[2] |= GET_VALUE(2, 0);
			f[2] |= GET_VALUE(2, 1);
			f[2] |= GET_VALUE(2, 2);
			f[2] |= GET_VALUE(2, 3);
			f[2] |= GET_VALUE(2, 4);
			f[2] |= GET_VALUE(2, 5);
			f[2] |= GET_VALUE(2, 6);
			f[2] |= GET_VALUE(2, 7);
			f[2] |= GET_VALUE(2, 8);
			f[2] |= GET_VALUE(2, 9);
			f[2] |= GET_VALUE(2, 10);
			f[2] |= GET_VALUE(2, 11);
			f[2] |= GET_VALUE(2, 12);
			f[2] |= GET_VALUE(2, 13);
			f[2] |= GET_VALUE(2, 14);
			f[2] |= GET_VALUE(2, 15);
			f[2] |= GET_VALUE(2, 16);
			f[2] |= GET_VALUE(2, 17);
			f[2] |= GET_VALUE(2, 18);
			f[2] |= GET_VALUE(2, 19);
			f[2] |= GET_VALUE(2, 20);
			f[2] |= GET_VALUE(2, 21);
			f[2] |= GET_VALUE(2, 22);
			f[2] |= GET_VALUE(2, 23);
			f[2] |= GET_VALUE(2, 24);
			f[2] |= GET_VALUE(2, 25);
			f[2] |= GET_VALUE(2, 26);
			f[2] |= GET_VALUE(2, 27);
			f[2] |= GET_VALUE(2, 28);
			f[2] |= GET_VALUE(2, 29);
			f[2] |= GET_VALUE(2, 30);
			f[2] |= GET_VALUE(2, 31);

			f[3] |= GET_VALUE(3, 0);
			f[3] |= GET_VALUE(3, 1);
			f[3] |= GET_VALUE(3, 2);
			f[3] |= GET_VALUE(3, 3);
			f[3] |= GET_VALUE(3, 4);
			f[3] |= GET_VALUE(3, 5);
			f[3] |= GET_VALUE(3, 6);
			f[3] |= GET_VALUE(3, 7);
			f[3] |= GET_VALUE(3, 8);
			f[3] |= GET_VALUE(3, 9);
			f[3] |= GET_VALUE(3, 10);
			f[3] |= GET_VALUE(3, 11);
			f[3] |= GET_VALUE(3, 12);
			f[3] |= GET_VALUE(3, 13);
			f[3] |= GET_VALUE(3, 14);
			f[3] |= GET_VALUE(3, 15);
			f[3] |= GET_VALUE(3, 16);
			f[3] |= GET_VALUE(3, 17);
			f[3] |= GET_VALUE(3, 18);
			f[3] |= GET_VALUE(3, 19);
			f[3] |= GET_VALUE(3, 20);
			f[3] |= GET_VALUE(3, 21);
			f[3] |= GET_VALUE(3, 22);
			f[3] |= GET_VALUE(3, 23);
			f[3] |= GET_VALUE(3, 24);
			f[3] |= GET_VALUE(3, 25);
			f[3] |= GET_VALUE(3, 26);
			f[3] |= GET_VALUE(3, 27);
			f[3] |= GET_VALUE(3, 28);
			f[3] |= GET_VALUE(3, 29);
			f[3] |= GET_VALUE(3, 30);
			f[3] |= GET_VALUE(3, 31);

			f[4] |= GET_VALUE(4, 0);
			f[4] |= GET_VALUE(4, 1);
			f[4] |= GET_VALUE(4, 2);
			f[4] |= GET_VALUE(4, 3);
			f[4] |= GET_VALUE(4, 4);
			f[4] |= GET_VALUE(4, 5);
			f[4] |= GET_VALUE(4, 6);
			f[4] |= GET_VALUE(4, 7);
			f[4] |= GET_VALUE(4, 8);
			f[4] |= GET_VALUE(4, 9);
			f[4] |= GET_VALUE(4, 10);
			f[4] |= GET_VALUE(4, 11);
			f[4] |= GET_VALUE(4, 12);
			f[4] |= GET_VALUE(4, 13);
			f[4] |= GET_VALUE(4, 14);
			f[4] |= GET_VALUE(4, 15);
			f[4] |= GET_VALUE(4, 16);
			f[4] |= GET_VALUE(4, 17);
			f[4] |= GET_VALUE(4, 18);
			f[4] |= GET_VALUE(4, 19);
			f[4] |= GET_VALUE(4, 20);
			f[4] |= GET_VALUE(4, 21);
			f[4] |= GET_VALUE(4, 22);
			f[4] |= GET_VALUE(4, 23);
			f[4] |= GET_VALUE(4, 24);
			f[4] |= GET_VALUE(4, 25);
			f[4] |= GET_VALUE(4, 26);
			f[4] |= GET_VALUE(4, 27);
			f[4] |= GET_VALUE(4, 28);
			f[4] |= GET_VALUE(4, 29);
			f[4] |= GET_VALUE(4, 30);
			f[4] |= GET_VALUE(4, 31);

			f[5] |= GET_VALUE(5, 0);
			f[5] |= GET_VALUE(5, 1);
			f[5] |= GET_VALUE(5, 2);
			f[5] |= GET_VALUE(5, 3);
			f[5] |= GET_VALUE(5, 4);
			f[5] |= GET_VALUE(5, 5);
			f[5] |= GET_VALUE(5, 6);
			f[5] |= GET_VALUE(5, 7);
			f[5] |= GET_VALUE(5, 8);
			f[5] |= GET_VALUE(5, 9);
			f[5] |= GET_VALUE(5, 10);
			f[5] |= GET_VALUE(5, 11);
			f[5] |= GET_VALUE(5, 12);
			f[5] |= GET_VALUE(5, 13);
			f[5] |= GET_VALUE(5, 14);
			f[5] |= GET_VALUE(5, 15);
			f[5] |= GET_VALUE(5, 16);
			f[5] |= GET_VALUE(5, 17);
			f[5] |= GET_VALUE(5, 18);
			f[5] |= GET_VALUE(5, 19);
			f[5] |= GET_VALUE(5, 20);
			f[5] |= GET_VALUE(5, 21);
			f[5] |= GET_VALUE(5, 22);
			f[5] |= GET_VALUE(5, 23);
			f[5] |= GET_VALUE(5, 24);
			f[5] |= GET_VALUE(5, 25);
			f[5] |= GET_VALUE(5, 26);
			f[5] |= GET_VALUE(5, 27);
			f[5] |= GET_VALUE(5, 28);
			f[5] |= GET_VALUE(5, 29);
			f[5] |= GET_VALUE(5, 30);
			f[5] |= GET_VALUE(5, 31);

			f[6] |= GET_VALUE(6, 0);
			f[6] |= GET_VALUE(6, 1);
			f[6] |= GET_VALUE(6, 2);
			f[6] |= GET_VALUE(6, 3);
			f[6] |= GET_VALUE(6, 4);
			f[6] |= GET_VALUE(6, 5);
			f[6] |= GET_VALUE(6, 6);
			f[6] |= GET_VALUE(6, 7);
			f[6] |= GET_VALUE(6, 8);
			f[6] |= GET_VALUE(6, 9);
			f[6] |= GET_VALUE(6, 10);
			f[6] |= GET_VALUE(6, 11);
			f[6] |= GET_VALUE(6, 12);
			f[6] |= GET_VALUE(6, 13);
			f[6] |= GET_VALUE(6, 14);
			f[6] |= GET_VALUE(6, 15);
			f[6] |= GET_VALUE(6, 16);
			f[6] |= GET_VALUE(6, 17);
			f[6] |= GET_VALUE(6, 18);
			f[6] |= GET_VALUE(6, 19);
			f[6] |= GET_VALUE(6, 20);
			f[6] |= GET_VALUE(6, 21);
			f[6] |= GET_VALUE(6, 22);
			f[6] |= GET_VALUE(6, 23);
			f[6] |= GET_VALUE(6, 24);
			f[6] |= GET_VALUE(6, 25);
			f[6] |= GET_VALUE(6, 26);
			f[6] |= GET_VALUE(6, 27);
			f[6] |= GET_VALUE(6, 28);
			f[6] |= GET_VALUE(6, 29);
			f[6] |= GET_VALUE(6, 30);
			f[6] |= GET_VALUE(6, 31);

			f[7] |= GET_VALUE(7, 0);
			f[7] |= GET_VALUE(7, 1);
			f[7] |= GET_VALUE(7, 2);
			f[7] |= GET_VALUE(7, 3);
			f[7] |= GET_VALUE(7, 4);
			f[7] |= GET_VALUE(7, 5);
			f[7] |= GET_VALUE(7, 6);
			f[7] |= GET_VALUE(7, 7);
			f[7] |= GET_VALUE(7, 8);
			f[7] |= GET_VALUE(7, 9);
			f[7] |= GET_VALUE(7, 10);
			f[7] |= GET_VALUE(7, 11);
			f[7] |= GET_VALUE(7, 12);
			f[7] |= GET_VALUE(7, 13);
			f[7] |= GET_VALUE(7, 14);
			f[7] |= GET_VALUE(7, 15);
			f[7] |= GET_VALUE(7, 16);
			f[7] |= GET_VALUE(7, 17);
			f[7] |= GET_VALUE(7, 18);
			f[7] |= GET_VALUE(7, 19);
			f[7] |= GET_VALUE(7, 20);
			f[7] |= GET_VALUE(7, 21);
			f[7] |= GET_VALUE(7, 22);
			f[7] |= GET_VALUE(7, 23);
			f[7] |= GET_VALUE(7, 24);
			f[7] |= GET_VALUE(7, 25);
			f[7] |= GET_VALUE(7, 26);
			f[7] |= GET_VALUE(7, 27);
			f[7] |= GET_VALUE(7, 28);
			f[7] |= GET_VALUE(7, 29);
			f[7] |= GET_VALUE(7, 30);
			f[7] |= GET_VALUE(7, 31);

			//_mm_store_si128((__m128i*)(&bd(0, 0) + j * n_rows + 0 * 2), _mm_load_si128((const __m128i*)(f + 0 * 4)));
			//_mm_store_si128((__m128i*)(&bd(0, 0) + j * n_rows + 1 * 2), _mm_load_si128((const __m128i*)(f + 1 * 4)));
			int posx = j * n_rows + 0 * 2;
			int posy = j * n_rows + 1 * 2;
			posx = posx > bd.size()- ps ? posx - ps : posx-(posx% ps);
			posy = posy > bd.size()- ps ? posy - ps : posy-(posy% ps);
			memcpy(&bd[0] + posx , (f + 0 * 4), ps);
			memcpy(&bd[0] + posy, (f + 1 * 4), ps);

		}

		//bd.erase(
		//	std::remove_if(
		//		bd.begin(),
		//		bd.end(),
		//		[](auto const& d)
		//{
		//	return d=='\0';
		//}
		//),
		//	bd.end() // HERE!!
		//	);
		return bd;
	}
}


/// @brief fast( Features from Accelerated Segment Test ) algorithm
/// @param t : threshold for ip weight
/// @parma cont : continueous key count threshold
/// @param v : min threshold sum of diff between keypoint of sircle
///	@return keypoints of corner
const vPt fast(const vImg &src, size_t cx, size_t cy
	, int t=50, size_t cont=12) {
	if (src.size() == 0 || src.size() != cx * cy) return vPt{};

	vPt pts;

	size_t tv = t * cont;
	size_t r = 3;
	for (size_t y = r; y != cy-r; ++y) {
		for (size_t x = r; x != cx-r; ++x) {
			size_t c = x + (y * cx);
			byte ip = src[c] + t;
			byte k1 = src[c - r * cx], k5 = src[c + r], k9 = src[c + r * cx], k13 = src[c - r];
			byte k2, k3, k4, k6, k7, k8, k10, k11, k12, k14, k15, k16;
			k2 = k3 = k4 = k6 = k7 = k8 = k10 = k11 = k12 = k14 = k15 = k16 = 0;

			int hc = 0, lc = 0;
			ip > k1 ? ++hc : ++lc;
			ip > k5 ? ++hc : ++lc;
			ip > k9 ? ++hc : ++lc;
			ip > k13 ? ++hc : ++lc;
			// key point is valid
			if (hc>2 || lc>2) {
				ip > (k2=src[c - r * cx + 1]) ? ++hc : ++lc; // k2
				ip > (k3=src[c - (r-1) * cx + 2]) ? ++hc : ++lc; // k3
				ip > (k4=src[c + r - cx]) ? ++hc : ++lc; // k4
				ip > (k6=src[c + r + cx]) ? ++hc : ++lc;	// k6
				ip > (k7=src[c + (r - 1) * cx + 2]) ? ++hc : ++lc;	// k7
				ip > (k8=src[c + r * cx + 1]) ? ++hc : ++lc; // k8
				ip > (k10=src[c + r * cx - 1]) ? ++hc : ++lc;	// k10
				ip > (k11=src[c + (r - 1) * cx - 2]) ? ++hc : ++lc;	// k11
				ip > (k12=src[c - r + cx]) ? ++hc : ++lc;	// k12
				ip > (k14=src[c - r - cx]) ? ++hc : ++lc;	// k14
				ip > (k15=src[c - (r - 1) * cx - 2]) ? ++hc : ++lc; // k15
				ip > (k16=src[c - r * cx - 1]) ? ++hc : ++lc; // k16
			}

			// filter low v
			vImg ks{ k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14,k15,k16 };
			int hv = 0, lv = 0;
			ip -= t; // org ip value
			for (byte const k : ks) {
				int hd = k - ip, ld=ip-k ;
				hv += (hd > t ? hd : 0);
				lv += (ld > t ? ld : 0);
			}
			int v = max(hv, lv);
			if (v >= tv && ( hc >= cont || lc >=cont )) {								
				pts.push_back(Point(x, y));
			}
		}
	}
	
	return pts;
}
/// @brief apply angle img fast
const vPtd fast2(const img_t img, const std::vector<imgRECT>& layerinfo
	, int t = 50, size_t cont = 12) {

	vPtd pts;

	size_t tv = t * cont;
	size_t r = 3;
	int patchSize = r * r;

	// pre-compute the end of a row in a circular patch
	int halfPatchSize = patchSize / 2;
	std::vector<int> umax(halfPatchSize + 2);
	{
		int v, v0, vmax = std::floor(halfPatchSize * std::sqrt(2.f) / 2 + 1);
		int vmin = std::ceil(halfPatchSize * std::sqrt(2.f) / 2);
		for (v = 0; v <= vmax; ++v)
			umax[v] = std::round(std::sqrt((double)halfPatchSize * halfPatchSize - v * v));

		// Make sure we are symmetric
		for (v = halfPatchSize, v0 = 0; v >= vmin; --v)
		{
			while (umax[v0] == umax[v0 + 1])
				++v0;
			umax[v] = v0;
			++v0;
		}
	}

	{
		const vImg& src = img.img;
		size_t cx = img.cx;
		size_t cy = img.cy;
		int half_k = halfPatchSize / 2;
		int step = 1;

		for (size_t y = r; y != cy - r; ++y) {
			for (size_t x = r; x != cx - r; ++x) {
				size_t c = x + (y * cx);
				byte ip = src[c] + t;
				byte k1 = src[c - r * cx], k5 = src[c + r], k9 = src[c + r * cx], k13 = src[c - r];
				byte k2, k3, k4, k6, k7, k8, k10, k11, k12, k14, k15, k16;
				k2 = k3 = k4 = k6 = k7 = k8 = k10 = k11 = k12 = k14 = k15 = k16 = 0;

				int hc = 0, lc = 0;
				ip > k1 ? ++hc : ++lc;
				ip > k5 ? ++hc : ++lc;
				ip > k9 ? ++hc : ++lc;
				ip > k13 ? ++hc : ++lc;
				// key point is valid
				if (hc > 2 || lc > 2) {
					ip > (k2 = src[c - r * cx + 1]) ? ++hc : ++lc; // k2
					ip > (k3 = src[c - (r - 1) * cx + 2]) ? ++hc : ++lc; // k3
					ip > (k4 = src[c + r - cx]) ? ++hc : ++lc; // k4
					ip > (k6 = src[c + r + cx]) ? ++hc : ++lc;	// k6
					ip > (k7 = src[c + (r - 1) * cx + 2]) ? ++hc : ++lc;	// k7
					ip > (k8 = src[c + r * cx + 1]) ? ++hc : ++lc; // k8
					ip > (k10 = src[c + r * cx - 1]) ? ++hc : ++lc;	// k10
					ip > (k11 = src[c + (r - 1) * cx - 2]) ? ++hc : ++lc;	// k11
					ip > (k12 = src[c - r + cx]) ? ++hc : ++lc;	// k12
					ip > (k14 = src[c - r - cx]) ? ++hc : ++lc;	// k14
					ip > (k15 = src[c - (r - 1) * cx - 2]) ? ++hc : ++lc; // k15
					ip > (k16 = src[c - r * cx - 1]) ? ++hc : ++lc; // k16
				}

				// filter low v
				vImg ks{ k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14,k15,k16 };
				int hv = 0, lv = 0;
				ip -= t; // org ip value
				for (byte const k : ks) {
					int hd = k - ip, ld = ip - k;
					hv += (hd > t ? hd : 0);
					lv += (ld > t ? ld : 0);
				}
				int v = max(hv, lv);
				if (v >= tv && (hc >= cont || lc >= cont)) {
					int o = -1;
					for (auto const& rt : layerinfo) {
						++o;
						if (rt.x >= x && rt.y >= y
							&& rt.cx <= x && rt.cy <= y)
							break;
					}
					const imgRECT& layer = layerinfo[o];
					const uchar* center = &(src[((y + layer.y)* layer.cx + x + layer.x)]);

					int m_01 = 0, m_10 = 0;
					// Treat the center line differently, v=0
					for (int u = -half_k; u <= half_k; ++u)
						m_10 += u * center[u];

					// Go line by line in the circular patch
					for (int v = 1; v <= half_k; ++v)
					{
						// Proceed over the two lines
						int v_sum = 0;
						int d = umax[v];
						for (int u = -d; u <= d; ++u)
						{
							int val_plus = center[u + v * step], val_minus = center[u - v * step];
							v_sum += (val_plus - val_minus);
							m_10 += u * (val_plus + val_minus);
						}
						m_01 += v * v_sum;
					}
					double a = std::atan2((double)m_01, (double)m_10);
					pts.push_back(PointData(ip, x, y, a, o));
				}
			}
		}
	}

	return pts;
}


/// @brief build pyramid
/// @return ?
int build_pyramid(img_t const &base
	, std::vector<img_t> &pyr, int nOctaveLayers, int nOctaves)	{

	pyr.resize(nOctaves * (nOctaveLayers + 3));
	imgRECT base_rt(0, 0, base.cx, base.cy);

	for (int o = 0; o != nOctaves; ++o)
	{
		for (int i = 0; i != nOctaveLayers + 3; ++i)
		{
			img_t& dst = pyr[o * (nOctaveLayers + 3) + i];
			if (o == 0 && i == 0)
				dst = base;
			// base of new octave is halved image from end of previous octave
			else if (i == 0)
			{
				const img_t& src = pyr[(o - 1) * (nOctaveLayers + 3) + nOctaveLayers];
				imgRECT src_rt(0, 0, src.cx, src.cy);
				imgRECT dst_rt(0, 0, src.cx * 0.8, src.cy * 0.8);
				resize(src.img, src_rt, dst.img, dst_rt);
			}
			//else
			//{
			//	const img_t& src = pyr[o * (nOctaveLayers + 3) + i - 1];
			//	imgRECT src_rt(0, 0, src.cx, src.cy);
			//	imgRECT dst_rt(0, 0, src.cx / 2, src.cy / 2);
			//	resize(src.img, src_rt, dst.img, dst_rt);
			//}
		}
	}

	return 0;
}

/// @brief brief ( Binary Robust Independent Elementary Features) algorithm
/// @return descriptor_ts
const descriptor_t brief(const vImg& src, size_t cx, size_t cy
	, int t = 50, size_t cont = 12) {
	if (src.size() != cx * cy) return descriptor_t{};

	descriptor_t d;

	vImg kimg(src.size(), 0);

	size_t tv = t * cont;
	size_t r = 3;
	for (size_t y = r; y != cy - r; ++y) {
		for (size_t x = r; x != cx - r; ++x) {
			size_t c = x + (y * cx);
			byte ip = src[c] + t;
			byte k1 = src[c - r * cx], k5 = src[c + r], k9 = src[c + r * cx], k13 = src[c - r];
			byte k2, k3, k4, k6, k7, k8, k10, k11, k12, k14, k15, k16;
			k2 = k3 = k4 = k6 = k7 = k8 = k10 = k11 = k12 = k14 = k15 = k16 = 0;

			int hc = 0, lc = 0;
			ip > k1 ? ++hc : ++lc;
			ip > k5 ? ++hc : ++lc;
			ip > k9 ? ++hc : ++lc;
			ip > k13 ? ++hc : ++lc;
			// key point is valid
			if (hc > 2 || lc > 2) {
				ip > (k2 = src[c - r * cx + 1]) ? ++hc : ++lc; // k2
				ip > (k3 = src[c - (r - 1) * cx + 2]) ? ++hc : ++lc; // k3
				ip > (k4 = src[c + r - cx]) ? ++hc : ++lc; // k4
				ip > (k6 = src[c + r + cx]) ? ++hc : ++lc;	// k6
				ip > (k7 = src[c + (r - 1) * cx + 2]) ? ++hc : ++lc;	// k7
				ip > (k8 = src[c + r * cx + 1]) ? ++hc : ++lc; // k8
				ip > (k10 = src[c + r * cx - 1]) ? ++hc : ++lc;	// k10
				ip > (k11 = src[c + (r - 1) * cx - 2]) ? ++hc : ++lc;	// k11
				ip > (k12 = src[c - r + cx]) ? ++hc : ++lc;	// k12
				ip > (k14 = src[c - r - cx]) ? ++hc : ++lc;	// k14
				ip > (k15 = src[c - (r - 1) * cx - 2]) ? ++hc : ++lc; // k15
				ip > (k16 = src[c - r * cx - 1]) ? ++hc : ++lc; // k16
			}

			vImg ks{ k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14,k15,k16 };
			int hv = 0, lv = 0;
			ip -= t; // org ip value
			for (byte const k : ks) {
				int hd = k - ip, ld = ip - k;
				hv += (hd > t ? hd : 0);
				lv += (ld > t ? ld : 0);
			}
			int v = max(hv, lv);
			if (v >= tv && (hc == cont || lc == cont)) {
				// brief algorithm
				byte b = 0;
				size_t step = ks.size() / 2;
				for (int i = 0; i != step; ++i) {
					b |= ((ks[i] > ks[i + step] ? 1 : 0) << i);
				}
				d.push_back(b);		
				kimg[c] = 255;
			}
		}
	}

	saveBMP("kimg1.bmp", src.data(), cx, cy, 1);
	saveBMP("kimg2.bmp", kimg.data(), cx, cy, 1);

	return d;
}

/// @brief orb_descriptor ref opencv
/// @return ?
descriptor_t& orb_descriptor(const img_t& imagePyramid, const std::vector<imgRECT>& layerInfo,
	const fl_vec& layerScale, vPtd& keypoints,
	descriptor_t& descriptors, const vPt& _pattern, int dsize, int wta_k)
{
	auto const_pi = []() { return std::atan(1) * 4; };
	double M_PI = const_pi();

	int step = imagePyramid.cx;
	int j, i, nkeypoints = (int)keypoints.size();

	for (j = 0; j != nkeypoints; ++j)
	{
		const PointData& kpt = keypoints[j];
		const imgRECT& layer = layerInfo[kpt.o];
		float scale = 1.f / layerScale[kpt.o];
		float angle = kpt.a;

		angle *= (float)(M_PI / 180.f);
		float a = (float)cos(angle), b = (float)sin(angle);

		const uchar* center = &(imagePyramid.img[((std::round(kpt.y * scale) + layer.y)*step
			+ std::round(kpt.x * scale) + layer.x)]);
		float x, y;
		int ix, iy;
		const Point* pattern = &_pattern[0];
		uchar* desc = &descriptors[j*dsize];

#if 1
		auto GET_VALUE = [&](int idx, const Point* pattern) {
			int x = pattern[idx].x * a - pattern[idx].y * b ;
			int	y = pattern[idx].x * b + pattern[idx].y * a ;
			int	ix = std::round(x);
			int iy = std::round(y);
			return	*(center + iy * step + ix);
		};
#else

#define GET_VALUE(idx) \
               (x = pattern[idx].x*a - pattern[idx].y*b, \
                y = pattern[idx].x*b + pattern[idx].y*a, \
                ix = cvRound(x), \
                iy = cvRound(y), \
                *(center + iy*step + ix) )
//#else
//#define GET_VALUE(idx) \
//            (x = pattern[idx].x*a - pattern[idx].y*b, \
//            y = pattern[idx].x*b + pattern[idx].y*a, \
//            ix = cvFloor(x), iy = cvFloor(y), \
//            x -= ix, y -= iy, \
//            cvRound(center[iy*step + ix]*(1-x)*(1-y) + center[(iy+1)*step + ix]*(1-x)*y + \
//                    center[iy*step + ix+1]*x*(1-y) + center[(iy+1)*step + ix+1]*x*y))
#endif

		if (wta_k == 2)
		{
			for (i = 0; i < dsize; ++i, pattern += 16)
			{
				int t0, t1, val;
				t0 = GET_VALUE(0, pattern); t1 = GET_VALUE(1, pattern);
				val = t0 < t1;
				t0 = GET_VALUE(2, pattern); t1 = GET_VALUE(3, pattern);
				val |= (t0 < t1) << 1;
				t0 = GET_VALUE(4, pattern); t1 = GET_VALUE(5, pattern);
				val |= (t0 < t1) << 2;
				t0 = GET_VALUE(6, pattern); t1 = GET_VALUE(7, pattern);
				val |= (t0 < t1) << 3;
				t0 = GET_VALUE(8, pattern); t1 = GET_VALUE(9, pattern);
				val |= (t0 < t1) << 4;
				t0 = GET_VALUE(10, pattern); t1 = GET_VALUE(11, pattern);
				val |= (t0 < t1) << 5;
				t0 = GET_VALUE(12, pattern); t1 = GET_VALUE(13, pattern);
				val |= (t0 < t1) << 6;
				t0 = GET_VALUE(14, pattern); t1 = GET_VALUE(15, pattern);
				val |= (t0 < t1) << 7;

				desc[i] = (uchar)val;
			}
		}
		else if (wta_k == 3)
		{
			for (i = 0; i < dsize; ++i, pattern += 12)
			{
				int t0, t1, t2, val;
				t0 = GET_VALUE(0, pattern); t1 = GET_VALUE(1, pattern); t2 = GET_VALUE(2, pattern);
				val = t2 > t1 ? (t2 > t0 ? 2 : 0) : (t1 > t0);

				t0 = GET_VALUE(3, pattern); t1 = GET_VALUE(4, pattern); t2 = GET_VALUE(5, pattern);
				val |= (t2 > t1 ? (t2 > t0 ? 2 : 0) : (t1 > t0)) << 2;

				t0 = GET_VALUE(6, pattern); t1 = GET_VALUE(7, pattern); t2 = GET_VALUE(8, pattern);
				val |= (t2 > t1 ? (t2 > t0 ? 2 : 0) : (t1 > t0)) << 4;

				t0 = GET_VALUE(9, pattern); t1 = GET_VALUE(10, pattern); t2 = GET_VALUE(11, pattern);
				val |= (t2 > t1 ? (t2 > t0 ? 2 : 0) : (t1 > t0)) << 6;

				desc[i] = (uchar)val;
			}
		}
		else if (wta_k == 4)
		{
			for (i = 0; i < dsize; ++i, pattern += 16)
			{
				int t0, t1, t2, t3, u, v, k, val;
				t0 = GET_VALUE(0, pattern); t1 = GET_VALUE(1, pattern);
				t2 = GET_VALUE(2, pattern); t3 = GET_VALUE(3, pattern);
				u = 0, v = 2;
				if (t1 > t0) t0 = t1, u = 1;
				if (t3 > t2) t2 = t3, v = 3;
				k = t0 > t2 ? u : v;
				val = k;

				t0 = GET_VALUE(4, pattern); t1 = GET_VALUE(5, pattern);
				t2 = GET_VALUE(6, pattern); t3 = GET_VALUE(7, pattern);
				u = 0, v = 2;
				if (t1 > t0) t0 = t1, u = 1;
				if (t3 > t2) t2 = t3, v = 3;
				k = t0 > t2 ? u : v;
				val |= k << 2;

				t0 = GET_VALUE(8, pattern); t1 = GET_VALUE(9, pattern);
				t2 = GET_VALUE(10, pattern); t3 = GET_VALUE(11, pattern);
				u = 0, v = 2;
				if (t1 > t0) t0 = t1, u = 1;
				if (t3 > t2) t2 = t3, v = 3;
				k = t0 > t2 ? u : v;
				val |= k << 4;

				t0 = GET_VALUE(12, pattern); t1 = GET_VALUE(13, pattern);
				t2 = GET_VALUE(14, pattern); t3 = GET_VALUE(15, pattern);
				u = 0, v = 2;
				if (t1 > t0) t0 = t1, u = 1;
				if (t3 > t2) t2 = t3, v = 3;
				k = t0 > t2 ? u : v;
				val |= k << 6;

				desc[i] = (uchar)val;
			}
		}
	}

	return descriptors;
}

/// @brief orb algorithm, not completed yet
/// @return ?
/// @param wta_k = 2=haming, 3||4=haming2
descriptor_t make_orb(const vImg& src, size_t cx, size_t cy
	, int wta_k = 4) {
	if (src.size() != cx * cy) return descriptor_t{};

	imgRECT src_rt(0, 0, cx, cy);
#if 1
	// scale 이미지를 크게 만든다. ㅡㅡㅋ
	// make scale img 128*128/64*64/32*32
	imgRECT s_rt(0, 0, 64 + 48 + 32, 64 + 48 + 32);
	vImg scale(s_rt.cx * s_rt.cy, 0);
	vImg img128(64 * 64, 0);
	imgRECT img128_rt(0, 0, 64, 64);
	resize(src, src_rt, img128, img128_rt);
	vImg img64(48 * 48, 0);
	imgRECT img64_rt(0, 0, 48, 48);
	resize(img128, img128_rt, img64, img64_rt);
	vImg img32(32 * 32, 0);
	imgRECT img32_rt(0, 0, 32, 32);
	resize(img64, img64_rt, img32, img32_rt);
	img128_rt.x = 16, img128_rt.y = 16;
	seg_paste(scale, s_rt, img128, img128_rt);
	img64_rt.x = 64 + 16; img64_rt.y = 16;
	seg_paste(scale, s_rt, img64, img64_rt);
	img32_rt.x = 64 + 16; img32_rt.y = 48 + 16;
	seg_paste(scale, s_rt, img32, img32_rt);

	//zs::thin(scale, s_rt, s_rt, false, false, true);

	saveBMP("scale.bmp", scale.data(), s_rt.cx, s_rt.cy, 1);

//	return brief(scale, s_rt.cx, s_rt.cy);
	//return brief(img128, img128_rt.cx, img128_rt.cy);
	//return brief(src, cx, cy, 60);

	std::vector<imgRECT> rects;
	rects.push_back(img128_rt);
	rects.push_back(img64_rt);
	rects.push_back(img32_rt);

	img_t img;
	img.img = scale; img.cx = s_rt.cx; img.cy = s_rt.cy;
	vPtd kps2 = fast2(img, rects);

	vPtd maxima_suppresss(const image_t & harris, vPtd * kps, bool haveKp
		, float percentage, int fr, int suppressionRadius);
	image_t db_img(img.cx, img.cy, 0.0);
	db_img.img = db_vec(img.img.begin(), img.img.end());
	vPtd kps = maxima_suppresss(db_img, &kps2, true, 1, 3, 3);
#endif

	auto getScale = [](int level, int firstLevel, double scaleFactor) {
		return (float)std::pow(scaleFactor, (double)(level - firstLevel));;
	};
	fl_vec layerScale(3);
	for (int level = 0; level < 3; level++)
	{
		float scale = getScale(level, 0, 1.25);
		layerScale[level] = scale;
	}

	int dsize = 32;

	vPt pattern;
	//BRIEF::g_angles
	if (wta_k == 2)
		for (int i = 0; i != dsize*16; ++i)
		{
			Point pt;
			pt.x = BRIEF::g_angles[i];
			pt.y = BRIEF::g_angles[i+dsize];
			pattern.push_back(pt);
		}
	else
	{
		for (int i = 0; i != dsize*16; ++i)
		{
			Point pt;
			pt.x = BRIEF::g_angles[i];
			pt.y = BRIEF::g_angles[i + dsize];
			pattern.push_back(pt);
		}
	}

	descriptor_t d(dsize*kps.size());

	orb_descriptor(img, rects, layerScale, kps
	 , d, pattern, dsize, wta_k);

	return d;
}

/// @brief compute_derivatives
/// @return derivatives_t
derivatives_t& compute_derivatives(const vImg& src, size_t cx, size_t cy
	, derivatives_t& deriv) {
	// compute v-gradient
	//image_t sobel_v(cx, (cy - 2), 0.0);
	image_t sobel_v(cx, (cy), 0.0);
	for (int y = 1; y != cy - 1; ++y) {
		for (int x = 1; x != cx-1; ++x) {

			double a1 = src[(y - 1) * cx + x];
			double a2 = src[(y) * cx + x];
			double a3 = src[(y + 1) * cx + x];

			sobel_v.img[(y - 1) * sobel_v.cx + x] = a1 + a2 + a2 + a3;
		}
	}
	// compute h-gradient
	//image_t sobel_h((cx - 2), cy, 0.0);
	image_t sobel_h((cx), cy, 0.0);
	for (int y = 1; y != cy-1; ++y) {
		for (int x = 1; x != cx - 1; ++x) {

			double a1 = src[y * cx + x - 1];
			double a2 = src[y * cx + x];
			double a3 = src[y * cx + x + 1];

			sobel_h.img[y * sobel_h.cx + x - 1] = a1 + a2 + a2 + a3;
		}
	}
	// Apply Sobel filter to compute 1st derivatives
	//image_t Ix((cx - 2), (cy - 2), 0.0);
	//image_t Iy((cx - 2), (cy - 2), 0.0);
	//image_t Ixy((cx - 2), (cy - 2), 0.0);
	image_t Ix((cx), (cy), 0.0);
	image_t Iy((cx), (cy), 0.0);
	image_t Ixy((cx), (cy), 0.0);

	for (int y = 0 ; y != cy-2; ++y) {
		for (int x = 0; x != cx-2; ++x) {
			Ix.img[y * Ix.cx + x] = sobel_h.img[(y*sobel_h.cx+x)] - sobel_h.img[((y + 2)*sobel_h.cx + x)];
			Iy.img[y * Iy.cx + x] = -sobel_v.img[(y * sobel_v.cx + x)] + sobel_v.img[(y * sobel_v.cx + x + 2)];
			Ixy.img[y * Ixy.cx + x] = Ix.img[(y * Ix.cx + x)] * Iy.img[(y * Iy.cx + x)];
		}
	}

	deriv.Ix = Ix;
	deriv.Iy = Iy;
	deriv.Ixy = Ixy;
	
	return deriv;
}


/// @brief gauss_filter
/// @return vdb_t
image_t gauss_filter(const vdb_t &img, size_t cx, size_t cy
				, int fr=1) {
	auto const_pi = []() { return std::atan(1) * 4; };
	double M_PI = const_pi();

	size_t w = cx - fr, h = cy - fr;
	// Helper Mats for better time complexity
	//image_t gaussHelperV((cx - fr * 2), (cy - fr * 2), 0.0);
	image_t gaussHelperV((cx), (cy), 0.0);
	for (int r = fr; r != h; ++r) {
		for (int c = fr; c < w; ++c) {
			float res = 0;

			for (int x = -fr; x <= fr; ++x) {
				float m = 1 / std::sqrt(2 * M_PI) * std::exp(-0.5 * x * x);

				res += m * img[(r - fr)*cx + c - fr];
			}

			gaussHelperV.img[(r - fr)* gaussHelperV.cx + c - fr] = res;
		}
	}

	//image_t gauss((cx - fr * 2), (cy - fr * 2), 0.0);
	image_t gauss((cx), (cy), 0.0);
	for (int r = fr; r != h; ++r) {
		for (int c = fr; c != w; ++c) {
			float res = 0;

			for (int x = -fr; x <= fr; ++x) {
				float m = 1 / std::sqrt(2 * M_PI) * std::exp(-0.5 * x * x);

				res += m * gaussHelperV.img[(r - fr)* gaussHelperV.cx + c - fr];
			}

			gauss.img[(r - fr)* gauss.cx + c - fr] = res;
		}
	}

	return gauss;
}


/// @brief mean_filter
/// @return vdb_t
image_t mean_filter(const vdb_t& img, size_t cx, size_t cy
	, int fr = 1) {
	
	// integral
	vdb_t integral(cy * cx, 0.0);
	integral[0] = img[0];
	for (int i = 1; i != cx; ++i) {
		integral[0 * cx + i] =
			integral[(0*cx + i - 1)]
			+ img[0*cx + i];
	}
	for (int j = 1; j != cy; ++j) {
		integral[(j*cx + 0)] =
			integral[(j - 1)*cx + 0]
			+ img[j*cx + 0];
	}
	for (int i = 1; i != cx; ++i) {
		for (int j = 1; j != cy; ++j) {
			integral[j*cx + i]=
				img[j*cx + i]
				+ integral[(j - 1)*cx + i]
				+ integral[(j*cx + i - 1)]
				- integral[(j - 1)*cx + i - 1];
		}
	}
	
	size_t w = cx - fr, h = cy - fr;
	//image_t mean((cx - fr * 2), (cy - fr * 2), 0.0);
	image_t mean((cx), (cy), 0.0);

	for (int r = fr; r != h; ++r) {
		for (int c = fr; c != w; ++c) {
			mean.img[(r - fr)*mean.cx + c - fr] =
				integral[(r + fr) * cx + c + fr]
				+ integral[(r - fr) * cx + c - fr]
				- integral[(r + fr) * cx + c - fr]
				- integral[(r - fr) * cx + c + fr];
		}
	}

	return mean;
}

/// @brief comput_harris response
/// @return vdb_t
image_t compute_harris(const derivatives_t& d //, size_t cx, size_t cy
	, float k) {

	image_t M(d.Ix.cx, d.Iy.cy, 0.0);

	for (int r = 0; r != d.Iy.cy; ++r) {
		for (int c = 0; c != d.Ix.cx; ++c) {
			double   a11, a12,
				a21, a22;

			a11 = d.Ix.img[(r * d.Ix.cx + c)] * d.Ix.img[(r * d.Ix.cx + c)];
			a22 = d.Iy.img[(r * d.Iy.cx + c)] * d.Iy.img[(r * d.Iy.cx + c)];
			a21 = d.Ix.img[(r * d.Ix.cx + c)] * d.Iy.img[(r * d.Iy.cx + c)];
			a12 = d.Ix.img[(r * d.Ix.cx + c)] * d.Iy.img[(r * d.Iy.cx + c)];

			double det = a11 * a22 - a12 * a21;
			double trace = a11 + a22;

			M.img[(r * M.cx + c)] = std::abs(det - k * trace * trace);
		}
	}

	//vImg tm(M.img.size(), 0);
	//for (int i = 0; i != M.img.size();++i) {
	//	tm[i] = M.img[i] != 0 ? 255 : 0;
	//}
	//saveBMP("harris1.bmp", tm.data(), M.cx, M.cy, 1);

	return M;
}

/// @biref make_harris
/// @param k = 0 < k < 0.25
/// @return ?
image_t make_harris(const vImg& src, size_t cx, size_t cy
				, float k, int fr, bool gauss) {
	if (src.size() != cx * cy) return image_t();

	// 1. compute_derivatives
	derivatives_t deriv;
	compute_derivatives(src, cx, cy, deriv);

	// 2. Median Filtering
	derivatives_t mderiv = deriv;
	if (gauss) {
		mderiv.Ix = gauss_filter(deriv.Ix.img, deriv.Ix.cx, deriv.Ix.cy, fr);
		mderiv.Iy = gauss_filter(deriv.Iy.img, deriv.Iy.cx, deriv.Iy.cy, fr);
		mderiv.Ixy = gauss_filter(deriv.Ixy.img, deriv.Ixy.cx, deriv.Ixy.cy, fr);
	}
	else
	{
		mderiv.Ix = mean_filter(deriv.Ix.img, deriv.Ix.cx, deriv.Ix.cy, fr);
		mderiv.Iy = mean_filter(deriv.Iy.img, deriv.Iy.cx, deriv.Iy.cy, fr);
		mderiv.Ixy = mean_filter(deriv.Ixy.img, deriv.Ixy.cx, deriv.Ixy.cy, fr);
	}

	// 3. Compute Harris Responses
	//image_t M = compute_harris(mderiv, k);
	return compute_harris(mderiv, k);
}

vPtd maxima_suppresss(const image_t &harris, vPtd* kps, bool haveKp
					,float percentage, int fr, int suppressionRadius) {
	// Declare a max suppression matrix
	img_t maxima(harris.cx, harris.cy, 0);

	// Create a vector of all Points
	vPtd points;
	if (haveKp) {
		points = *kps;
	}

	for (int r = 0; r != harris.cy; ++r) {
		for (int c = 0; c != harris.cx; ++c) {
			PointData pd;

			pd.v = harris.img[r * harris.cx + c];
			pd.x = c;
			pd.y = r;

			if (pd.v != 0) {
				//if (haveKp) 
					points.push_back(pd);
				if (r* maxima.cx + c < maxima.img.size())
					maxima.img[r * maxima.cx + c] = 1;
			}
		}
	}
	
	// Sort points by corner Response
	std::sort(points.begin(), points.end(), [](auto &a, auto &b) {
		return a.v > b.v;
	});

	// Get top points, given by the percentage
	int numberTopPoints = harris.cx * harris.cy * percentage;
	vPtd topPoints;

	int i = 0;
	while (i != points.size()){ //(topPoints.size() < numberTopPoints) {
		//if (i == points.size())
		//	break;

		int supRows = maxima.cy;
		int supCols = maxima.cx;

		vPtd ev;
		// Check if point marked in maximaSuppression matrix
		if (maxima.img[(points[i].y * maxima.cx + points[i].x)] == 1) {
			for (int r = -suppressionRadius; r <= suppressionRadius; ++r) {
				for (int c = -suppressionRadius; c <= suppressionRadius; ++c) {
					int sx = points[i].x + c;
					int sy = points[i].y + r;

					// bound checking
					if (sy >= supRows)
						sy = supRows-1;
					if (sy < 0)
						sy = 0;
					if (sx >= supCols)
						sx = supCols-1;
					if (sx < 0)
						sx = 0;

					//maxima.img[((points[i].x + c) * maxima.cx) + (points[i].y + r)] = 1;
					//if ((sy)*maxima.cx + (sx) < maxima.img.size())
					maxima.img[(sy)*maxima.cx + (sx)] = 0;
					ev.push_back(points[i]);
				}
			}

			// Convert back to original image coordinate system 
			//points[i].x += 1 + fr; 
			//points[i].y += 1 + fr;
			//points[i].x = points[i].x >= harris.cx ? harris.cx-1 : points[i].x;
			//points[i].y = points[i].y >= harris.cy ? harris.cy-1 : points[i].y;
			//topPoints.push_back(points[i]);

			auto p = std::max_element(std::begin(ev), std::end(ev),
				[](auto& l, auto& r) { return l.v > r.v; });
			auto v = std::distance(ev.begin(), p);
			topPoints.push_back(*p);
		}

		++i;
	}

	vImg tm(harris.img.size(), 0);
	for (auto const& pd : topPoints) {
		tm[pd.y * harris.cx + pd.x] = 255;
	}
	//vImg tm(maxima.img.size(), 0);
	//for (int i = 0; i != maxima.img.size();++i) {
	//	tm[i] = maxima.img[i] != 0 ? 255 : 0;
	//}
	saveBMP("harris2.bmp", tm.data(), maxima.cx, maxima.cy, 1);

	return topPoints;
}

descriptor_t ptd_to_brief(const vImg& src, size_t cx, size_t cy
						, const vPtd &kps, int fr) {

	//int n_features = kps.size(); // 1 << 16;
	//vPt corners(n_features);
	//std::vector<float> angles(n_features);
	//BRIEF::create_synthetic_data(corners, BRIEF::g_angles.data(), 2560, cx, cy);

	descriptor_t d = BRIEF::compute(src, cy, cx, 1, 1, kps, BRIEF::g_angles);

	vImg kimg(src.size()* 16, 0);
	for (auto const &pd : kps) {
		kimg[pd.y * cx + pd.x] = 255;
	}
	saveBMP("harris.bmp", kimg.data(), cx, cy, 1);

	return d;
}

template<class T, class I1, class I2>
T hamming_distance(size_t size, I1 b1, I2 b2) {
	return std::inner_product(b1, b1 + size, b2, T{},
		std::plus<T>(), std::not_equal_to<T>());
}

// Function to calculate hamming distance 
int hammingDistance(int n1, int n2)
{
	int x = n1 ^ n2;
	int setBits = 0;

	while (x > 0) {
		setBits += x & 1;
		x >>= 1;
	}

	return setBits;
}

/// @brief hamming distans by brute force
double get_hd(const descriptor_t &d1, const descriptor_t &d2) {
	int d1l = d1.size(), d2l = d2.size();
	int s = std::abs(d1l - d2l);

	// brute-force hamming distance
	std::vector<double> bf;	bf.reserve(s+1);
	descriptor_t const & dsrc = (d1l > d2l ? d1 : d2);
	descriptor_t const & dtar = (d1l > d2l ? d2 : d1);
	
	int i = 0;
	do {
		double hd = 0.0;
		for (size_t pos = 0; pos != dtar.size()-1; ++pos) {
			int h1 = dsrc[i + pos], h2 = dtar[pos];
			hd += hammingDistance(h1, h2);
		}	
		bf.push_back(hd);
	} while (++i < s);

#if 1
	// return only min value
	if (s == 0)
		return bf[0];
	auto min = std::min_element(std::begin(bf), std::end(bf));
	return std::distance(std::begin(bf), min);
#else
	// return sum;
	std::vector<double> sum(bf.size(), 0.0); 
	std::partial_sum(std::begin(bf), std::end(bf), std::begin(sum));
	return sum.back();
#endif
}

std::vector<std::pair<int, double>>
min_hd(const std::vector<descriptor_t>& sigs, const descriptor_t& sig) {

	std::unordered_map<int, double> dists;

	for (int i = 0; i != sigs.size(); ++i) {
		dists[i] = get_hd(sigs[i], sig);
	}

	std::vector<std::pair<int, double>> top_3(10);
	std::partial_sort_copy(dists.begin(),
		dists.end(),
		top_3.begin(),
		top_3.end(),
		[](std::pair<int, double> const& l,
			std::pair<int, double> const& r)
	{
		return l.second < r.second;
	});

	return top_3;
}

double get_ud(const int_vec& d1, const int_vec& d2) {
	int d1l = d1.size(), d2l = d2.size();
	int s = std::abs(d1l - d2l);

	// brute-force euclidean distance
	std::vector<double> bf;
	int_vec const& dsrc = (d1l > d2l ? d1 : d2);
	int_vec const& dtar = (d1l > d2l ? d2 : d1);

	int i = 0;
	//double ud = 0;
	do {
		double ud = 0;
	//	for (size_t pos = 0; pos != s; ++pos) {
			ud += vectorDistance(dtar.begin(), dtar.end(), dsrc.begin()+i);
			bf.push_back(ud);
	//	}
	} while (++i < s);

	if (s == 0)
		return bf[0];
	auto min = std::min_element(std::begin(bf), std::end(bf));
	return std::distance(std::begin(bf), min);
}

std::vector<std::pair<int, double>>
min_dist_5(const sigi_vec& sigs, const int_vec& sig) {
	std::unordered_map<int, double> dists;

	for (int i = 0; i != sigs.size(); ++i) {
		dists[i] = get_ud(sig, sigs[i]);
	}

	std::vector<std::pair<int, double>> top_3(5);
	std::partial_sort_copy(dists.begin(),
		dists.end(),
		top_3.begin(),
		top_3.end(),
		[](std::pair<int, double> const& l,
			std::pair<int, double> const& r)
	{
		return l.second < r.second;
	});

	return top_3;
}
