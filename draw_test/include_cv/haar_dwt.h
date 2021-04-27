/// @file haar_dwt.h
/// @brief haar dwt functions
/// @author locketk9@gmail.com, 2019-05-27

#pragma once

#include <vector>
#include <algorithm>


using byte = unsigned char;
using ch_vec = std::vector<byte>;
using fl_vec = std::vector<float>;

// Filter type
#define NONE 0  // no filter
#define HARD 1  // hard shrinkage
#define SOFT 2  // soft shrinkage
#define GARROT 3  // garrot filter
//--------------------------------
// signum
//--------------------------------
float sgn(float x)
{
	float res = 0;
	if (x == 0)
	{
		res = 0;
	}
	if (x>0)
	{
		res = 1;
	}
	if (x<0)
	{
		res = -1;
	}
	return res;
}
//--------------------------------
// Soft shrinkage
//--------------------------------
float soft_shrink(float d, float T)
{
	float res;
	if (std::fabs(d)>T)
	{
		res = sgn(d)*(std::fabs(d) - T);
	}
	else
	{
		res = 0;
	}

	return res;
}
//--------------------------------
// Hard shrinkage
//--------------------------------
float hard_shrink(float d, float T)
{
	float res;
	if (std::fabs(d)>T)
	{
		res = d;
	}
	else
	{
		res = 0;
	}

	return res;
}
//--------------------------------
// Garrot shrinkage
//--------------------------------
float Garrot_shrink(float d, float T)
{
	float res;
	if (std::fabs(d)>T)
	{
		res = d - ((T*T) / d);
	}
	else
	{
		res = 0;
	}

	return res;
}

static fl_vec& haar_dwt(fl_vec &src, int w, int h, fl_vec &dst, int NIter)	{
	float c, dh, dv, dd;
	int width = w;
	int height = h;
	for (int k = 0; k<NIter; k++)
	{
		int wk = width >> (k + 1);
		int hk = height >> (k + 1);

		for (int y = 0; y<(height >> (k + 1)); y++)
		{
			for (int x = 0; x<(width >> (k + 1)); x++)
			{
				c = (src[(2 * (y*w) + 2 * x)] + src[(2 * (y*w) + 2 * x + 1)] + src[(2 * (y*w) + 1 * w + 2 * x)] + src[(2 * (y*w) + 1 * w + 2 * x + 1)])*0.5;
				dst[((y*w) + x)] = c;

				dh = (src[(2 * (y*w) + 2 * x)] + src[(2 * (y*w) + 1 * w + 2 * x)] - src[(2 * (y*w) + 2 * x + 1)] - src[(2 * (y*w) + 1 * w + 2 * x + 1)])*0.5;
				dst[((y*w) + x + (wk))] = dh;

				dv = (src[(2 * (y*w) + 2 * x)] + src[(2 * (y*w) + 2 * x + 1)] - src[(2 * (y*w) + 1 * w + 2 * x)] - src[(2 * (y*w) + 1 * w + 2 * x + 1)])*0.5;
				dst[((y*w) + (hk)*w + x)] = dv;

				dd = (src[(2 * (y*w) + 2 * x)] - src[(2 * (y*w) + 2 * x + 1)] - src[(2 * (y*w) + 1 * w + 2 * x)] + src[(2 * (y*w) + 1 * w + 2 * x + 1)])*0.5;
				dst[((y*w) + (hk)*w + x + (wk))] = dd;
			}
		}
		src = dst;
	}

	return dst;
}

static fl_vec& inv_haar_dwt(fl_vec &src, int w, int h, fl_vec &dst, int NIter
	, int SHRINKAGE_TYPE = 0, float SHRINKAGE_T = 50)	{

	float c, dh, dv, dd;
	int width = w;
	int height = h;
	//--------------------------------
	// NIter - number of iterations 
	//--------------------------------
	for (int k = NIter; k>0; k--)
	{
		int wk = width >> (k + 1);
		int hk = height >> (k + 1);

		for (int y = 0; y<(height >> k); y++)
		{
			for (int x = 0; x<(width >> k); x++)
			{
				c = src[((y*w) + x)];
				dh = src[((y*w) + x + (width >> k))];
				dv = src[((y*w) + (height >> k)*w + x)];
				dd = src[((y*w) + (height >> k)*w + x + (width >> k))];

				// (shrinkage)
				switch (SHRINKAGE_TYPE)
				{
				case HARD:
					dh = hard_shrink(dh, SHRINKAGE_T);
					dv = hard_shrink(dv, SHRINKAGE_T);
					dd = hard_shrink(dd, SHRINKAGE_T);
					break;
				case SOFT:
					dh = soft_shrink(dh, SHRINKAGE_T);
					dv = soft_shrink(dv, SHRINKAGE_T);
					dd = soft_shrink(dd, SHRINKAGE_T);
					break;
				case GARROT:
					dh = Garrot_shrink(dh, SHRINKAGE_T);
					dv = Garrot_shrink(dv, SHRINKAGE_T);
					dd = Garrot_shrink(dd, SHRINKAGE_T);
					break;
				}

				//-------------------
				dst[((y*w) * 2 + x * 2)] = 0.5*(c + dh + dv + dd);
				dst[((y*w) * 2 + x * 2 + 1)] = 0.5*(c - dh + dv - dd);
				dst[((y*w) * 2 + 1*w + x * 2)] = 0.5*(c + dh - dv - dd);
				dst[((y*w) * 2 + 1*w + x * 2 + 1)] = 0.5*(c - dh - dv + dd);
			}
		}
		//Mat C = src(Rect(0, 0, width >> (k - 1), height >> (k - 1)));
		//Mat D = dst(Rect(0, 0, width >> (k - 1), height >> (k - 1)));
		//D.copyTo(C);
		src = dst;
	}

	return dst;
}
