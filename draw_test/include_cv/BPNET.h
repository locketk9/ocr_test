#pragma once

#include "stdlib.h"
#include "time.h"

#include <math.h>

class CBPNET
{
public:
	CBPNET(void);
	~CBPNET(void);

public:
	////////////////////////////////////////////
	// 학습에 필요한 변수 설정

	double**	x;	// 입력층
	double**	t;	// 목표

	double**	v;	// 입력층-은닉층 간 연결강도
	double**	w;	// 은닉층-출력층 간 연결강도

public:

	int		m_nInputs;
	int		m_nSize;
	int		m_nNeurons;
	int		m_nOutputs;

public:

	////////////////////////////////////////////
	// 학습에 필요한 함수 설정

	// 네트워크 초기화
	void	BP_INIT_NET(int nInputs, int nSize, int nNeurons, int nOutputs);
	void	BP_FREE_NET();

	// 연결강도 초기화
	void	BP_INIT_WEIGHT();
	
public:

	////////////////////////////////////////////
	// 실제 학습 함수

	/* BP 알고리즘 */
	bool	BP_LEARNING(double	alpha,	/* 학습률 */
						double	Emax,	/* 최대 출력 오차 */
						int		nCnt,	/* 최대 반복 횟수 */
						double*	Efinal,	/* 최종 출력 오차 */
						int*	Cfinal);/* 최종 반복 횟수 */

	/* 모멘텀 BP 알고리즘 */
	bool	BP_LEARNING(double	alpha,	/* 학습률 */
						double	beta,	/* 모멘텀 상수 */ 
						double	Emax,	/* 최대 출력 오차 */
						int		Cmax,	/* 최대 반복 횟수 */
						double*	Efinal,	/* 최종 출력 오차 */
						int*	Cfinal);/* 최종 반복 횟수 */

public:

	////////////////////////////////////////////
	// 파일 입출력 관련 함수
	void	BP_LOAD_NET(char* szFilePath);
	void	BP_SAVE_NET(char* szFilePath);


	////////////////////////////////////////////
	// 인식 결과 도출 함수
	void	BP_RECOGNIZE(double* vector, double* result);
};
