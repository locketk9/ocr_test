/// @file	bmp_func.h
/// @brief	bmp relative functions
/// @author	locketk9@gmail.com

#ifndef _BMP_FUNC_H
#define _BMP_FUNC_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <deque>

typedef unsigned char byte;
typedef std::vector<byte>	ch_vec;
typedef std::vector<double>	db_vec;

using uchar = unsigned char;
using int_vec = std::vector<int>;
using sh_vec = std::vector<short>;
using fl_vec = std::vector<float>;

////typedef std::pair<double, double> Point;
//typedef struct _POINT {
//	double	x;
//	double	y;
//	_POINT(double _x, double _y) {
//		x = _x; y = _y;
//	}
//	_POINT() = default;
//	_POINT(_POINT&) = default;
//	_POINT& operator=(_POINT&) = default;
//} Point;

//using point_vec = std::vector<Point>;

typedef std::unordered_map<char, db_vec> sig_map;
typedef std::unordered_map<char, ch_vec> chain_map;

//using namespace std;

// define
#define PI	3.141592654

// resampling relative
#define BOX_WIDTH		8
#define BOX_HEIGHT		12
#define BOX_PIXEL		4
#define BOX_SIZE		BOX_WIDTH * BOX_HEIGHT * BOX_PIXEL * BOX_PIXEL

#if 0
// operator
class imgRECT;
bool operator == (const imgRECT &lhs, const imgRECT &rhs);
bool operator < (const imgRECT &lhs, const imgRECT &rhs);

/// @class imgRECT
class imgRECT
{
public:
	int	x, y, cx, cy;
	imgRECT(int _x = 0, int _y = 0, int _cx = 0, int _cy = 0)
	{
		x = _x; y = _y; cx = _cx; cy = _cy;
	}
	imgRECT(const imgRECT &rhs)
	{
		x = rhs.x; y = rhs.y; cx = rhs.cx; cy = rhs.cy;
	}
	const imgRECT& operator = (const imgRECT &rhs)
	{
		if (*this == rhs) return rhs;
		x = rhs.x; y = rhs.y; cx = rhs.cx; cy = rhs.cy;
		return (*this);
	}

};

namespace std {
	template <> struct hash<imgRECT>
	{
		size_t operator()(const imgRECT & x) const
		{
			return  hash<int>()(x.x * 1000 + x.y * 100 + x.cx * 10 + x.cy);
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
#endif


/// @brief load bmp file
/// @param const char *
/// @param ch_vec &
/// @param int &
/// @param int &
int loadBMP(const char *filename, ch_vec &bmp, int &width, int &height);

/// @brief load bmp file - func OK
///int loadBMP(const char *filename, ch_vec &bmp, int &width, int &height);
int saveBMP(const char *filename, const byte *img, int width
			, int height, int bp=3) ;

/// @brief bmp 2 raw
byte* bmp2raw(const byte *bmp, byte *conv, int imgWidth, int imgHeight
	, int bp)  ;


#endif // !_BMP_FUNC_H