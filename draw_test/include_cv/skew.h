/// @file   skew.h
/// @author locketk9@gmail.net

#pragma once

//#define _USE_MATH_DEFINES
#include <cmath>
#include "ocr_def.h"


/// @brief affine_skew rotate
ch_vec& affine_skew(const ch_vec& src, imgRECT& rect, ch_vec& dst
	, double x_rot, double y_rot
	, double x_expan = 1, double y_expan = 1
	, int x_move = 0, int y_move = 0)
{
#if 0	//  순방향.
	/* output image generation by affine transformation
	and bilinear transformation  */

	double affine[2][2], beta[2];

	/* forward affine transformation */
	affine[0][0] = x_expan * cos(x_rot * PI / 180.0);
	affine[0][1] = y_expan * sin(y_rot * PI / 180.0);
	affine[1][0] = x_expan * sin(x_rot * PI / 180.0);
	affine[1][1] = y_expan * cos(y_rot * PI / 180.0);
	beta[0] = x_move;
	beta[1] = y_move;

	double x_new, y_new, x_frac, y_frac;
	double gray_new;
	int gv_new;
	int x, y, m, n;
	int pos;
	int x_size2 = rect.x + rect.cx;
	int y_size2 = rect.y + rect.cy;
	for (int y = rect.y; y < y_size2; ++y)
	{
		for (int x = rect.x; x < x_size2; ++x)
		{
			pos = y * rect.cx + x;

			x_new = beta[0] + affine[0][0] * (x - x_size2 / 2.0)
				+ affine[0][1] * (y - y_size2 / 2.0) + x_size2 / 2.0;
			y_new = beta[1] + affine[1][0] * (x - x_size2 / 2.0)
				+ affine[1][1] * (y - y_size2 / 2.0) + y_size2 / 2.0;
			m = (int)floor(x_new);
			n = (int)floor(y_new);
			x_frac = x_new - m;
			y_frac = y_new - n;

			if (m >= 0 && m + 1 < x_size2 && n >= 0 && n + 1 < y_size2)
			{
				gray_new = (1.0 - y_frac) * ((1.0 - x_frac) * src[n * rect.cx + m] +
					x_frac * src[n * rect.cx + m + 1]) +
					y_frac * ((1.0 - x_frac) * src[(n + 1) * rect.cx + m] +
						x_frac * src[(n + 1) * rect.cx + m + 1]);
				dst[pos] = (unsigned char)gray_new;
			}
			else if (m + 1 == x_size2 && n >= 0 && n < y_size2
				|| n + 1 == y_size2 && m >= 0 && m < x_size2)
			{
				dst[pos] = src[n * rect.cx + m];
			}
			else
			{
				dst[pos] = 0;
			}
		}
	}
#endif
#if 1	// 역방향
	/* output image generation by inverse affine transformation
	and bilinear transformation  */

	double affine[2][2], beta[2];

	/* forward affine transformation */
	affine[0][0] = x_expan * cos(x_rot * PI / 180.0);
	affine[0][1] = y_expan * sin(y_rot * PI / 180.0);
	affine[1][0] = x_expan * sin(x_rot * PI / 180.0);
	affine[1][1] = y_expan * cos(y_rot * PI / 180.0);
	beta[0] = x_move;
	beta[1] = y_move;

	/* determination of inverse affine transformation */
	double det, i_affine[2][2], i_beta[2];

	det = affine[0][0] * affine[1][1] - affine[0][1] * affine[1][0];
	if (det == 0.0) {
		i_affine[0][0] = 1.0;
		i_affine[0][1] = 0.0;
		i_affine[1][0] = 0.0;
		i_affine[1][1] = 1.0;
		i_beta[0] = -beta[0];
		i_beta[1] = -beta[1];
	}
	else {
		i_affine[0][0] = affine[1][1] / det;
		i_affine[0][1] = -affine[0][1] / det;
		i_affine[1][0] = -affine[1][0] / det;
		i_affine[1][1] = affine[0][0] / det;
		i_beta[0] = -i_affine[0][0] * beta[0] - i_affine[0][1] * beta[1];
		i_beta[1] = -i_affine[1][0] * beta[0] - i_affine[1][1] * beta[1];
	}

	double x_new, y_new, x_frac, y_frac;
	double gray_new;
	int gv_new;
	int x, y, m, n;
	int pos;
	int x_size2 = rect.x + rect.cx;
	int y_size2 = rect.y + rect.cy;
	for (int y = rect.y; y < y_size2; ++y) {
		for (int x = rect.x; x < x_size2; ++x) {
			pos = y * rect.cx + x;

			x_new = i_beta[0] + i_affine[0][0] * (x - x_size2 / 2.0)
				+ i_affine[0][1] * (y - y_size2 / 2.0) + x_size2 / 2.0;
			y_new = i_beta[1] + i_affine[1][0] * (x - x_size2 / 2.0)
				+ i_affine[1][1] * (y - y_size2 / 2.0) + y_size2 / 2.0;
			m = (int)floor(x_new);
			n = (int)floor(y_new);
			x_frac = x_new - m;
			y_frac = y_new - n;

			if (m >= 0 && m + 1 < x_size2 && n >= 0
				&& n + 1 < y_size2) {
				gray_new = (1.0 - y_frac) * ((1.0 - x_frac) * src[n * rect.cx + m] +
					x_frac * src[n * rect.cx + m + 1]) +
					y_frac * ((1.0 - x_frac) * src[(n + 1) * rect.cx + m] +
						x_frac * src[(n + 1) * rect.cx + m + 1]);
				dst[pos] = (unsigned char)gray_new;
			}
			else if (m + 1 == x_size2 && n >= 0 && n < y_size2
				|| n + 1 == y_size2 && m >= 0 && m < x_size2) {
				dst[pos] = src[n * rect.cx + m];
			}
			else {
				dst[pos] = 255; // fill 0 -> 255
			}
		}
	}
#endif	
	return (dst);
}



/// @brief  detect_skew
/// @param  bin = binary image
/// @param  cx  = bin's width
/// @param  cy  = bin's height
/// @param  d   = threshold of gradient 
double detect_skew(const ch_vec& bin, int cx, int cy, float d=0.1)  {
    if (bin.size() == 0 || cx < 0 || cy < 0) return 0.0;

    // init point
    Point l(cx,cy),t(cx,cy),r(0,0),b(0,0);
    // get left, top, right, bottom
    int pos = 0;
    for (int y=0; y!=cy; ++y)   {
        for (int x=0; x!=cx; ++x)   {
            pos = (y*cx)+x;
            if (bin[pos]<128)  {
                if (l.x > x) { l.x=x; l.y=y; }
                if (t.y > y) { t.x=x; t.y=y; }
                if (r.x < x) { r.x=x; r.y=y; }
                if (b.y < y) { b.x=x; b.y=y; }
            }
        }
    }

    // calc gradient left-top, top-right, right-bottom, left-bottom line
    float lt = (t.x == l.x) ? d : ((float)(l.y - t.y)) / ((float)(t.x - l.x));
    float br = (r.x == b.x) ? d : ((float)(b.y - r.y)) / ((float)(r.x - b.x));
    float tr = (r.x == t.x) ? d : ((float)(r.y - t.y)) / ((float)(r.x - t.x));
    float lb = (b.x == l.x) ? d : ((float)(b.y - l.y)) / ((float)(b.x - l.x));

    // compare gradient value and get skew radian
    // 1 g = 0.015707963267949 rad - right ?
    // slope degree is arctan(g)
    float line1 = lt > br ? lt : br;
    float line2 = lb > tr ? lb : tr;
    float d1 = std::fabs(lt - br), d2 = std::fabs(lb - tr);
    return d1 > d ? (d2 > d ? 0.0 : std::atan(line2) * (180 / PI)) : std::atan(line1) * (180 / PI) ;
}


/// @brief  detect_skew2
/// @param  bin = binary image
/// @param  cx  = bin's width
/// @param  cy  = bin's height
double detect_skew2(const ch_vec& bin, int cx, int cy) {
    if (bin.size() == 0 || cx < 0 || cy < 0) return 0.0;

    // init point
    Point l(cx, cy), t(cx, cy), r(0, 0), b(0, 0);
    // get left, top, right, bottom
    int pos = 0;
    for (int y = 0; y != cy; ++y) {
        for (int x = 0; x != cx; ++x) {
            pos = (y * cx) + x;
            if (bin[pos] < 128) {
                if (l.x > x) { l.x = x; l.y = y; }
                if (t.y > y) { t.x = x; t.y = y; }
                if (r.x < x) { r.x = x; r.y = y; }
                if (b.y < y) { b.x = x; b.y = y; }
            }
        }
    }

    auto in_angle = [](const Point& p1, const Point& p2,
        const Point& p3, const Point& p4) {
        // cos(angle) = (ac + bd) / ( |v| * |w| )
        double a1 = (p1.x - p2.x);
        double b1 = (p1.y - p2.y);
        double c1 = (p3.x - p4.x);
        double d1 = (p3.y - p4.y);
        //
        double mag_v1 = sqrt(a1 * a1 + b1 * b1);
        double mag_v2 = sqrt(c1 * c1 + d1 * d1);
        //
        double cos_angle = (a1 * c1 + b1 * d1) / (mag_v1 * mag_v2);
        double angle = acos(cos_angle);
        angle = angle * 180.0 / PI; // convert to degrees ???

        return angle;
    };

    Point h1(l.x+1, 0), h2(l.x+1, cy);
    double theta1 = in_angle(l, r, h1, h2);
    Point h3(t.x+1, 0), h4(t.y+1, cy);
    double theta2 = in_angle(t, b, h3, h4);

    double skew = min(fabs(theta1), fabs(theta2));
	// find angle direction
	if (t.x < b.x) {
		skew = -skew;
	}
	else if (l.y < r.y) {
		skew = -skew;
	}

    return skew;
}
