/// @file	ocr_func.h
/// @brief	ocr relative functions
/// @author	locketk9@gmail.com

#ifndef _OCR_FUNC_H
#define _OCR_FUNC_H


#include "ocr_def.h"

#if 0
/// @brief load bmp file - func OK
///int loadBMP(const char *filename, ch_vec &bmp, int &width, int &height);
int saveBMP(const char *filename, const byte *img, int width
			, int height, int bp=3) ;

/// @brief bmp 2 raw
byte* bmp2raw(const byte *bmp, byte *conv, int imgWidth, int imgHeight
	, int bp)  ;
#endif

/// @brief color to grayscale
ch_vec& to_grayscale(const ch_vec &src, ch_vec &dst)  ;
ch_vec& to_grayscale(const byte *data, int width, int height, ch_vec &dst)  ;

/// #biref	ToGrey - internet src it's working
void ToGrey(CBitmap& color, CBitmap& grey, unsigned int bitspixel)  ;

/// @brief	DrawDIBSection -  internet src, it's not working
void DrawDIBSection(HDC hDC, HBITMAP hBitmap, int xDest, int yDest)  ;

/// @brief DrawGrayScale - internet src, it's not working
void DrawGrayScale(CDC *pDC, HANDLE hDIB)  ;

/// @brief	draw_bmp
int draw_bmp(HDC hdc, int x, int y, int width, int height
	, const ch_vec &raw)   ;


/// @brief	histogram
ch_vec& histogram(const ch_vec &gray, ch_vec &hg)   ;
/// @brief	draw histogram
int draw_hg(HDC hdc, int x, int y, ch_vec &hg)  ;
/// @brief histogram equalization
ch_vec& hg_equalconst(const ch_vec &src, const imgRECT rect
						, ch_vec &dst)  ;



/// @brief	sobel
ch_vec& sobel(const ch_vec &src, const imgRECT &rect, ch_vec &dst)  ;


/// @brief	bit functinos
imgRECT bit_plus(ch_vec &a, const imgRECT &ra
					, ch_vec &b, const imgRECT &rb)  ;
imgRECT bit_minus(ch_vec& a, const imgRECT& ra
	, ch_vec& b, const imgRECT& rb);
imgRECT bit_minus(const ch_vec& a, const imgRECT& ra
	, const ch_vec& b, const imgRECT& rb
	, ch_vec& c, imgRECT& rc);

imgRECT bit_blz(ch_vec &a, const imgRECT &ra
	, ch_vec &b, const imgRECT &rb);
bool bit_and(ch_vec &a, const imgRECT &ra
	, ch_vec &b, const imgRECT &rb, int threshold, int cnt);

/// @brief set function
/// @brief 차집합
imgRECT set_minus(ch_vec &a, const imgRECT &ra
			 , ch_vec &b, const imgRECT &rb
			 , ch_vec &c, imgRECT &rc)  ;
/// @brief 교집합
imgRECT set_intersect(ch_vec &a, const imgRECT &ra
			 , ch_vec &b, const imgRECT &rb
			 , ch_vec &c, imgRECT &rc)  ;
/// @brief 여집합
imgRECT set_complement(ch_vec &a, const imgRECT &ra
	, ch_vec &b, imgRECT &rb)  ;
/// @brief 합집합 - 일단 보류
imgRECT set_union(ch_vec &a, const imgRECT &ra
			 , ch_vec &b, const imgRECT &rb
			 , ch_vec &c, imgRECT &rc)  ;




/// @brief order_statistics(순서-통계(비선형)필터)
ch_vec& order_statistics(const ch_vec &src, int width, int height
	, ch_vec &dst, int kernel = 3)  ;

/// @todo	unsharp_masking
ch_vec& unsharp_masking(const ch_vec &src, int width, int height
	, ch_vec &dst)  ;

/// @brief	otsu
ch_vec& otsu(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, double &threshold)  ;

/// @brief segmentation for 이진화영상.
rt_set&	segmentation(const ch_vec &src, const imgRECT &rect
	, rt_set &seg)  ;
rt_vec&	segmentation(const ch_vec &src, const imgRECT &rect
	, rt_vec &seg);
/// @brief	seg_copy
ch_vec& seg_copy(const ch_vec &src, const imgRECT &rect
	, ch_vec &cp, const imgRECT &seg)  ;
/// @brief	seg_paste
ch_vec& seg_paste(ch_vec &tar, const imgRECT rect
	, const ch_vec &paste, const imgRECT &seg)  ;

/// @brief draw segmaentation
int draw_seg(HDC hdc, const rt_set &seg)  ;
int draw_seg(HDC hdc, const rt_vec &seg);



/// @brief affine function by internet
ch_vec& affine(const ch_vec &src, imgRECT &rect, ch_vec &dst
			   , double x_rot, double y_rot
			   , double x_expan = 1, double y_expan = 1
			   , int x_move = 0, int y_move = 0)  ;

/// @brief contour
ch_vec& contour(const ch_vec &src, const imgRECT &rect, ch_vec &dst
	, double Tm = 179.2)  ;
/// @brief fill_line
ch_vec& fill_line(const ch_vec &src, const imgRECT &rect, ch_vec &dst
	, int K = 25)  ;


/// @brief	canny
ch_vec& gaussian(const ch_vec &src, const imgRECT &rect, ch_vec &dst);
ch_vec& canny(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, double t_high, double t_low);
ch_vec& canny2(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, double t_high, double t_low, bool L2gradient = false);




/// @breif morphology
/// @brief 침식
ch_vec& erode(const ch_vec &src, const imgRECT &rect
				, ch_vec &dst, const byte se[3][3]
				, const imgRECT &se_rt)  ;
ch_vec& erode(const ch_vec &src, const imgRECT &rect
				, ch_vec &dst, const ch_vec &se
				, const imgRECT &se_rt)  ;
/// @brief 팽창
ch_vec& dilate(const ch_vec &src, const imgRECT &rect
				, ch_vec &dst, const byte se[3][3]
				, const imgRECT &se_rt)  ;
ch_vec& dilate(const ch_vec &src, const imgRECT &rect
				, ch_vec &dst, const ch_vec &se
				, const imgRECT &se_rt)  ;
/// @brief 원점반사
ch_vec& reflect(const ch_vec &src, const imgRECT rect
	, ch_vec &dst)  ;

/// @brief 열기 후 닫기
ch_vec& open2close(const ch_vec &src, const imgRECT rect
	, ch_vec &dst, imgRECT rt)  ;

/// @brief 적중
ch_vec& match(const ch_vec &a, const imgRECT &seg
			  , ch_vec &dst, imgRECT &dr
			  , const byte se[3][3], imgRECT &se_rt)  ;

/// @brief 세선화 - 잘 안된다.
ch_vec& thinning(const ch_vec &src, const imgRECT &seg
	, ch_vec &dst)  ;

/// @brief   홀채우기
ch_vec& fill_hole(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, const imgRECT seg)   ;


/// @brief	경계추출
ch_vec& boundary(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, const imgRECT &seg)   ;


/// @brief	signature
db_vec& signature(const ch_vec &img, const imgRECT &rect, const imgRECT &seg
	, db_vec &sig);
db_vec& signature2(const ch_vec &img, const imgRECT &rect, const imgRECT &seg
	, db_vec &sig);
ch_vec& signature3(const ch_vec &thin, const imgRECT &rect, const imgRECT &seg
	, ch_vec &sig);


/// @brief	허프변환
line_vec& hough_line(const ch_vec &img, const imgRECT &rect, line_vec& lines
	, double rho, double theta, double threshold
	, double min_theta = 0.0, double max_theta = PI);

/// @brief	라인찾기 - 잘되면 좋겠다.
ch_vec& find_LinePolar(const ch_vec &src, const imgRECT &rect
	, const line_vec &lines, ch_vec &dst);


/// @brief	허프 원 변환 - 기울기메소드, OpenCV 참조
///			소스분석을 하면서 한글 주석을 먼저 달고 코딩을 시작하자.
circle_vec & hough_circle(const ch_vec &img, const imgRECT &rect
	, circle_vec &circles, double dp, double min_dist
	, double threshold, double min_radius, double max_radius);



/// @brief sub sampleing
ch_vec& subsampling(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, double x_expan, double y_expan)  ;
ch_vec& resampling(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, imgRECT &rt);
ch_vec& resize(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, imgRECT &rt);
ch_vec& resampling2(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, size_t s_x, size_t s_y);
double sinc(double x);
double LanczosFilter(double x);

/// @brief chaincode
ch_vec& chaincode(const ch_vec &src, const imgRECT rect
	, ch_vec &dst, ch_vec &chain, const imgRECT seg)  ;

/// @brief	boundary following
ch_vec& cascade(ch_vec &img, const imgRECT &rect
	, ch_vec &chain, int expan)  ;
char chain_follow(ch_vec &img, const imgRECT rect
	, int &x, int &y, int expan, int type = 4)  ;

/// @brief	min distance classify
char min_dist_classify(const chain_map &chains
	, const ch_vec &ccode);
std::ptrdiff_t min_dist_classify2(const sig_vec &sigs, const ch_vec &sig);
std::vector<std::pair<int, double>> min_dist_classify3(const sigf_vec &sigs,
	const fl_vec &sig, const rt_vec &segs, const imgRECT &rt);

//std::vector<std::pair<int, double>>
//min_dist_classify4(const sigi_vec& sigs, const int_vec& sig, const rt_vec& segs, const imgRECT& rt);
std::vector<std::pair<int, double>>
min_dist_classify4(const sigi_vec& sigs, const int_vec& sig);



template<class Iter_T, class Iter2_T>
double vectorDistance(Iter_T first, Iter_T last, Iter2_T first2);

/// @brief Hamming Distance
int hammingDist(const sig_vec &sigs, const ch_vec &sig);

/// @bief adapt seg posiztion
imgRECT& adapt_seg(imgRECT& seg1);

#endif // !_OCR_FUNC_H