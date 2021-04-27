#include "StdAfx.h"
#include "BPNET.h"

CBPNET::CBPNET(void)
{
	x = NULL;
	t = NULL;

	v = NULL;
	w = NULL;

	m_nInputs	= 0;
	m_nSize		= 0;
	m_nNeurons	= 0;
	m_nOutputs	= 0;
}

CBPNET::~CBPNET(void)
{
	BP_FREE_NET();
}

/////////////////////////////////////////////////////////////////
// 초기화 관련 함수

/* 네트워크 초기화 */
void	CBPNET::BP_INIT_NET(int nInputs, int nSize, int nNeurons, int nOutputs)
{
	// 기존 메모리 해제
	BP_FREE_NET();

	m_nInputs	= nInputs;
	m_nSize		= nSize;
	m_nNeurons	= nNeurons;
	m_nOutputs	= nOutputs;

	/////////////////////////////////////
	// 메모리 공간 확보

	int n, p, m;

	// 1. 입력층
	x = new double* [ m_nInputs ];

	for( n = 0; n < m_nInputs; n++)		x[n] = new double [ m_nSize ];

	// 1-1. 목표치
	t = new double* [ m_nInputs ];

	for( n = 0; n < m_nInputs; n++)		t[n] = new double [ m_nOutputs ];

	// 3. 입력층 - 은닉층 간 연결강도
	v = new double * [ m_nNeurons ];

	for( p = 0;p < m_nNeurons; p++)		v[p] = new double [ m_nSize + 1/*bias*/ ];

	// 5. 은닉층-출력간 연결강도
	w = new double * [ m_nOutputs ];

	for(m=0; m < m_nOutputs; m++)		w[m] = new double [ m_nNeurons + 1/*bias*/ ];
}

void	CBPNET::BP_INIT_WEIGHT()
{
	srand( (unsigned int)time(NULL) );

	int n, p, m;

	for( p = 0;p < m_nNeurons; p++)
	{
		double val;

		for( n = 0; n < m_nSize; n++ )
		{
			// -0.5 ~ 0.5 사이의 값을 임의로 생성
			val = (double)(rand() % 101) / 100.f - .5f;

			v[p][n] = val;
		}

		val = (double)(rand() % 101) / 100.f - .5f;

		v[p][m_nSize] = val;		// bias
	}

	// 3-1. 베타 값 계산
	// beta = 0.7 x n√p ( n: 입력층 뉴런 수, p: 은닉층 뉴런 수 )
	//      = 0.7 x pow( p, 1/n )
	double beta = .7f * pow( (double)m_nNeurons, 1.f/(double)m_nInputs );

	// 3-2. 베타값 이용 초기값 변형
	//      v_new = beta x v_old / || v_old ||
	double norm = 0.f;

	for( p = 0;p < m_nNeurons; p++)
	{
		// 벡터 놈 계산
		for( n = 0; n < m_nSize + 1; n++ )		norm += v[p][n] * v[p][n];
	}

	norm = sqrt( norm );

	for( p = 0;p < m_nNeurons; p++)
	{
		// 각 weight 변형
		for( n = 0; n < m_nSize; n++ )			v[p][n] = beta * v[p][n] / norm;

		// 바이어스는 -beta ~ beta 사이 값으로 설정
		double val = (double)(rand() % 101) / 50.f - 1.f;

		v[p][m_nSize] = beta * val;
	}

	for(m=0; m < m_nOutputs; m++)
	{
		double val;

		for( p = 0; p < m_nNeurons; p++ )
		{
			// -0.5 ~ 0.5 사이의 값을 생성
			val = (double)(rand() % 101) / 100.f - .5f;

			w[m][p] = val;
		}

		val = (double)(rand() % 101) / 100.f - .5f;

		w[m][m_nNeurons] = val;	// bias
	}
}

void	CBPNET::BP_FREE_NET()
{
	int n, p, m;

	for(n=0; n < m_nInputs;	 n++)		delete x[n];
	for(n=0; n < m_nInputs;	 n++)		delete t[n];

	for(p=0; p < m_nNeurons; p++)		delete v[p];
	for(m=0; m < m_nOutputs; m++)		delete w[m];	
}

/////////////////////////////////////////////////////////////////
// 실제 학습 함수

/* BP 알고리즘 */
bool	CBPNET::BP_LEARNING(double alpha, double Emax, int Cmax, double* Efinal, int* Cfinal )
{
	return BP_LEARNING( alpha, 0, Emax, Cmax, Efinal, Cfinal );
}

/* 모멘텀 BP 알고리즘 */
bool	CBPNET::BP_LEARNING(double alpha, double beta, double Emax, int Cmax, double* Efinal, int* Cfinal )
{
	bool bSuccess = true; //false;

	int n, p, m;

	double error, E;

	//////////////////////////////////////////////
	// 임시 배열 생성
	double*	y  = new double [ m_nOutputs ];		// 출력층
	double*	z  = new double [ m_nNeurons ];		// 은닉층
	
	double* dy = new double [ m_nOutputs ];		// 출력층 오차신호
	double* dz = new double [ m_nNeurons ];		// 은닉층 오차신호

	double** dw = new double * [ m_nOutputs ];	// 출력층 변화량
	for( m = 0; m < m_nOutputs; m++ )
	{
		dw[m] = new double [ m_nNeurons + 1 ];

		for( p = 0; p < m_nNeurons + 1; p++ )
		{
			dw[m][p] = 0.f;
		}
	}
	double** dv = new double * [ m_nNeurons ];	// 은닉층 변화량
	for( p = 0;p < m_nNeurons; p++)
	{
		dv[p] = new double [ m_nSize + 1 ];

		for( int s = 0; s < m_nSize + 1; s++ )
		{
			dv[p][s] = 0.f;
		}
	}

	int cnt = 0;

	//////////////////////////////////////////////
	// 학습 시작

	for( cnt = 0; cnt < Cmax; cnt++ )
	{
		error = 0.f;

		for( n = 0; n < m_nInputs; n++ )
		{
			// a. NETz의 계산
			for( p=0; p < m_nNeurons; p++ )
			{
				double NETz = 0.f;
				
				for( int s = 0; s < m_nSize; s++ )		NETz += x[n][s] * v[p][s];

				NETz += 1.f * v[p][m_nSize];			// bias


				// f(NETz)
				z[p] = 1.f / (1.f + exp(-NETz) );		// 단극성 시그모이드 활성화함수
			}

			// b. NETy의 계산
			for( m=0; m < m_nOutputs; m++ )
			{
				double NETy = 0.f;

				for( int i=0; i < m_nNeurons; i++ )		NETy += z[i] * w[m][i];

				NETy += 1.f * w[m][m_nNeurons];			// bias

				// f(NETy)
				y[m] = 1.f / (1.f + exp(-NETy) );		// 단극성 시그모이드 활성화함수
			}

			// c. 제곱오차의 계산
			for( m=0; m < m_nOutputs; m++ )
			{
				double d = t[n][m];		// 목표치

				E = .5 * ( d - y[m] ) * ( d - y[m] );

				error += E;
			}

			// d. 출력층 오차신호 계산
			for( m=0; m < m_nOutputs; m++ )
			{
				double d = t[n][m];		// 목표치

				dy[m] = (d - y[m]) * y[m] * (1.f - y[m]);				// 단극성
			}

			// e. 은닉층 오차신호 계산
			for( m=0; m < m_nOutputs; m++ )
			{
				for( p=0; p < m_nNeurons; p++ )
				{
					dz[p] = z[p] * ( 1.f - z[p] ) * dy[m] * w[m][p];	// 단극성
				}
			}

			// f. 연결강도 변화량 계산
			for( m=0; m < m_nOutputs; m++ )
			{					
				for( p=0; p < m_nNeurons; p++ )
				{
//					dw[m][p] = alpha * dy[m] * z[p];						// BP 알고리즘
					dw[m][p] = alpha * dy[m] * z[p] + beta * dw[m][p];		// 모멘텀 BP알고리즘
				}

//				dw[m][m_nNeurons] = alpha * dy[m] * 1.f;
				dw[m][m_nNeurons] = alpha * dy[m] * 1.f + beta * dw[m][m_nNeurons];
			}
			
			// g. 연결강도 변화량 계산
			for( p = 0;p < m_nNeurons; p++)
			{
				for( int s = 0; s < m_nSize; s++ )
				{
//					dv[p][s] = alpha * dz[p] * x[n][s];						// BP 알고리즘
					dv[p][s] = alpha * dz[p] * x[n][s] + beta * dv[p][s];	// 모멘텀 BP 알고리즘
				}

//				dv[p][m_nSize] = alpha * dz[p] * 1.f;
				dv[p][m_nSize] = alpha * dz[p] * 1.f + beta * dv[p][m_nSize];
			}

			// h. w 연결강도 변경
			for( m=0; m < m_nOutputs; m++ )
			{
				for( p = 0; p < m_nNeurons;  p++ )		w[m][p] += dw[m][p];

				w[m][m_nNeurons] += dw[m][m_nNeurons];
			}

			// i. v 연결 강도 변경
			for( p = 0; p < m_nNeurons; p++ )
			{
				for(int s = 0; s < m_nSize; s++ )		v[p][s] += dv[p][s];

				v[p][m_nSize] += dv[p][m_nSize];
			}
		}

		printf("ERR : %.2f...\n", error);

		// j. 출력 오차가 최대 출력 오차보다 적은 경우 학습 종료
		if( error < Emax )	{	bSuccess = true;	break;	}
	}

	// 메모리 해제
	for( m = 0; m < m_nOutputs; m++ )	delete dw[m];
	for( p = 0;p < m_nNeurons; p++)		delete dv[p];

	// 리턴값 반환
	*Efinal = error;
	*Cfinal = cnt;

	// 종료
	return bSuccess;
}

/////////////////////////////////////////////////////////////////
// 파일 입출력 관련 함수

/* 파일 불러오기 */
void	CBPNET::BP_LOAD_NET(char* szFilePath)
{
}

/* 파일 저장하기 */
void	CBPNET::BP_SAVE_NET(char* szFilePath)
{
}

/////////////////////////////////////////////////////////////////
// 인식 결과 도출 함수

void	CBPNET::BP_RECOGNIZE(double* vector, double* result)
{
	int p, m;

	double*	y  = new double [ m_nOutputs ];		// 출력층
	double*	z  = new double [ m_nNeurons ];		// 은닉층
	
	// a. NETz의 계산
	for( p=0; p < m_nNeurons; p++ )
	{
		double NETz = 0.f;
		
		for( int s = 0; s < m_nSize; s++ )		NETz += vector[s] * v[p][s];

		NETz += 1.f * v[p][m_nSize];			// bias

		// f(NETz)
		z[p] = 1.f / (1.f + exp(-NETz) );		// 단극성 시그모이드 활성화함수
	}

	// b. NETy의 계산
	for( m=0; m < m_nOutputs; m++ )
	{
		double NETy = 0.f;

		for( int i=0; i < m_nNeurons; i++ )		NETy += z[i] * w[m][i];

		NETy += 1.f * w[m][m_nNeurons];			// bias

		// f(NETy)
		y[m] = 1.f / (1.f + exp(-NETy) );		// 단극성 시그모이드 활성화함수
	}

	memcpy(result, y, m_nOutputs * sizeof(double));
}