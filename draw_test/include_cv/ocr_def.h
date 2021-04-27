/// @file	ocr_def.h
/// @brief	ocr relative define
/// @author	locketk9@gmail.com

#pragma once

//#ifndef _OCR_DEF_H
//#define _OCR_DEF_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <deque>
#include <map>

typedef unsigned char byte;
typedef std::vector<byte>	ch_vec;
typedef std::vector<double>	db_vec;

using uchar = unsigned char;
using int_vec = std::vector<int>;
using sh_vec = std::vector<short>;
using fl_vec = std::vector<float>;




#if 1
//typedef std::pair<double, double> Point;
typedef struct _POINT {
	double	x;
	double	y;
	_POINT(double _x, double _y) {
		x = _x; y = _y;
	}
	_POINT() = default;
	_POINT(const _POINT&) = default;
	_POINT& operator=(const _POINT&) = default;
} Point ;

typedef
struct _PointData : public Point {
	double v{ 0.0 };	// intencity
	double a{ 0.0 };	// angle
	int o{ 0 };			// octabe
	size_t s{ 0 };		// patch size
	size_t r{ 0 };		// response
	_PointData(double _v, double _x, double _y, double _a=0.0, int _o=0, size_t _s=0, size_t _r=0) {
		v = _v; x = _x; y = _y; a = _a; o = _o; s = _s; r = _r;
	}
	_PointData() = default;
	_PointData(const _PointData&) = default;
	_PointData& operator = (_PointData&) = default;
} PointData;

using point_vec = std::vector<Point>;
#endif

typedef std::unordered_map<char, db_vec> sig_map;
typedef std::unordered_map<char, ch_vec> chain_map;

typedef std::vector<fl_vec> sigf_vec;
typedef std::vector<ch_vec> sig_vec;

using sigi_vec = std::vector<int_vec>;


// defines
using byte = unsigned char;
using vImg = std::vector<byte>;
using vHist = std::vector<int>;
using size_t = std::size_t;
using vPt = std::vector<Point>;
using vPtd = std::vector<PointData>;

using descriptor_t = vImg;



//using namespace std;

// define
#define PI	3.141592654

// resampling relative
#define BOX_WIDTH		8
#define BOX_HEIGHT		12
#define BOX_PIXEL		4
#define BOX_SIZE		BOX_WIDTH * BOX_HEIGHT * BOX_PIXEL * BOX_PIXEL

// operator
class imgRECT;
bool operator == (const imgRECT &lhs, const imgRECT &rhs);
bool operator < (const imgRECT &lhs, const imgRECT &rhs);

/// @class imgRECT
class imgRECT
{
public:
	int	x, y, cx, cy;
	imgRECT(int _x=0, int _y=0, int _cx=0, int _cy=0)
	{
		x=_x; y=_y; cx=_cx; cy=_cy;
	}
	imgRECT(const imgRECT &rhs)
	{		
		x=rhs.x; y=rhs.y; cx=rhs.cx; cy=rhs.cy;
	}
	const imgRECT& operator = (const imgRECT &rhs)
	{
		if (*this == rhs) return rhs;
		x=rhs.x; y=rhs.y; cx=rhs.cx; cy=rhs.cy;
		return (*this);
	}

};

namespace std {
  template <> struct hash<imgRECT>
  {
    size_t operator()(const imgRECT & x) const
    {
      return  hash<int>()(x.x*1000+x.y*100+x.cx*10+x.cy);
    }
  };
}

typedef std::unordered_set<imgRECT> rt_set;
//typedef std::set<imgRECT> rt_set;
//typedef std::deque<imgRECT> rt_deq;
using rt_vec = std::vector<imgRECT>;


//namespace std {
//  template <> struct hash<db_vec>
//  {
//    size_t operator()(const db_vec & x) const
//    {
//      return  hash<double>()(x[0]);
//    }
//  };
//}

typedef std::unordered_set<db_vec> sig_set;


struct _LINE_POLAR{
	double	rho{ 0.0 };
	double	theta{ 0.0 };

	_LINE_POLAR() = default;
	_LINE_POLAR(_LINE_POLAR&) = default;
	_LINE_POLAR& operator=(_LINE_POLAR&) = default;
};
using LinePolar = struct _LINE_POLAR;
using line_vec = std::vector<LinePolar>;

struct _CIRCLE	{
	double x;
	double y;
	double r;

	_CIRCLE() = default;
	_CIRCLE(_CIRCLE&) = default;
	_CIRCLE& operator=(_CIRCLE&) = default;
};
using Circle = struct _CIRCLE;
using circle_vec = std::vector<Circle>;



// defines
using byte = unsigned char;
using vImg = std::vector<byte>;
using vHist = std::vector<int>;
using size_t = std::size_t;
using vPt = std::vector<Point>;

using descriptor_t = vImg;

using vdb_t = std::vector<double>;

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
	image(std::vector<T>& _img, size_t _cx, size_t _cy) {
		img = _img; cx = _cx; cy = _cy;
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




//#endif	// _OCR_DEF_H