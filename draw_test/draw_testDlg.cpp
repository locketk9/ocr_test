
// draw_testDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "draw_test.h"
#include "draw_testDlg.h"
#include "afxdialogex.h"

//#include "resource.h"

#include <iterator>

//#include "./include_cv/draw_pic.h"
#include "./include_cv/ocr.h"
#include "./include_cv/zhangsuen.h"
#include "./include_cv/bmp_func.h"
#include "./include_cv/haar_dwt.h"
#include "./include_cv/dt.h"
#include "./include_cv/lbp.h"
#include "./include_cv/hog.h"

#include "./include_cv/orb.h"
#include "./include_cv/sift.h"

#include "./include_cv/seg_projection.h"
#include "./include_cv/skew.h"

//---------------------
#include "../tiny_cnn/tiny_cnn.h"
#include "../tiny_cnn/network.h"

using namespace tiny_cnn;
using namespace tiny_cnn::activation;
//---------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool in_rect(const CRect &rt, CPoint pt)	{
	bool b = false;
	if (rt.left < pt.x && rt.right > pt.x
		&& rt.top < pt.y && rt.bottom > pt.y)
		b = true;
	return b;
}

int draw_pts(HDC hdc, const curve::vertex &vs)	{
	CDC *dc = CDC::FromHandle(hdc);

	//CPen* pPenOld, pen;
	//pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	//pPenOld = dc->SelectObject(&pen);
	//CBrush *bbrush = (CBrush*)dc->SelectStockObject(BLACK_BRUSH);
	for (auto &e : vs)	{
		CRect crt(e.x - 4, e.y - 4, e.x + 8, e.y + 8);
		dc->FillSolidRect(crt, RGB(255,0,0));
	}
	
	//dc->SelectObject(pPenOld);
	//pen.DeleteObject();

	return 0;
}

std::vector<unsigned char> get_bmp(HDC hdc, int cx, int cy)	{
	CDC *dc = CDC::FromHandle(hdc);

	CRect rt(0, 0, cx, cy);

	CDC dcMem;
	CBitmap bitmap;

	dcMem.CreateCompatibleDC(dc);
	bitmap.CreateCompatibleBitmap(dc, rt.Width(), rt.Height());
	CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);

	dcMem.BitBlt(rt.left, rt.top, rt.Width(), rt.Height(), dc, 0, 0, SRCCOPY);

	dcMem.SelectObject(pOldBitmap);

	//CImage img;
	//img.Attach(bitmap);
	//img.Save(_T("save.bmp"));

	int bp = 4; // img.GetBPP() / 8;
	std::vector<unsigned char> rv(cx * cy * bp, 0);
	bitmap.GetBitmapBits(cx * cy * bp, rv.data());
	
	// 32bit -> 24bit
	std::vector<unsigned char> rv2(cx * cy * 3, 0);
	for (int i = 0, j = 0; j < rv2.size(); i += bp, j += 3)	{
		rv2[j] = rv[i];
		rv2[j + 1] = rv[i+1];
		rv2[j + 2] = rv[i+2];
	}

	// rgb -> grayscale
	std::vector<unsigned char> rv3(cx * cy, 0);
	for (int i = 0, j = 0; j < rv3.size(); i += 3, ++j)	{
		double Y = 0.2126 * rv2[i] + 0.7152 * rv2[i + 1] + 0.0722 * rv2[i + 2];
		rv3[j] = Y; //static_cast<unsigned char>(116 * Y) ^ 1 / 3 - 16;
	}

#if 0
	saveBMP("save1.bmp", rv.data(), cx, cy, 4);
	saveBMP("save2.bmp", rv2.data(), cx, cy, 3);
	saveBMP("save3.bmp", rv3.data(), cx, cy, 1);
#endif

	return rv3;
}

int ResultOut(HDC hDC, std::vector<std::pair<int, double>> r, int x=10, int y=10) {
	CDC *dc = CDC::FromHandle(hDC);
	
	CString csr; csr.Format(_T("%d%d%d%d%d%d%d%d%d"), r[0].first, r[1].first, r[2].first, r[3].first, r[4].first, r[5].first, r[6].first, r[7].first, r[8].first);
	//CClientDC dc(&m_pic_result);
	CFont font, *oldFont;
	font.CreateFontW(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
	oldFont = (CFont*)dc->SelectObject(font);
	dc->TextOutW(x, y, csr);
	//dc->SelectObject(oldFont);
	//font.DeleteObject();

	return 0;
}

int ResultOut(HDC hDC, int r) {
	CDC *dc = CDC::FromHandle(hDC);

	CString csr; csr.Format(_T("%d"), r);
	//CClientDC dc(&m_pic_result);
	CFont font, *oldFont;
	font.CreateFontW(80, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
	oldFont = (CFont*)dc->SelectObject(font);
	dc->TextOutW(10, 10, csr);
	//dc->SelectObject(oldFont);
	//font.DeleteObject();

	return 0;
}


//---------------------------
///////////////////////////////////////////////////////////////////////////////
// learning convolutional neural networks (LeNet-5 like architecture)
network<mse, gradient_descent_levenberg_marquardt> nn;
bool g_bnn = false;
int g_type = 0;

void Cdraw_testDlg::sample1_convnet(void) {
	// construct LeNet-5 architecture
//	network<mse, gradient_descent_levenberg_marquardt> nn;

	// connection table [Y.Lecun, 1998 Table.1]
#define O true
#define X false
	static const bool connection[] = {
		O, X, X, X, O, O, O, X, X, O, O, O, O, X, O, O,
		O, O, X, X, X, O, O, O, X, X, O, O, O, O, X, O,
		O, O, O, X, X, X, O, O, O, X, X, O, X, O, O, O,
		X, O, O, O, X, X, O, O, O, O, X, X, O, X, O, O,
		X, X, O, O, O, X, X, O, O, O, O, X, O, O, X, O,
		X, X, X, O, O, O, X, X, O, O, O, O, X, O, O, O
	};
#undef O
#undef X

	nn << convolutional_layer<tan_h>(32, 32, 5, 1, 6) // 32x32 in, 5x5 kernel, 1-6 fmaps conv
		<< average_pooling_layer<tan_h>(28, 28, 6, 2) // 28x28 in, 6 fmaps, 2x2 subsampling
		<< convolutional_layer<tan_h>(14, 14, 5, 6, 16,
			connection_table(connection, 6, 16)) // with connection-table
		<< average_pooling_layer<tan_h>(10, 10, 16, 2)
		<< convolutional_layer<tan_h>(5, 5, 5, 16, 120)
		<< fully_connected_layer<tan_h>(120, 10);

	//	std::cout << "load models..." << std::endl;

	float pos = 0.0;
	m_prog_load.SetRange(0, 100);

	// create callback
	auto on_load_prog = [&](size_t sz) {
	//	disp += minibatch_size;

		pos += 1.0 / sz * 100;
		m_prog_load.SetPos(pos);
	};

	// load weights
	std::ifstream ifs("LeNet-weights");
	//ifs >> nn;
	nn.load2(ifs, on_load_prog);

	//AfxMessageBox(_T("load OK!"));
}


//---------------------------


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cdraw_testDlg 대화 상자



Cdraw_testDlg::Cdraw_testDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cdraw_testDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cdraw_testDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_DRAW, m_picDraw);
	DDX_Control(pDX, IDC_PIC_RESULT, m_pic_result);
	DDX_Control(pDX, IDC_PROG_LOAD, m_prog_load);
	DDX_Control(pDX, IDC_PIC_VP, m_picVP);
	DDX_Control(pDX, IDC_PIC__HP, m_picHP);
}

BEGIN_MESSAGE_MAP(Cdraw_testDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BT_CLEAR, &Cdraw_testDlg::OnBnClickedBtClear)
	ON_BN_CLICKED(IDC_BT_RECOG, &Cdraw_testDlg::OnBnClickedBtRecog)
	ON_BN_CLICKED(IDC_BT_LOAD, &Cdraw_testDlg::OnBnClickedBtLoad)
	ON_BN_CLICKED(IDC_BT_CLOSE, &Cdraw_testDlg::OnBnClickedBtClose)
	ON_BN_CLICKED(IDC_BT_LOAD2, &Cdraw_testDlg::OnBnClickedBtLoad2)
	ON_BN_CLICKED(IDC_BT_LOAD3, &Cdraw_testDlg::OnBnClickedBtLoad3)
	ON_BN_CLICKED(IDC_BT_LOAD4, &Cdraw_testDlg::OnBnClickedBtLoad4)
	ON_BN_CLICKED(IDC_BT_LOAD5, &Cdraw_testDlg::OnBnClickedBtLoad5)
	ON_BN_CLICKED(IDC_BT_LOAD6, &Cdraw_testDlg::OnBnClickedBtLoad6)
	ON_BN_CLICKED(IDC_BT_SKEW, &Cdraw_testDlg::OnBnClickedBtSkew)
END_MESSAGE_MAP()


// Cdraw_testDlg 메시지 처리기

BOOL Cdraw_testDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
		// breif init
	BRIEF::create_synthetic_data(BRIEF::g_angles);

	load_number();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void Cdraw_testDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void Cdraw_testDlg::OnPaint()
{
	static int one1 = 0;
	if (++one1 == 1 )
		OnBnClickedBtClear();

	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		draw_pts(dc.m_hDC, vts_);

		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR Cdraw_testDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL Cdraw_testDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CRect rt;
	m_picDraw.GetClientRect(rt);
	m_picDraw.ClientToScreen(&rt);

	switch (pMsg->message)	{
	case WM_MOUSEMOVE:
	{

						 if (!drawing_ || !in_rect(rt, pMsg->pt)) break;
						 curve::Point pt1(pMsg->pt.x - rt.left, pMsg->pt.y - rt.top);
						 vts_.push_back(pt1);
						 curve::vertex tx; tx.push_back(pt_prev_); tx.push_back(pt1);
						 pt_prev_ = pt1;
						 curve_.setPoints(tx);
						   curve_.drawCurve(curve::CDrawCurve::spline);
						   //CClientDC cd(&m_picDraw);
						   CDC *dc = m_picDraw.GetDC();
						   draw_pts(dc->m_hDC, curve_.getPts());
						   //draw_pts(dc->m_hDC, vts_);
						   m_picDraw.ReleaseDC(dc);
						   break;
	}
	case WM_LBUTTONDOWN: 
		drawing_ = true;
		pt_prev_ = curve::Point(pMsg->pt.x - rt.left, pMsg->pt.y - rt.top);
		break;
	case WM_LBUTTONUP: 
		drawing_ = false;
		//pt_prev_ = curve::Point(pMsg->pt.x - rt.left, pMsg->pt.y - rt.top); 
		break;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// clear board
void Cdraw_testDlg::OnBnClickedBtClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	vts_.clear();
	{
		CDC *dc = m_picDraw.GetDC();

		CRect rt;
		m_picDraw.GetClientRect(rt);

		CBrush *bs = (CBrush*)dc->SelectStockObject(WHITE_BRUSH);
		dc->FillRect(rt, bs);
		dc->Rectangle(rt);

		m_picDraw.ReleaseDC(dc);
	}
	{
		CDC *dc = m_pic_result.GetDC();

		CRect rt;
		m_pic_result.GetClientRect(rt);

		CBrush *bs = (CBrush*)dc->SelectStockObject(WHITE_BRUSH);
		dc->FillRect(rt, bs);
		dc->Rectangle(rt);

		m_pic_result.ReleaseDC(dc);
	}
	{
		CDC* dc = m_picHP.GetDC();

		CRect rt;
		m_picHP.GetClientRect(rt);

		CBrush* bs = (CBrush*)dc->SelectStockObject(WHITE_BRUSH);
		dc->FillRect(rt, bs);
		dc->Rectangle(rt);

		m_picHP.ReleaseDC(dc);
	}
	{
		CDC* dc = m_picVP.GetDC();

		CRect rt;
		m_picVP.GetClientRect(rt);

		CBrush* bs = (CBrush*)dc->SelectStockObject(WHITE_BRUSH);
		dc->FillRect(rt, bs);
		dc->Rectangle(rt);

		m_picVP.ReleaseDC(dc);
	}

}

#define SAMPLE_W 32
#define SAMPLE_H 32


vec_t& parse_image(
	std::vector<uint8_t> &src,
	//std::ifstream& ifs,
//	const mnist_header& header,
int rows,
int cols,
float scale_min,
float scale_max,
int x_padding,
int y_padding,
vec_t& dst) {
	const int width = cols + 2 * x_padding;
	const int height = rows + 2 * y_padding;

	//	std::vector<uint8_t> image_vec(rows * cols);
	std::vector<uint8_t> image_vec(src);

	//	ifs.read((char*)&image_vec[0], rows * cols);

	dst.resize(width * height, scale_min);

	for (size_t y = 0; y < rows; y++)
		for (size_t x = 0; x < cols; x++)
			dst[width * (y + y_padding) + x + x_padding]
			= (image_vec[y * cols + x] / 255.0) * (scale_max - scale_min) + scale_min;

	return dst;
}
////////////////


int Cdraw_testDlg::draw_HP(const int_vec& hp, const CRect& draw) {
	CRect rt;
	m_picHP.GetClientRect(rt);

	int x = rt.left;
	int y = rt.bottom;
	float cx = rt.Width() / (float)draw.Width();
	float cy = rt.Height() / (float)draw.Height();

	CDC* pdc = m_picHP.GetDC();
	for (int i = 0; i != hp.size(); ++i) {
		CRect ort(i, rt.Height()-hp[i]*cy, i+1, y);
		pdc->FillSolidRect(ort, RGB(0, 0, 255));
		//hps_.push_back(ort);
	}
	m_picHP.ReleaseDC(pdc);

	return 0;
}

int Cdraw_testDlg::draw_VP(const int_vec& vp, const CRect& draw) {
	CRect rt;
	m_picVP.GetClientRect(rt);

	int x = rt.left;
	int y = rt.top;
	float cx = rt.Width() / (float)draw.Width();
	float cy = rt.Height() / (float)draw.Height();

	CDC* pdc = m_picVP.GetDC();
	for (int i = 0; i != vp.size(); ++i) {
		CRect ort(x, i,  cx*vp[i], i+1);
		pdc->FillSolidRect(ort, RGB(0, 0, 255));
		//vps_.push_back(ort);
	}
	m_picVP.ReleaseDC(pdc);

	return 0;
}


// recognition
void Cdraw_testDlg::OnBnClickedBtRecog()
{
	CDC *dc = m_picDraw.GetDC();

	//CRect rt;
	//m_picDraw.GetClientRect(rt);
	//rt.right -= rt.Width() % 4;
	//rt.bottom -= rt.Height() % 4;

	//auto bmp1 = get_bmp(dc->m_hDC, rt.Width(), rt.Height());

	// recognition
#if 1
	if (!g_bnn)
	{
		CRect rt;
		m_picDraw.GetClientRect(rt);
		rt.right -= rt.Width() % 4;
		rt.bottom -= rt.Height() % 4;

		auto bmp1 = get_bmp(dc->m_hDC, rt.Width(), rt.Height());

		imgRECT s_rt(0, 0, rt.Width()-(rt.Width()%4), rt.Height()-(rt.Height()%4));
		imgRECT d_rt(0, 0, SAMPLE_W, SAMPLE_H);

		// remove boundary
		ch_vec bin2(s_rt.cx * s_rt.cy, 0);
		for (int y = 5; y < s_rt.cy - 5; ++y) {
			for (int x = 5; x < s_rt.cx -5; ++x) {
				bin2[y*s_rt.cx + x] = bmp1[y*s_rt.cx + x] < 128 ? 255 : 0;
			}
		}

		//saveBMP("bin2.bmp", bin2.data(), s_rt.cx, s_rt.cy, 1);

		//rt_vec	segs1;
		//segmentation(bin2, s_rt, segs1);
		// segmenation of projection
		int_vec hp, vp;
		rt_vec segs1 = seg_projection(bin2, s_rt.cx, s_rt.cy, hp, vp);

		// draw projection histogram
		draw_HP(hp, rt);
		draw_VP(vp, rt);

		//if (segs1.size() != 1)
		//	return;
		// support multi recognition
		int idx_segs = 0;
		int disp_gap = 65;
		for (auto seg1 : segs1) {

			//imgRECT seg_rt(0, 0, segs[0].cx, segs[0].cy);
			//ch_vec seg(seg_rt.cx * seg_rt.cy);
			//seg_copy(bin2, s_rt, seg, segs[0]);

			//ch_vec gray(d_rt.cx* d_rt.cy);
			//resize(seg, seg_rt, gray, d_rt);

			//imgRECT s_rt(0, 0, rt.Width(), rt.Height());
			//imgRECT d_rt(0, 0, SAMPLE_W, SAMPLE_H);

			//imgRECT seg1 = segs1[0];
			// adapt seg size
		//	seg1.cx = std::min(seg1.cx, s_rt.cx);
		//	seg1.cy = std::min(seg1.cy, s_rt.cy);
			adapt_seg(seg1);
			ch_vec grab(seg1.cx * seg1.cy);
			seg_copy(bin2, s_rt, grab, seg1);

			//saveBMP("a1.bmp", bin2.data(), s_rt.cx, s_rt.cy, 1);
			//saveBMP("a2.bmp", grab.data(), seg1.cx, seg1.cy, 1);

			ch_vec gray(d_rt.cx * d_rt.cy);

			seg1.x = seg1.y = 0;
			//	zs::thin(grab, seg1, seg1, false, false, true);
			resize(grab, seg1, gray, d_rt);
			zs::thin(gray, d_rt, d_rt, false, false, true);

			saveBMP("a3.bmp", grab.data(), seg1.cx, seg1.cy, 1);
			saveBMP("a4.bmp", gray.data(), d_rt.cx, d_rt.cy, 1);

			ch_vec bin(gray);

			// remove boundary
			//ch_vec bin(d_rt.cx* d_rt.cy, 0);
			//for (int y = 1; y < d_rt.cy - 1; ++y) {
			//	for (int x = 1; x < d_rt.cx - 1; ++x) {
			//		bin[y*d_rt.cx + x] = gray[y*d_rt.cx + x] < 128 ? 255 : 0;
			//	}
			//}
			//for (int i = 0; i < bin.size(); ++i) {
			//	bin[i] = gray[i] < 128 ? 255 : 0;
			//}


			ch_vec sig(bin);


			//	fl_vec cvt(sig.begin(), sig.end()), dwt(sig.begin(), sig.end());
			//	//haar_dwt(cvt, d_rt.cx, d_rt.cy, dwt, 1);
			//
			//	//saveBMP("resize.bmp", bin.data(), d_rt.cx, d_rt.cy, 1);
			//
			//	//std::for_each(dwt.begin(), dwt.end(), [](float &fl) { fl = fl<255 ? 0 : 1; });
			//	dt(dwt, d_rt.cx, d_rt.cy);
			//	
			//	auto r = min_dist_classify3(sigs_, dwt, sig2s_, d_rt);

			std::vector<std::pair<int, double>> r;
			if (g_type == 1) {
				// @todo lbp
				vImg lbp;
				make_lbp(sig, d_rt.cx, d_rt.cy, lbp);
				//saveBMP("lbp.bmp", lbp.data(), 32, 32, 1);
				vHist hist = lbp_hist(lbp, d_rt.cx, d_rt.cy);
				r = min_dist_classify4(sigs_, hist);
			}
			else if (g_type == 2) {
				//vImg blur(sig.size(), 0);
				//gaussian(sig, d_rt, blur);
				descriptor_t d = make_orb(sig, d_rt.cx, d_rt.cy);

				//vImg big(128 * 128, 0);
				//imgRECT big_rt(0, 0, 128, 128);
				//resize(sig, d_rt, big, big_rt);
				//descriptor_t d = make_orb(big, big_rt.cx, big_rt.cy);

				if (d.size() == 0) {
					MessageBox(_T("Failed Feature Extraction"), _T("Error"), MB_OK);
					m_picDraw.ReleaseDC(dc);
					return;
				}

				r = min_hd(ds_, d);
				//int_vec di(d.begin(), d.end());
				//r = min_dist_5(sigs_, di);
			}
			else if (g_type == 3) {
				vImg big(128 * 128, 0);
				imgRECT big_rt(0, 0, 128, 128);
				resize(sig, d_rt, big, big_rt);

				float k = 0.25; int fr = 3; int maximaSuppresstionDimention = 10;// 10;
				image_t harris = make_harris(big, big_rt.cx, big_rt.cy, k, fr, true);
				vPtd kps = maxima_suppresss(harris, nullptr, false, 1, fr, maximaSuppresstionDimention);
				auto d = ptd_to_brief(big, big_rt.cx, big_rt.cy, kps, "brief_n.bmp");

				if (d.size() == 0) {
					MessageBox(_T("Failed Feature Extraction"), _T("Error"), MB_OK);
					m_picDraw.ReleaseDC(dc);
					return;
				}

				r = min_hd(ds_, d);
				//int_vec di(d.begin(), d.end());
				//r = min_dist_5(sigs_, di);
			}
			else if (g_type == 4) {
				vImg big(128 * 128, 0);
				imgRECT big_rt(0, 0, 128, 128);
				resize(sig, d_rt, big, big_rt);

				vPtd kps = SIFT::make_sift(sig, d_rt.cx, d_rt.cy, 3);
				//vPtd kps = SIFT::make_sift(big, big_rt.cx, big_rt.cy, 3);

				auto d = ptd_to_brief(sig, d_rt.cx*4, d_rt.cy*4, kps, "brief_n.bmp");
				//auto d = ptd_to_brief(big, big_rt.cx*8, big_rt.cy*8, kps, "brief_n.bmp");

				if (d.size() == 0) {
					MessageBox(_T("Failed Feature Extraction"), _T("Error"), MB_OK);
					m_picDraw.ReleaseDC(dc);
					return;
				}

				r = min_hd(ds_, d);
			}
			else {
				// @todo hog
				vHist hog;
				make_hog(sig, d_rt.cx, d_rt.cy, hog, 8, 8);
				r = min_dist_classify4(sigs_, hog);
			}

			// calc disp pos
			int y = 10 + idx_segs * disp_gap;
			++idx_segs;

			CClientDC dc(&m_pic_result);
			ResultOut(dc.m_hDC, r, 10, y);
		}
	}
#endif
	else
	// recognition
	{	
		// 이미지 획득이 왜 달라지지? 
		CRect rt;
		m_picDraw.GetClientRect(rt);
		//rt.right -= rt.Width() % 4;
		//rt.bottom -= rt.Height() % 4;

		auto bmp1 = get_bmp(dc->m_hDC, rt.Width(), rt.Height());


		// for predict 
		// resize target
		imgRECT s_rt(0, 0, rt.Width(), rt.Height());
		imgRECT d_rt(0, 0, SAMPLE_W, SAMPLE_H);

		ch_vec resize_img(32 * 32);
		imgRECT rt_img{ 0, 0, 32, 32 };

		// remove boundary
		ch_vec bin2(s_rt.cx* s_rt.cy, 0);
		for (int y = 5; y < s_rt.cy - 5; ++y) {
			for (int x = 5; x < s_rt.cx - 5; ++x) {
				bin2[y * s_rt.cx + x] = bmp1[y * s_rt.cx + x] < 128 ? 255 : 0;
			}
		}

		//rt_vec	segs1;
		//segmentation(bin2, s_rt, segs1);
		// segmenation of projection
		int_vec hp, vp;
		rt_vec segs1 = seg_projection(bin2, s_rt.cx, s_rt.cy, hp, vp);
		
		// draw projection histogram
		draw_HP(hp, rt);
		draw_VP(vp, rt);

		if (segs1.size() != 1)
			return;

		imgRECT seg1 = segs1[0];
		// adapt seg size
		seg1.cx = std::min(seg1.cx, s_rt.cx);
		seg1.cy = std::min(seg1.cy, s_rt.cy);
		adapt_seg(seg1);
		ch_vec grab(seg1.cx* seg1.cy);
		seg_copy(bin2, s_rt, grab, seg1);
		seg1.x = seg1.y = 0;
		resize(grab, seg1, resize_img, rt_img);
		//zs::thin(resize_img, rt_img, rt_img, false, false, true);

		//saveBMP("a1.bmp", bmp1.data(), s_rt.cx, s_rt.cy, 1);
		//saveBMP("a2.bmp", grab.data(), seg1.cx, seg1.cy, 1);
		saveBMP("a3.bmp", resize_img.data(), rt_img.cx, rt_img.cy, 1);

	//	// remove boundary
	//	ch_vec bin(d_rt.cx* d_rt.cy, 0);
	//	for (int y = 1; y < d_rt.cy - 1; ++y) {
	//		for (int x = 1; x < d_rt.cx - 1; ++x) {
	//			bin[y*d_rt.cx + x] = resize_img[y*d_rt.cx + x] < 128 ? 255 : 0;
	//		}
	//	}

	//	saveBMP("bin.bmp", bin.data(), 32, 32, 1);

	//	int w, h;
	//	loadBMP("1.bmp", bin, w, h);

		vec_t in_img5;
		parse_image(resize_img, rt_img.cy, rt_img.cx, -1.0, 1.0, 0, 0, in_img5);
	//	vec_t in_img5(bin.begin(), bin.end());

		// predict
		vec_t &out_vec = nn.predict(in_img5);
	//	auto digit = std::max_element(out_vec.begin(), out_vec.end());
	//	int r = std::distance(out_vec.begin(), digit);
			   
		std::unordered_map<int, double> dists;
		int i = 0;
		for (auto &e : out_vec) {
			dists[i++] = e;
		}

		std::vector<std::pair<int, double>> top_3(10);
		std::partial_sort_copy(dists.begin(),
			dists.end(),
			top_3.begin(),
			top_3.end(),
			[](std::pair<int, double> const& l,
				std::pair<int, double> const& r)
		{
			return l.second > r.second;
		});

		CClientDC dc(&m_pic_result);
		ResultOut(dc.m_hDC, top_3);

	}

	m_picDraw.ReleaseDC(dc);
}

// load number
void Cdraw_testDlg::OnBnClickedBtLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (g_bnn) return;
	
	sample1_convnet();
	g_bnn = true;
}

// close
void Cdraw_testDlg::OnBnClickedBtClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

int Cdraw_testDlg::load_number() {

	sigs_.clear();
	ds_.clear();


	ch_vec src;
	int w = 0, h = 0;
	loadBMP("./sample image/number1.bmp", src, w, h);
	size_t rs = w * h;

	//saveBMP("data.bmp", src.data(), w, h, 3);

	ch_vec gray(rs);
	to_grayscale(src, gray);

	imgRECT rect(0, 0, w, h);

	//ch_vec can(rs);
	//gaussian(eq, rect, can);
	//canny2(gray, rect, can, 179, 89.5);

	ch_vec mor(rs), mor2(rs);
	imgRECT rt;
	//fill_hole(can, rect, mor2, rt);
	//open2close(gray, rect, mor, rect);

	ch_vec bin(gray);
	for (int i = 0; i < rs; ++i) {
		bin[i] = gray[i] < 128 ? 255 : 0;
	}

	// 이전버전 세그멘테이션
//	rt_vec	segs;
//	//{
//	segmentation(bin, rect, segs);
	
	// segmentation of projection
	int_vec hp, vp;
	rt_vec segs = seg_projection(bin, rect.cx, rect.cy, hp, vp);

	// segs to regulation
	//int rw = SAMPLE_W, rh = SAMPLE_H;
	//for (auto &e : segs) {
	//	//int gx = (rw - e.cx) / 2;
	//	//int gy = (rh - e.cy) / 2;
	//	//e.x -= gx; e.cx = rw;
	//	//e.y -= gy; e.cy = rh;

	//	
	//	zs::thin(bin, rect, e, false, false, false);
	//}

	//CClientDC dc(&m_picDraw);
	//draw_seg(dc.m_hDC, segs);

	//{
	//	// dc draw 32bit bitmap only
	//	ch_vec draw(w*h * 4);
	//	for (int i = 0, j = 0; i < draw.size(); i += 4, j += 1) {
	//		draw[i] = bin[j];
	//		draw[i + 1] = bin[j];
	//		draw[i + 2] = bin[j];
	//	}

	//	CClientDC dc(this);
	//	draw_bmp(dc.m_hDC, 0, 0, w, h, 4, draw.data());
	//}

	//CClientDC dc(this);
	//draw_seg(dc.m_hDC, segs);
//}

	//saveBMP("a_bin.bmp", bin.data(), rect.cx, rect.cy, 1);
	int num = 0;
	for (const auto &seg : segs) {
		CString csfn; csfn.Format(_T("brief_%d.bmp"), num++);
		_bstr_t bsfn((LPCTSTR)csfn);
		const char* fn = (char*)bsfn;
		
		//ch_vec sig;
		//signature3(bin, rect, seg, sig);

		// adapt seg size
		imgRECT seg1 = seg;
		adapt_seg(seg1);

		ch_vec sig(seg1.cx * seg1.cy);
		seg_copy(bin, rect, sig, seg1);

		ch_vec sig2(SAMPLE_W*SAMPLE_H);
		imgRECT sig_rt(0, 0, seg1.cx, seg1.cy);
		imgRECT sig2_rt(0, 0, SAMPLE_W, SAMPLE_H);
		
	//	zs::thin(sig, sig_rt, sig_rt, false, false, true);
		resize(sig, sig_rt, sig2, sig2_rt);
		zs::thin(sig2, sig2_rt, sig2_rt, false, false, true);

	//	saveBMP("a_seg.bmp", sig2.data(), sig2_rt.cx, sig2_rt.cy, 1);
		
	//	fl_vec cvt(sig2.begin(), sig2.end()), dwt(sig2.begin(), sig2.end());
		//haar_dwt(cvt, seg.cx, seg.cy, dwt, 1);

		//ch_vec cdwt(dwt.begin(), dwt.end());
		//saveBMP("dwt.bmp", cdwt.data(), seg.cx, seg.cy, 1);

		//std::for_each(dwt.begin(), dwt.end(), [](float &fl) { fl = fl < 255 ? 0 : 1; });
	//	dt(dwt, sig2_rt.cx, sig2_rt.cy);
		
	//	sigs_.push_back(dwt);
		
		if (g_type == 1) {
				vImg lbp;
				make_lbp(sig2, sig2_rt.cx, sig2_rt.cy, lbp);
				vHist hist = lbp_hist(lbp, sig2_rt.cx, sig2_rt.cy);
				sigs_.push_back(hist);
		}
		else if (g_type == 2) {
			vImg blur(sig2.size(), 0);
			//gaussian(sig2, sig2_rt, blur);
			descriptor_t d = make_orb(sig2, sig2_rt.cx, sig2_rt.cy);
			
			//vImg big(128 * 128, 0);
			//imgRECT big_rt(0, 0, 128, 128);
			//resize(sig2, sig2_rt, big, big_rt);
			//descriptor_t d = make_orb(big, big_rt.cx, big_rt.cy);
			
			if (d.size() == 0) {
				MessageBox(_T("Failed Feature Extraction"), _T("Error"), MB_OK);
				return -1;
			}

			ds_.push_back(d);
			//int_vec di(d.begin(), d.end());
			//Wsigs_.push_back(di);
		}
		else if (g_type == 3) {
			vImg big(128 * 128, 0);
			imgRECT big_rt(0, 0, 128, 128);
			resize(sig2, sig2_rt, big, big_rt);

			float k = 0.25; int fr = 3; int maximaSuppresstionDimention = 10;//10;
			image_t harris = make_harris(big, big_rt.cx, big_rt.cy, k, fr, true);
			vPtd kps = maxima_suppresss(harris, nullptr, false, 1, fr, maximaSuppresstionDimention);
			auto d = ptd_to_brief(big, big_rt.cx, big_rt.cy, kps, fn);

			if (d.size() == 0) {
				MessageBox(_T("Failed Feature Extraction"), _T("Error"), MB_OK);
				return -1;
			}

			ds_.push_back(d);
			//int_vec di(d.begin(), d.end());
			//sigs_.push_back(di);
		}
		else if (g_type == 4) {
			vImg big(128 * 128, 0);
			imgRECT big_rt(0, 0, 128, 128);
			resize(sig2, sig2_rt, big, big_rt);

			vPtd kps = SIFT::make_sift(sig2, sig2_rt.cx, sig2_rt.cy, 3);
			//vPtd kps = SIFT::make_sift(big, big_rt.cx, big_rt.cy, 3);

			auto d = ptd_to_brief(sig2, sig2_rt.cx * 4, sig2_rt.cy * 4, kps, fn);
			//auto d = ptd_to_brief(big, big_rt.cx * 8, big_rt.cy * 8, kps, fn);

			if (d.size() == 0) {
				MessageBox(_T("Failed Feature Extraction"), _T("Error"), MB_OK);
				return -1;
			}

			ds_.push_back(d);
		}
		else {
				vHist hog;
				make_hog(sig2, sig2_rt.cx, sig2_rt.cy, hog, 8, 8);
				sigs_.push_back(hog);
		}
	}
	sig2s_ = segs;

	return 0;
}

void Cdraw_testDlg::OnBnClickedBtLoad2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (g_bnn) return;
	g_type = 1;
	load_number();
}


void Cdraw_testDlg::OnBnClickedBtLoad3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (g_bnn) return;
	g_type = 0;
	load_number();
}


void Cdraw_testDlg::OnBnClickedBtLoad4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (g_bnn) return;
	g_type = 2;
	load_number();
}


void Cdraw_testDlg::OnBnClickedBtLoad5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (g_bnn) return;
	g_type = 3;
	load_number();
}


void Cdraw_testDlg::OnBnClickedBtLoad6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (g_bnn) return;
	g_type = 4;
	load_number();
}

INT raw2hdc(HDC hDC, const ch_vec& raw, CRect& rt) {
	CDC* dc = CDC::FromHandle(hDC);

	CDC dcMem;
	CBitmap bitmap;

	int cx = rt.Width(), cy = rt.Height();

	dcMem.CreateCompatibleDC(dc);
	//bitmap.CreateCompatibleBitmap(&dcMem, cx, cy);
	//CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);

	// 8bit -> 32bit
	int bp = 4;
	std::vector<unsigned char> rv2(cx * cy * bp, 255);
	for (int i = 0, j = 0; i != raw.size(); ++i, j += bp) {
		rv2[j] = raw[i];
		rv2[j + 1] = raw[i];
		rv2[j + 2] = raw[i];
		//rv2[j + 3] = 255;
	}

	bitmap.CreateBitmap(cx, cy, 1, 32, rv2.data());
	//bitmap.SetBitmapBits(cx * cy * bp, rv2.data());
	CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);

	//CImage oimg;
	//oimg.Attach(bitmap);
	//oimg.Save(_T(".\\test.bmp"));

	dc->BitBlt(0, 0, cx, cy, &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(pOldBitmap);
	dcMem.DeleteDC();

	return 0;
}

void Cdraw_testDlg::OnBnClickedBtSkew()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDC* dc = m_picDraw.GetDC();
	CRect rt;
	m_picDraw.GetClientRect(rt);
	//rt.right -= rt.Width() % 4;
	//rt.bottom -= rt.Height() % 4;

	auto bmp1 = get_bmp(dc->m_hDC, rt.Width(), rt.Height());

	// copy roi
	imgRECT bmp1_rt(0, 0, rt.Width(), rt.Height());
	imgRECT bmp2_rt(4, 4, bmp1_rt.cx - 8, bmp1_rt.cy - 8);
	ch_vec bmp2(bmp2_rt.cx * bmp2_rt.cy, 255), bmp3(bmp1.size(), 255);
	seg_copy(bmp1, bmp1_rt, bmp2, bmp2_rt);
	seg_paste(bmp3, bmp1_rt, bmp2, bmp2_rt);
	//saveBMP("temp1.bmp", bmp3.data(), bmp1_rt.cx, bmp1_rt.cy, 1);

	// detect skew
	double skew = detect_skew2(bmp3, bmp1_rt.cx, bmp1_rt.cy);

	// affine test
	ch_vec dst(bmp1.size(), 255);
	imgRECT bmp3_rt(0, 0, bmp1_rt.cx, bmp1_rt.cy);
	double x_rot = -skew, y_rot = skew;
	affine_skew(bmp3, bmp3_rt, dst, x_rot, y_rot);


	// draw affine
	raw2hdc(dc->m_hDC, dst, rt);
	//saveBMP("temp.bmp", dst.data(), bmp3_rt.cx, bmp3_rt.cy, 1);
	//CImage oimg; oimg.Load(_T("temp.bmp"));
	//CRect rt2(2,2,rt.right-2, rt.bottom-2);
	//oimg.Draw(dc->m_hDC, rt2);

	m_picDraw.ReleaseDC(dc);
}
