// bp.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "BPNET.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CBPNET		bp;

	// 1. BP 네트워크 초기화
/*	bp.BP_INIT_NET( int nInputs,	: 입력 벡터의 수
					int nSize,		: 입력 벡터의 크기
					int nNeurons,	: 은닉층 뉴런 수 : 은닉층의 뉴런 수 p는 일반적으로 log2(M)로 한다. 여기서 M은 선형분리 가능한 영역의 개수
					int nOutputs);	: 출력 수 */
	
	bp.BP_INIT_NET( 3, 9, 3, 2 );		// ㄱ, ㄴ, ㄷ 예제
//	bp.BP_INIT_NET( 4, 2, 2, 1 );		// XOR 예제

	// 2. 입력 데이터 설정
/**/
	double x[3][9] = {	{1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f},
						{1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 1.f},
						{1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 1.f} };

	int n, p, m;

	for(n=0;n<3; n++)
	for(p=0;p<9; p++)
	{
		bp.x[n][p] = x[n][p];
	}
/**
	bp.x[0][0] = 0.f;	bp.x[0][1] = 0.f;		// XOR Data
	bp.x[1][0] = 0.f;	bp.x[1][1] = 1.f;
	bp.x[2][0] = 1.f;	bp.x[2][1] = 0.f;
	bp.x[3][0] = 1.f;	bp.x[3][1] = 1.f;

	// 3. 목표치 입력 t;
/**/
	bp.t[0][0] = 0.f;	bp.t[0][1] = 0.f;
	bp.t[1][0] = 0.f;	bp.t[1][1] = 1.f;
	bp.t[2][0] = 1.f;	bp.t[2][1] = 0.f;
/**
	bp.t[0][0] = 0.f;		// XOR Data
	bp.t[1][0] = 1.f;
	bp.t[2][0] = 1.f;
	bp.t[3][0] = 0.f;
/**/

	// 4. 연결강도 초기화
	bp.BP_INIT_WEIGHT();

	// 5. 학습 시작
	
	///////////////////////////////////////////////////////////
	printf("START LEARNING...\n\n");
	///////////////////////////////////////////////////////////

	double	E = .0f;
	int		cnt = 0;

	while( !bp.BP_LEARNING(	5.f,	/* 학습률 : 일반적으로 0.001 ~ 10 사이의 값을 임의로 사용 */
							.5f,	/* 모멘텀 상수 : 일반적으로 0 ~ 0.8 사이의 값을 사용 */
							.01f,	/* 최대 출력 오차 */
							20000,	/* 최대 학습 회수 */
							&E,		/* 최종 출력 오차 */
							&cnt ))	/* 최종 학습 회수 */
	{
		printf("ERR : %.2f, CNT : %5d\n", E, cnt);
		
		// 연결강도 재설정
		bp.BP_INIT_WEIGHT();

	}

	printf("\nRESULT >>>\n\n");
	
	// 6. 최종 연결강도, 최종 출력 오차 화면에 출력
	for( p=0; p < bp.m_nNeurons; p++ )
	{
		if( p == 0 )	printf("v = [ ");
		else			printf("    [ ");

		for( int s=0; s < bp.m_nSize+1; s++ )
		{
			printf("%.2f ", bp.v[p][s]);
		}

		printf("]\n");
	}

	for( m=0; m < bp.m_nOutputs; m++ )
	{
		if( m == 0 )	printf("w = [ ");
		else			printf("    [ ");

		for( p=0; p < bp.m_nNeurons+1; p++ )
		{
			printf("%.2f ", bp.w[m][p]);
		}

		printf("]\n");
	}

	printf("\nERR(final) : %.2f, CNT(final) : %5d\n", E, cnt);
	
	char c;	
	scanf("%c", &c);

	printf("\n\nTEST >>>\n\n");

	double result[2];

	bp.BP_RECOGNIZE( x[0], result );	printf("[ %.2f %.2f ]\n", result[0], result[1]);
	bp.BP_RECOGNIZE( x[1], result );	printf("[ %.2f %.2f ]\n", result[0], result[1]);
	bp.BP_RECOGNIZE( x[2], result );	printf("[ %.2f %.2f ]\n", result[0], result[1]);

	scanf("%c", &c);

	printf("\nTEST2 >>>\n\n");

	// 원본 변형
	x[0][0] = 0.f;	x[0][8] = 0.f;
	x[1][5] = 1.f;	x[1][6] = 0.f;
	x[2][4] = 1.f;	x[2][8] = 0.f;

	bp.BP_RECOGNIZE( x[0], result );	printf("[ %.2f %.2f ]\n", result[0], result[1]);
	bp.BP_RECOGNIZE( x[1], result );	printf("[ %.2f %.2f ]\n", result[0], result[1]);
	bp.BP_RECOGNIZE( x[2], result );	printf("[ %.2f %.2f ]\n", result[0], result[1]);

	scanf("%c", &c);	

	return 0;
}

