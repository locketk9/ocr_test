/// @file sift.h
/// @author locketk9@gmail.com, 2020-04-30
/// @desc set of function SIFT(Scale-invariant feature transform)
///		  ref : opencv

#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <cstdlib>

//#include "./ocr_def.h"
//#include "./ocr_func.h"
#include "./ocr.h"


// defines
using vimg_t = std::vector<img_t>;
using vimage_t = std::vector<image_t>;

// sift
namespace SIFT {
    // constants
    static const int SIFT_FIXPT_SCALE = 1;
    // default number of bins in histogram for orientation assignment
    static const int SIFT_ORI_HIST_BINS = 36;
    // width of border in which to ignore keypoints
    static const int SIFT_IMG_BORDER = 5;
    // maximum steps of keypoint interpolation before failure
    static const int SIFT_MAX_INTERP_STEPS = 5;

    // determines gaussian sigma for orientation assignment
    static const float SIFT_ORI_SIG_FCTR = 1.5f;

    // determines the radius of the region used in orientation assignment
    static const float SIFT_ORI_RADIUS = 3 * SIFT_ORI_SIG_FCTR;

    // orientation magnitude relative to max that results in new feature
    static const float SIFT_ORI_PEAK_RATIO = 0.8f;

    // determines the size of a single descriptor orientation histogram
    static const float SIFT_DESCR_SCL_FCTR = 3.f;

    // threshold on magnitude of elements of descriptor vector
    static const float SIFT_DESCR_MAG_THR = 0.2f;

    // factor used to convert floating-point descriptor to unsigned char
    static const float SIFT_INT_DESCR_FCTR = 512.f;

    //static const float FLT_EPSILON = 1.19209e-07;

    // init vars
    int nOctaveLayers = 3;
    double contrastThreshold = 0.04;
    double edgeThreshold = 10;
    double sigma = 1.6;

    /// @brief 
    /// @return void
    void buildGaussianPyramid(const img_t& base, vimg_t& pyr, int nOctaves)
    {
        //std::vector<double> sig(nOctaveLayers + 3);
        pyr.resize(nOctaves * (nOctaveLayers + 3));
        for (auto& e : pyr) {
            e.img.resize(1024);
            e.cx = base.cx;
            e.cy = base.cy;
        }

        //// precompute Gaussian sigmas using the following formula:
        ////  \sigma_{total}^2 = \sigma_{i}^2 + \sigma_{i-1}^2
        //sig[0] = sigma;
        //double k = std::pow(2., 1. / nOctaveLayers);
        //for (int i = 1; i != nOctaveLayers + 3; ++i)
        //{
        //    double sig_prev = std::pow(k, (double)(i - 1)) * sigma;
        //    double sig_total = sig_prev * k;
        //    sig[i] = std::sqrt(sig_total * sig_total - sig_prev * sig_prev);

        //}

        for (int o = 0; o < nOctaves; o++)
        {
            for (int i = 0; i < nOctaveLayers + 3; i++)
            {
                img_t& dst = pyr[o * (nOctaveLayers + 3) + i];
                if (o == 0 && i == 0)
                    dst = base;
                // base of new octave is halved image from end of previous octave
                else if (i == 0)
                {
                    const img_t& src = pyr[(o - 1) * (nOctaveLayers + 3) + nOctaveLayers];
                    imgRECT src_rt(0, 0, src.cx, src.cy);
                    imgRECT dst_rt(0, 0, dst.cx*2, dst.cy*2);
                    resize(src.img, src_rt, dst.img, dst_rt);
                }
                else
                {
                    const img_t& src = pyr[o * (nOctaveLayers + 3) + i - 1];
                    imgRECT src_rt(0, 0, src.cx, src.cy);
                    gaussian(src.img, src_rt, dst.img);
                }
            }
        }
    }

    /// @brief buildDoGPyramid
    /// @return void
    void buildDoGPyramid(const vimg_t& gpyr, vimage_t& dogpyr)
    {
        int nOctaves = (int)gpyr.size() / (nOctaveLayers + 3);
        dogpyr.resize(nOctaves * (nOctaveLayers + 2));

        //const int o = a / (nOctaveLayers + 2);
        //const int i = a % (nOctaveLayers + 2);
        for (int o = 0; o != nOctaves; ++o)
        {
            for (int i = 0; i != nOctaveLayers + 2; ++i) {
                const img_t& src1 = gpyr[o * (nOctaveLayers + 3) + i];
                const img_t& src2 = gpyr[o * (nOctaveLayers + 3) + i + 1];
                image_t& dst = dogpyr[o * (nOctaveLayers + 2) + i];
                //subtract(src2, src1, dst, noArray(), DataType<sift_wt>::type);
                imgRECT src1_rt(0, 0, src1.cx, src1.cy);
                imgRECT src2_rt(0, 0, src2.cx, src2.cy);
                dst.cx = src1.cx; dst.cy = src1.cy;
                img_t tdst;
                imgRECT dst_rt;
                bit_minus(src1.img, src1_rt, src2.img, src2_rt, tdst.img, dst_rt);
                
                dst.img.assign(tdst.img.begin(), tdst.img.end());
            }
        }
    }


    template<typename _Tp, int m=3, int n=3> inline
        std::vector<_Tp> Matx33(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8)
    {
        std::vector<_Tp> val(m * n);
        val[0] = v0; val[1] = v1; val[2] = v2; val[3] = v3;
        val[4] = v4; val[5] = v5; val[6] = v6; val[7] = v7;
        val[8] = v8; //val[9] = v9;
        //for (int i = 10; i < channels; i++) val[i] = _Tp(0);
        return val;
    }
    template<typename _Tp, int m=3, int n=1> inline
        std::vector<_Tp> Matx31(_Tp v0, _Tp v1, _Tp v2)
    {
        std::vector<_Tp> val(m * n);
        val[0] = v0; val[1] = v1; val[2] = v2; //val[3] = v3;
        //val[4] = v4; val[5] = v5; val[6] = v6; val[7] = v7;
        //val[8] = v8; //val[9] = v9;
        //for (int i = 10; i < channels; i++) val[i] = _Tp(0);
        return val;
    }
    template<typename _Tp> static inline int
    LUImpl(_Tp* A, size_t astep, int m, _Tp* b, size_t bstep, int n, _Tp eps)
    {
        int i, j, k, p = 1;
        astep /= sizeof(A[0]);
        bstep /= sizeof(b[0]);

        for (i = 0; i < m; i++)
        {
            k = i;

            for (j = i + 1; j < m; j++)
                if (std::abs(A[j * astep + i]) > std::abs(A[k * astep + i]))
                    k = j;

            if (std::abs(A[k * astep + i]) < eps)
                return 0;

            if (k != i)
            {
                for (j = i; j < m; j++)
                    std::swap(A[i * astep + j], A[k * astep + j]);
                if (b)
                    for (j = 0; j < n; j++)
                        std::swap(b[i * bstep + j], b[k * bstep + j]);
                p = -p;
            }

            _Tp d = -1 / A[i * astep + i];

            for (j = i + 1; j < m; j++)
            {
                _Tp alpha = A[j * astep + i] * d;

                for (k = i + 1; k < m; k++)
                    A[j * astep + k] += alpha * A[i * astep + k];

                if (b)
                    for (k = 0; k < n; k++)
                        b[j * bstep + k] += alpha * b[i * bstep + k];
            }
        }

        if (b)
        {
            for (i = m - 1; i >= 0; i--)
                for (j = 0; j < n; j++)
                {
                    _Tp s = b[i * bstep + j];
                    for (k = i + 1; k < m; k++)
                        s -= A[i * astep + k] * b[k * bstep + j];
                    b[i * bstep + j] = s / A[i * astep + i];
                }
        }

        return p;
    }

    bool solve(image_t& _src, std::vector<float> &_src2arg, std::vector<float> &_dst)
    {

        int m = _src.cy, m_ = m, n = _src.cx, nb = 3;
        std::vector<float> dst(_src.cx * 3);
        bool result = LUImpl<float>((float*)_src.img.data(), 1, n, dst.data(), 1, nb, FLT_EPSILON*10);
        return result;
    }

    template<typename _Tp, int m, int n> inline
        _Tp dot(const std::vector<_Tp>& src, const std::vector<_Tp>& M)
    {
        _Tp s = 0;
        for (int i = 0; i != src.size(); ++i) s += src[i] * M[i];
        return s;
    }
    // Interpolates a scale-space extremum's location and scale to subpixel
    // accuracy to form an image feature. Rejects features with low contrast.
    // Based on Section 4 of Lowe's paper.
    /// @brief adjustLocalExtrema
    /// @return bool
    bool adjustLocalExtrema(const vimage_t& dog_pyr, PointData& kpt, int octv,
        int& layer, int& r, int& c, int nOctaveLayers,
        float contrastThreshold, float edgeThreshold, float sigma)
    {
        const float img_scale = 1.f / (255 * SIFT_FIXPT_SCALE);
        const float deriv_scale = img_scale * 0.5f;
        const float second_deriv_scale = img_scale;
        const float cross_deriv_scale = img_scale * 0.25f;

        float xi = 0, xr = 0, xc = 0, contr = 0;
        int i = 0;

        for (; i < SIFT_MAX_INTERP_STEPS; i++)
        {
            int idx = octv * (nOctaveLayers + 2) + layer;
            const image_t& img = dog_pyr[idx];
            const image_t& prev = dog_pyr[idx - 1];
            const image_t& next = dog_pyr[idx + 1];

            std::vector<double> dD{ (img.img[(r * img.cx + c + 1)] - img.img[(r * img.cx + c - 1)])* deriv_scale,
                (img.img[(r + 1) * img.cx + c] - img.img[(r - 1) * img.cx + c])* deriv_scale,
                (next.img[(r * next.cx + c)] - prev.img[(r * prev.cx + c)])* deriv_scale };

            float v2 = (float)img.img[(r*img.cx + c)] * 2;
            float dxx = (img.img[(r*img.cx + c + 1)] + img.img[(r*img.cx + c - 1)] - v2) * second_deriv_scale;
            float dyy = (img.img[(r + 1)*img.cx + c] + img.img[(r - 1)*img.cx + c] - v2) * second_deriv_scale;
            float dss = (next.img[(r*next.cx + c)] + prev.img[(r*prev.cx + c)] - v2) * second_deriv_scale;
            float dxy = (img.img[(r + 1)*img.cx + c + 1] - img.img[(r + 1)*img.cx + c - 1] -
                img.img[(r - 1)*img.cx + c + 1] + img.img[(r - 1)*img.cx + c - 1]) * cross_deriv_scale;
            float dxs = (next.img[(r*next.cx + c + 1)] - next.img[(r*next.cx + c - 1)] -
                prev.img[(r*prev.cx + c + 1)] + prev.img[(r*prev.cx + c - 1)]) * cross_deriv_scale;
            float dys = (next.img[(r + 1)*next.cx + c] - next.img[(r - 1)*next.cx + c] -
                prev.img[(r + 1)*prev.cx + c] + prev.img[(r - 1)*prev.cx + c]) * cross_deriv_scale;

            std::vector<float> H = Matx33<float, 3, 3>(dxx, dxy, dxs,
                dxy, dyy, dys,
                dxs, dys, dss);

            //Vec3f X = H.solve(dD, DECOMP_LU);
            std::vector<float> X(3);
            //LUImpl(a.ptr<float>(), a.step, n, X.data(), 1, nb);
            image_t dDimg(dD, 3, 3);
            solve(dDimg, H, X);

            xi = -X[2];
            xr = -X[1];
            xc = -X[0];

            if (std::abs(xi) < 0.5f && std::abs(xr) < 0.5f && std::abs(xc) < 0.5f)
                break;

            if (std::abs(xi) > (float)(INT_MAX / 3) ||
                std::abs(xr) > (float)(INT_MAX / 3) ||
                std::abs(xc) > (float)(INT_MAX / 3))
                return false;

            c += std::round(xc);
            r += std::round(xr);
            layer += std::round(xi);

            if (layer < 1 || layer > nOctaveLayers ||
                c < SIFT_IMG_BORDER || c >= img.cx - SIFT_IMG_BORDER ||
                r < SIFT_IMG_BORDER || r >= img.cy - SIFT_IMG_BORDER)
                return false;
        }

        // ensure convergence of interpolation
        if (i >= SIFT_MAX_INTERP_STEPS)
            return false;

        {
            int idx = octv * (nOctaveLayers + 2) + layer;
            const image_t& img = dog_pyr[idx];
            const image_t& prev = dog_pyr[idx - 1];
            const image_t& next = dog_pyr[idx + 1];
            std::vector<float> dD = Matx31<float, 3, 1>((img.img[(r*img.cx + c + 1)] - img.img[(r*img.cx + c - 1)]) * deriv_scale,
                (img.img[(r + 1)*img.cx + c] - img.img[(r - 1)*img.cx + c]) * deriv_scale,
                (next.img[(r*next.cx + c)] - prev.img[(r*prev.cx + c)]) * deriv_scale);
            //float t = dD.dot(Matx31f(xc, xr, xi));
            float t = dot<float, 3, 1>(dD, Matx31<float,3,1>(xc, xr, xi));

            contr = img.img[(r*img.cx + c)] * img_scale + t * 0.5f;
            if (std::abs(contr) * nOctaveLayers < contrastThreshold)
                return false;

            // principal curvatures are computed using the trace and det of Hessian
            float v2 = img.img[(r*img.cx + c)] * 2.f;
            float dxx = (img.img[(r*img.cx + c + 1)] + img.img[(r*img.cx + c - 1)] - v2) * second_deriv_scale;
            float dyy = (img.img[(r + 1)*img.cx + c] + img.img[(r - 1)*img.cx + c] - v2) * second_deriv_scale;
            float dxy = (img.img[(r + 1)*img.cx + c + 1] - img.img[(r + 1)*img.cx + c - 1] -
                img.img[(r - 1)*img.cx + c + 1] + img.img[(r - 1)*img.cx + c - 1]) * cross_deriv_scale;
            float tr = dxx + dyy;
            float det = dxx * dyy - dxy * dxy;

            if (det <= 0 || tr * tr * edgeThreshold >= (edgeThreshold + 1) * (edgeThreshold + 1) * det)
                return false;
        }

        kpt.x = (c + xc) * (1 << octv);
        kpt.y = (r + xr) * (1 << octv);
        kpt.o = octv + (layer << 8) + ((int)std::round((xi + 0.5) * 255) << 16);
        kpt.s = sigma * powf(2.f, (layer + xi) / nOctaveLayers) * (1 << octv) * 2;
        kpt.r = std::abs(contr);

        return true;
    }

    // Computes a gradient orientation histogram at a specified pixel
    static float calcOrientationHist(const img_t& img, Point pt, int radius,
        float sigma, float* hist, int n)
    {
        int i, j, k, len = (radius * 2 + 1) * (radius * 2 + 1);

        float expf_scale = -1.f / (2.f * sigma * sigma);
        std::vector<float> buf(len * 4 + n + 4);
        float* X = buf.data(), * Y = X + len, * Mag = X, * Ori = Y + len, * W = Ori + len;
        float* temphist = W + len + 2;

        for (i = 0; i < n; i++)
            temphist[i] = 0.f;

        for (i = -radius, k = 0; i <= radius; i++)
        {
            int y = pt.y + i;
            if (y <= 0 || y >= img.cy - 1)
                continue;
            for (j = -radius; j <= radius; j++)
            {
                int x = pt.x + j;
                if (x <= 0 || x >= img.cx - 1)
                    continue;

                float dx = (float)(img.img[(y*img.cx + x + 1)] - img.img[(y*img.cx + x - 1)]);
                float dy = (float)(img.img[(y - 1)*img.cx + x] - img.img[(y + 1)*img.cx + x]);

                X[k] = dx; Y[k] = dy; W[k] = (i * i + j * j) * expf_scale;
                k++;
            }
        }

        len = k;

        // compute gradient values, orientations and the weights over the pixel neighborhood
        int len2 = len - 1 > -1 ? len - 1 : 0 ;
        W[len2] = std::exp(W[len2]); // exp(W, W, len);
        Ori[len2] = std::atan2(Y[len2], X[len2]); //fastAtan2(Y, X, Ori, len, true);
        Mag[len2] = std::abs(X[len2] * Y[len2]); // magnitude(X, Y, Mag, len);

        for (k = 0; k < len; k++)
        {
            int bin = std::round((n / 360.f) * Ori[k]);
            if (bin >= n)
                bin -= n;
            if (bin < 0)
                bin += n;
            temphist[bin] += W[k] * Mag[k];
        }

        // smooth the histogram
        temphist[-1] = temphist[n - 1];
        temphist[-2] = temphist[n - 2];
        temphist[n] = temphist[0];
        temphist[n + 1] = temphist[1];
        for (i = 0; i < n; i++)
        {
            hist[i] = (temphist[i - 2] + temphist[i + 2]) * (1.f / 16.f) +
                (temphist[i - 1] + temphist[i + 1]) * (4.f / 16.f) +
                temphist[i] * (6.f / 16.f);
        }

        float maxval = hist[0];
        for (i = 1; i < n; i++)
            maxval = max(maxval, hist[i]);

        return maxval;
    }



    // Detects features at extrema in DoG scale space.  Bad features are discarded
    // based on contrast and ratio of principal curvatures.
    /// @biref  findScaleSpaceExtrema
    /// @return void
    void findScaleSpaceExtrema(vimg_t& gauss_pyr, const vimage_t& dog_pyr,
        vPtd& keypoints)
    {

        const int nOctaves = (int)gauss_pyr.size() / (nOctaveLayers + 3);
        const int threshold = std::floor(0.5 * contrastThreshold / nOctaveLayers * 255 * SIFT_FIXPT_SCALE);

        keypoints.clear();

        for (int o = 0; o < nOctaves; o++)
            for (int i = 1; i <= nOctaveLayers; i++)
            {
                const int idx = o * (nOctaveLayers + 2) + i;
                const image_t& img = dog_pyr[idx];
                const int step = 1;
                const int rows = img.cy, cols = img.cx;

                {
                    static const int n = SIFT_ORI_HIST_BINS;
                    float hist[n];

                    const image_t& img = dog_pyr[idx];
                    const image_t& prev = dog_pyr[idx - 1];
                    const image_t& next = dog_pyr[idx + 1];

                    PointData kpt;
                    for (int r = SIFT_IMG_BORDER; r != rows- SIFT_IMG_BORDER; ++r)
                    {
                        const double* currptr = &(img.img[r*img.cx]);
                        const double* prevptr = &(prev.img[r * prev.cx]);;
                        const double* nextptr = &(next.img[r * next.cx]);;

                        for (int c = SIFT_IMG_BORDER; c < cols - SIFT_IMG_BORDER; c++)
                        {
                            double val = currptr[c];

                            // find local extrema with pixel accuracy
                            if (std::abs(val) > threshold &&
                                ((val > 0 && val >= currptr[c - 1] && val >= currptr[c + 1] &&
                                    val >= currptr[c - step - 1] && val >= currptr[c - step] && val >= currptr[c - step + 1] &&
                                    val >= currptr[c + step - 1] && val >= currptr[c + step] && val >= currptr[c + step + 1] &&
                                    val >= nextptr[c] && val >= nextptr[c - 1] && val >= nextptr[c + 1] &&
                                    val >= nextptr[c - step - 1] && val >= nextptr[c - step] && val >= nextptr[c - step + 1] &&
                                    val >= nextptr[c + step - 1] && val >= nextptr[c + step] && val >= nextptr[c + step + 1] &&
                                    val >= prevptr[c] && val >= prevptr[c - 1] && val >= prevptr[c + 1] &&
                                    val >= prevptr[c - step - 1] && val >= prevptr[c - step] && val >= prevptr[c - step + 1] &&
                                    val >= prevptr[c + step - 1] && val >= prevptr[c + step] && val >= prevptr[c + step + 1]) ||
                                    (val < 0 && val <= currptr[c - 1] && val <= currptr[c + 1] &&
                                        val <= currptr[c - step - 1] && val <= currptr[c - step] && val <= currptr[c - step + 1] &&
                                        val <= currptr[c + step - 1] && val <= currptr[c + step] && val <= currptr[c + step + 1] &&
                                        val <= nextptr[c] && val <= nextptr[c - 1] && val <= nextptr[c + 1] &&
                                        val <= nextptr[c - step - 1] && val <= nextptr[c - step] && val <= nextptr[c - step + 1] &&
                                        val <= nextptr[c + step - 1] && val <= nextptr[c + step] && val <= nextptr[c + step + 1] &&
                                        val <= prevptr[c] && val <= prevptr[c - 1] && val <= prevptr[c + 1] &&
                                        val <= prevptr[c - step - 1] && val <= prevptr[c - step] && val <= prevptr[c - step + 1] &&
                                        val <= prevptr[c + step - 1] && val <= prevptr[c + step] && val <= prevptr[c + step + 1])))
                            {
                                int r1 = r, c1 = c, layer = i;
                                if (!adjustLocalExtrema(dog_pyr, kpt, o, layer, r1, c1,
                                    nOctaveLayers, (float)contrastThreshold,
                                    (float)edgeThreshold, (float)sigma))
                                    continue;
                                float scl_octv = kpt.s * 0.5f / (1 << o);
                                float omax = calcOrientationHist(gauss_pyr[o * (nOctaveLayers + 3) + layer],
                                    Point(c1, r1),
                                    std::round(SIFT_ORI_RADIUS * scl_octv),
                                    SIFT_ORI_SIG_FCTR * scl_octv,
                                    hist, n);
                                float mag_thr = (float)(omax * SIFT_ORI_PEAK_RATIO);
                                for (int j = 0; j < n; j++)
                                {
                                    int l = j > 0 ? j - 1 : n - 1;
                                    int r2 = j < n - 1 ? j + 1 : 0;

                                    if (hist[j] > hist[l] && hist[j] > hist[r2] && hist[j] >= mag_thr)
                                    {
                                        float bin = j + 0.5f * (hist[l] - hist[r2]) / (hist[l] - 2 * hist[j] + hist[r2]);
                                        bin = bin < 0 ? n + bin : bin >= n ? bin - n : bin;
                                        kpt.a = 360.f - (float)((360.f / n) * bin);
                                        if (std::abs(kpt.a - 360.f) < FLT_EPSILON)
                                            kpt.a = 0.f;
                                        {
                                            keypoints.push_back(kpt);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }


            }

    }

    struct KeyPoint12_LessThan
    {
        bool operator()(const PointData& kp1, const PointData& kp2) const
        {
            if (kp1.x != kp2.x)
                return kp1.x < kp2.x;
            if (kp1.y != kp2.y)
                return kp1.y < kp2.y;
            if (kp1.s != kp2.s)
                return kp1.s > kp2.s;
            if (kp1.a != kp2.a)
                return kp1.a < kp2.a;
            if (kp1.r != kp2.r)
                return kp1.r > kp2.r;
            if (kp1.o != kp2.o)
                return kp1.o > kp2.o;
            return kp1.v > kp2.v;
        }
    };


    void removeDuplicatedSorted(std::vector<PointData>& keypoints)
    {
        int i, j, n = (int)keypoints.size();

        if (n < 2) return;

        std::sort(keypoints.begin(), keypoints.end(), KeyPoint12_LessThan());

        for (i = 0, j = 1; j < n; ++j)
        {
            const PointData& kp1 = keypoints[i];
            const PointData& kp2 = keypoints[j];
            if (kp1.x != kp2.x || kp1.y != kp2.y ||
                kp1.s != kp2.s || kp1.a != kp2.a) {
                keypoints[++i] = keypoints[j];
            }
        }
        keypoints.resize(i + 1);
    }

    struct KeypointResponseGreaterThanThreshold
    {
        KeypointResponseGreaterThanThreshold(float _value) :
            value(_value)
        {
        }
        inline bool operator()(const PointData& kpt) const
        {
            return kpt.r >= value;
        }
        float value;
    };

    struct KeypointResponseGreater
    {
        inline bool operator()(const PointData& kp1, const PointData& kp2) const
        {
            return kp1.r > kp2.r;
        }
    };

    // takes keypoints and culls them by the response
    void retainBest(std::vector<PointData>& keypoints, int n_points)
    {
        //this is only necessary if the keypoints size is greater than the number of desired points.
        if (n_points >= 0 && keypoints.size() > (size_t)n_points)
        {
            if (n_points == 0)
            {
                keypoints.clear();
                return;
            }
            //first use nth element to partition the keypoints into the best and worst.
            std::nth_element(keypoints.begin(), keypoints.begin() + n_points - 1, keypoints.end(), KeypointResponseGreater());
            //this is the boundary response, and in the case of FAST may be ambiguous
            float ambiguous_response = keypoints[n_points - 1].r;
            //use std::partition to grab all of the keypoints with the boundary response.
            std::vector<PointData>::const_iterator new_end =
                std::partition(keypoints.begin() + n_points, keypoints.end(),
                    KeypointResponseGreaterThanThreshold(ambiguous_response));
            //resize the keypoints, given this new end point. nth_element and partition reordered the points inplace
            keypoints.resize(new_end - keypoints.begin());
        }
    }


    /// @brief make_sift
    /// @return vPtd
    vPtd make_sift(const vImg& src, size_t cx, size_t cy, int nfeatures) {
        // init
        int firstOctave = -1, actualNOctaves = 0, actualNLayers = 0;

        img_t base;
        base.cx = cx;
        base.cy = cy;
        base.img = src;

        vimg_t gpyr(src.size());
        int nOctaves = actualNOctaves > 0 ? actualNOctaves : std::round(std::log((double)min(base.cx, base.cy)) / std::log(2.) - 2) - firstOctave;

        buildGaussianPyramid(base, gpyr, nOctaves);

        // get feature
        {
            vimage_t dogpyr(gpyr.size());
            vPtd keypoints;
            buildDoGPyramid(gpyr, dogpyr);
            findScaleSpaceExtrema(gpyr, dogpyr, keypoints);
            removeDuplicatedSorted(keypoints);

            if (nfeatures > 0)
                retainBest(keypoints, nfeatures);
            //t = (double)getTickCount() - t;
            //printf("keypoint detection time: %g\n", t*1000./tf);

            if (firstOctave < 0)
                for (size_t i = 0; i < keypoints.size(); i++)
                {
                    PointData& kpt = keypoints[i];
                    float scale = 1.f / (float)(1 << -firstOctave);
                    kpt.o = (kpt.o & ~255) | ((kpt.o + firstOctave) & 255);
                    kpt.v *= scale;
                    kpt.s *= scale;
                }

            // no mask
            //if (!mask.empty())
            //    KeyPointsFilter::runByPixelsMask(keypoints, mask);

            return keypoints;
        }

        return vPtd();
    }

}