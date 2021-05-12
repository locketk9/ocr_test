
// draw_testDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"

#include "./include_cv/draw_pic.h"
#include "./include_cv/ocr.h"
//#include "./include_cv/ocr_func.h"
#include "./include_cv/ocr_def.h"


 descriptor_t;

// Cdraw_testDlg 대화 상자
class Cdraw_testDlg : public CDialogEx
{
// 생성입니다.
public:
	Cdraw_testDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DRAW_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


	curve::vertex vts_;
	curve::CDrawCurve curve_;
	bool drawing_ = false;
	curve::Point pt_prev_;

	//sigf_vec sigs_;
	sigi_vec sigs_;
	rt_vec		sig2s_;
	std::vector<descriptor_t> ds_;

	int load_number();
	void sample1_convnet(void);

	std::vector<CRect> hps_;
	std::vector<CRect> vps_;
	int draw_HP(const int_vec &hp, const CRect& draw);
	int draw_VP(const int_vec& vp, const CRect& draw);

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_picDraw;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtClear();
	afx_msg void OnBnClickedBtRecog();
	afx_msg void OnBnClickedBtLoad();
	CStatic m_pic_result;
	afx_msg void OnBnClickedBtClose();
	CProgressCtrl m_prog_load;
	afx_msg void OnBnClickedBtLoad2();
	afx_msg void OnBnClickedBtLoad3();
	afx_msg void OnBnClickedBtLoad4();
	afx_msg void OnBnClickedBtLoad5();
	afx_msg void OnBnClickedBtLoad6();
	CStatic m_picVP;
	CStatic m_picHP;
};
