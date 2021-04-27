/// @file templateMatch.h
/// @brief forting opencv function

#pragma once

#include <algorithm>
#include <vector>

#include "ocr_def.h"


template<typename _Tp> 
class Scalar_
{
public:
	_Tp v0_, v1_, v2_, v3_;

public:
	//! various constructors
	Scalar_() {}
	Scalar_(_Tp v0, _Tp v1, _Tp v2 = 0, _Tp v3 = 0) {
		v0_ = v0; v1_ = v1; v2_ = v2; v3_ = v3;
	}
	Scalar_(const CvScalar& s) = default;
	Scalar_(_Tp v0) { v0_ = v1_ = v2_ = v3_ = v0; }

	//! returns a scalar with all elements set to v0
	static Scalar_<_Tp> all(_Tp v0) { return Scalar_(v0); }
	//! conversion to the old-style CvScalar
	//operator CvScalar() const;

	//! conversion to another data type
	template<typename T2> operator Scalar_<T2>() const {
		return Scalar_<T2>(v0_, v1_, v2_, v3_);
	}

	//! per-element product
	Scalar_<_Tp> mul(const Scalar_<_Tp>& t, double scale = 1) const {
		Scalar_<_Tp> rs(v0_, v1_, v2_, v3_);
		rs.v0_ = rs.v0_ * t.v0_ * scale;
		rs.v1_ = rs.v1_ * t.v1_ * scale;
		rs.v2_ = rs.v2_ * t.v2_ * scale;
		rs.v3_ = rs.v3_ * t.v3_ * scale;
		return rs;
	}

	// returns (v0, -v1, -v2, -v3)
	Scalar_<_Tp> conj() const {
		Scalar_<_Tp> rs(v0_, v1_, v2_, v3_);
		rs.v0_ = rs.v0_ ;
		rs.v1_ = -rs.v1_ ;
		rs.v2_ = -rs.v2_ ;
		rs.v3_ = -rs.v3_ ;
		return rs;
	}

	// returns true iff v1 == v2 == v3 == 0
	//? bool isReal() const;
};

typedef Scalar_<double> Scalar;


/// @brief opencv function - matchTemplate
static void common_matchTemplate(db_vec& img, Mat& templ, Mat& result, int method, int cn)
{
	if (method == CV_TM_CCORR)
		return;

	int numType = method == CV_TM_CCORR || method == CV_TM_CCORR_NORMED ? 0 :
		method == CV_TM_CCOEFF || method == CV_TM_CCOEFF_NORMED ? 1 : 2;
	bool isNormed = method == CV_TM_CCORR_NORMED ||
		method == CV_TM_SQDIFF_NORMED ||
		method == CV_TM_CCOEFF_NORMED;

	double invArea = 1. / ((double)templ.rows * templ.cols);

	Mat sum, sqsum;
	Scalar templMean, templSdv;
	double *q0 = 0, *q1 = 0, *q2 = 0, *q3 = 0;
	double templNorm = 0, templSum2 = 0;

	if (method == CV_TM_CCOEFF)
	{
		integral(img, sum, CV_64F);
		templMean = mean(templ);
	}
	else
	{
		integral(img, sum, sqsum, CV_64F);
		meanStdDev(templ, templMean, templSdv);

		templNorm = templSdv[0] * templSdv[0] + templSdv[1] * templSdv[1] + templSdv[2] * templSdv[2] + templSdv[3] * templSdv[3];

		if (templNorm < DBL_EPSILON && method == CV_TM_CCOEFF_NORMED)
		{
			result = Scalar::all(1);
			return;
		}

		templSum2 = templNorm + templMean[0] * templMean[0] + templMean[1] * templMean[1] + templMean[2] * templMean[2] + templMean[3] * templMean[3];

		if (numType != 1)
		{
			templMean = Scalar::all(0);
			templNorm = templSum2;
		}

		templSum2 /= invArea;
		templNorm = std::sqrt(templNorm);
		templNorm /= std::sqrt(invArea); // care of accuracy here

		CV_Assert(sqsum.data != NULL);
		q0 = (double*)sqsum.data;
		q1 = q0 + templ.cols*cn;
		q2 = (double*)(sqsum.data + templ.rows*sqsum.step);
		q3 = q2 + templ.cols*cn;
	}

	CV_Assert(sum.data != NULL);
	double* p0 = (double*)sum.data;
	double* p1 = p0 + templ.cols*cn;
	double* p2 = (double*)(sum.data + templ.rows*sum.step);
	double* p3 = p2 + templ.cols*cn;

	int sumstep = sum.data ? (int)(sum.step / sizeof(double)) : 0;
	int sqstep = sqsum.data ? (int)(sqsum.step / sizeof(double)) : 0;

	int i, j, k;

	for (i = 0; i < result.rows; i++)
	{
		float* rrow = result.ptr<float>(i);
		int idx = i * sumstep;
		int idx2 = i * sqstep;

		for (j = 0; j < result.cols; j++, idx += cn, idx2 += cn)
		{
			double num = rrow[j], t;
			double wndMean2 = 0, wndSum2 = 0;

			if (numType == 1)
			{
				for (k = 0; k < cn; k++)
				{
					t = p0[idx + k] - p1[idx + k] - p2[idx + k] + p3[idx + k];
					wndMean2 += t * t;
					num -= t * templMean[k];
				}

				wndMean2 *= invArea;
			}

			if (isNormed || numType == 2)
			{
				for (k = 0; k < cn; k++)
				{
					t = q0[idx2 + k] - q1[idx2 + k] - q2[idx2 + k] + q3[idx2 + k];
					wndSum2 += t;
				}

				if (numType == 2)
				{
					num = wndSum2 - 2 * num + templSum2;
					num = MAX(num, 0.);
				}
			}

			if (isNormed)
			{
				double diff2 = MAX(wndSum2 - wndMean2, 0);
				if (diff2 <= std::min(0.5, 10 * FLT_EPSILON * wndSum2))
					t = 0; // avoid rounding errors
				else
					t = std::sqrt(diff2)*templNorm;

				if (fabs(num) < t)
					num /= t;
				else if (fabs(num) < t*1.125)
					num = num > 0 ? 1 : -1;
				else
					num = method != CV_TM_SQDIFF_NORMED ? 0 : 1;
			}

			rrow[j] = (float)num;
		}
	}
}