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
// �ʱ�ȭ ���� �Լ�

/* ��Ʈ��ũ �ʱ�ȭ */
void	CBPNET::BP_INIT_NET(int nInputs, int nSize, int nNeurons, int nOutputs)
{
	// ���� �޸� ����
	BP_FREE_NET();

	m_nInputs	= nInputs;
	m_nSize		= nSize;
	m_nNeurons	= nNeurons;
	m_nOutputs	= nOutputs;

	/////////////////////////////////////
	// �޸� ���� Ȯ��

	int n, p, m;

	// 1. �Է���
	x = new double* [ m_nInputs ];

	for( n = 0; n < m_nInputs; n++)		x[n] = new double [ m_nSize ];

	// 1-1. ��ǥġ
	t = new double* [ m_nInputs ];

	for( n = 0; n < m_nInputs; n++)		t[n] = new double [ m_nOutputs ];

	// 3. �Է��� - ������ �� ���ᰭ��
	v = new double * [ m_nNeurons ];

	for( p = 0;p < m_nNeurons; p++)		v[p] = new double [ m_nSize + 1/*bias*/ ];

	// 5. ������-��°� ���ᰭ��
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
			// -0.5 ~ 0.5 ������ ���� ���Ƿ� ����
			val = (double)(rand() % 101) / 100.f - .5f;

			v[p][n] = val;
		}

		val = (double)(rand() % 101) / 100.f - .5f;

		v[p][m_nSize] = val;		// bias
	}

	// 3-1. ��Ÿ �� ���
	// beta = 0.7 x n��p ( n: �Է��� ���� ��, p: ������ ���� �� )
	//      = 0.7 x pow( p, 1/n )
	double beta = .7f * pow( (double)m_nNeurons, 1.f/(double)m_nInputs );

	// 3-2. ��Ÿ�� �̿� �ʱⰪ ����
	//      v_new = beta x v_old / || v_old ||
	double norm = 0.f;

	for( p = 0;p < m_nNeurons; p++)
	{
		// ���� �� ���
		for( n = 0; n < m_nSize + 1; n++ )		norm += v[p][n] * v[p][n];
	}

	norm = sqrt( norm );

	for( p = 0;p < m_nNeurons; p++)
	{
		// �� weight ����
		for( n = 0; n < m_nSize; n++ )			v[p][n] = beta * v[p][n] / norm;

		// ���̾�� -beta ~ beta ���� ������ ����
		double val = (double)(rand() % 101) / 50.f - 1.f;

		v[p][m_nSize] = beta * val;
	}

	for(m=0; m < m_nOutputs; m++)
	{
		double val;

		for( p = 0; p < m_nNeurons; p++ )
		{
			// -0.5 ~ 0.5 ������ ���� ����
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
// ���� �н� �Լ�

/* BP �˰��� */
bool	CBPNET::BP_LEARNING(double alpha, double Emax, int Cmax, double* Efinal, int* Cfinal )
{
	return BP_LEARNING( alpha, 0, Emax, Cmax, Efinal, Cfinal );
}

/* ����� BP �˰��� */
bool	CBPNET::BP_LEARNING(double alpha, double beta, double Emax, int Cmax, double* Efinal, int* Cfinal )
{
	bool bSuccess = true; //false;

	int n, p, m;

	double error, E;

	//////////////////////////////////////////////
	// �ӽ� �迭 ����
	double*	y  = new double [ m_nOutputs ];		// �����
	double*	z  = new double [ m_nNeurons ];		// ������
	
	double* dy = new double [ m_nOutputs ];		// ����� ������ȣ
	double* dz = new double [ m_nNeurons ];		// ������ ������ȣ

	double** dw = new double * [ m_nOutputs ];	// ����� ��ȭ��
	for( m = 0; m < m_nOutputs; m++ )
	{
		dw[m] = new double [ m_nNeurons + 1 ];

		for( p = 0; p < m_nNeurons + 1; p++ )
		{
			dw[m][p] = 0.f;
		}
	}
	double** dv = new double * [ m_nNeurons ];	// ������ ��ȭ��
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
	// �н� ����

	for( cnt = 0; cnt < Cmax; cnt++ )
	{
		error = 0.f;

		for( n = 0; n < m_nInputs; n++ )
		{
			// a. NETz�� ���
			for( p=0; p < m_nNeurons; p++ )
			{
				double NETz = 0.f;
				
				for( int s = 0; s < m_nSize; s++ )		NETz += x[n][s] * v[p][s];

				NETz += 1.f * v[p][m_nSize];			// bias


				// f(NETz)
				z[p] = 1.f / (1.f + exp(-NETz) );		// �ܱؼ� �ñ׸��̵� Ȱ��ȭ�Լ�
			}

			// b. NETy�� ���
			for( m=0; m < m_nOutputs; m++ )
			{
				double NETy = 0.f;

				for( int i=0; i < m_nNeurons; i++ )		NETy += z[i] * w[m][i];

				NETy += 1.f * w[m][m_nNeurons];			// bias

				// f(NETy)
				y[m] = 1.f / (1.f + exp(-NETy) );		// �ܱؼ� �ñ׸��̵� Ȱ��ȭ�Լ�
			}

			// c. ���������� ���
			for( m=0; m < m_nOutputs; m++ )
			{
				double d = t[n][m];		// ��ǥġ

				E = .5 * ( d - y[m] ) * ( d - y[m] );

				error += E;
			}

			// d. ����� ������ȣ ���
			for( m=0; m < m_nOutputs; m++ )
			{
				double d = t[n][m];		// ��ǥġ

				dy[m] = (d - y[m]) * y[m] * (1.f - y[m]);				// �ܱؼ�
			}

			// e. ������ ������ȣ ���
			for( m=0; m < m_nOutputs; m++ )
			{
				for( p=0; p < m_nNeurons; p++ )
				{
					dz[p] = z[p] * ( 1.f - z[p] ) * dy[m] * w[m][p];	// �ܱؼ�
				}
			}

			// f. ���ᰭ�� ��ȭ�� ���
			for( m=0; m < m_nOutputs; m++ )
			{					
				for( p=0; p < m_nNeurons; p++ )
				{
//					dw[m][p] = alpha * dy[m] * z[p];						// BP �˰���
					dw[m][p] = alpha * dy[m] * z[p] + beta * dw[m][p];		// ����� BP�˰���
				}

//				dw[m][m_nNeurons] = alpha * dy[m] * 1.f;
				dw[m][m_nNeurons] = alpha * dy[m] * 1.f + beta * dw[m][m_nNeurons];
			}
			
			// g. ���ᰭ�� ��ȭ�� ���
			for( p = 0;p < m_nNeurons; p++)
			{
				for( int s = 0; s < m_nSize; s++ )
				{
//					dv[p][s] = alpha * dz[p] * x[n][s];						// BP �˰���
					dv[p][s] = alpha * dz[p] * x[n][s] + beta * dv[p][s];	// ����� BP �˰���
				}

//				dv[p][m_nSize] = alpha * dz[p] * 1.f;
				dv[p][m_nSize] = alpha * dz[p] * 1.f + beta * dv[p][m_nSize];
			}

			// h. w ���ᰭ�� ����
			for( m=0; m < m_nOutputs; m++ )
			{
				for( p = 0; p < m_nNeurons;  p++ )		w[m][p] += dw[m][p];

				w[m][m_nNeurons] += dw[m][m_nNeurons];
			}

			// i. v ���� ���� ����
			for( p = 0; p < m_nNeurons; p++ )
			{
				for(int s = 0; s < m_nSize; s++ )		v[p][s] += dv[p][s];

				v[p][m_nSize] += dv[p][m_nSize];
			}
		}

		printf("ERR : %.2f...\n", error);

		// j. ��� ������ �ִ� ��� �������� ���� ��� �н� ����
		if( error < Emax )	{	bSuccess = true;	break;	}
	}

	// �޸� ����
	for( m = 0; m < m_nOutputs; m++ )	delete dw[m];
	for( p = 0;p < m_nNeurons; p++)		delete dv[p];

	// ���ϰ� ��ȯ
	*Efinal = error;
	*Cfinal = cnt;

	// ����
	return bSuccess;
}

/////////////////////////////////////////////////////////////////
// ���� ����� ���� �Լ�

/* ���� �ҷ����� */
void	CBPNET::BP_LOAD_NET(char* szFilePath)
{
}

/* ���� �����ϱ� */
void	CBPNET::BP_SAVE_NET(char* szFilePath)
{
}

/////////////////////////////////////////////////////////////////
// �ν� ��� ���� �Լ�

void	CBPNET::BP_RECOGNIZE(double* vector, double* result)
{
	int p, m;

	double*	y  = new double [ m_nOutputs ];		// �����
	double*	z  = new double [ m_nNeurons ];		// ������
	
	// a. NETz�� ���
	for( p=0; p < m_nNeurons; p++ )
	{
		double NETz = 0.f;
		
		for( int s = 0; s < m_nSize; s++ )		NETz += vector[s] * v[p][s];

		NETz += 1.f * v[p][m_nSize];			// bias

		// f(NETz)
		z[p] = 1.f / (1.f + exp(-NETz) );		// �ܱؼ� �ñ׸��̵� Ȱ��ȭ�Լ�
	}

	// b. NETy�� ���
	for( m=0; m < m_nOutputs; m++ )
	{
		double NETy = 0.f;

		for( int i=0; i < m_nNeurons; i++ )		NETy += z[i] * w[m][i];

		NETy += 1.f * w[m][m_nNeurons];			// bias

		// f(NETy)
		y[m] = 1.f / (1.f + exp(-NETy) );		// �ܱؼ� �ñ׸��̵� Ȱ��ȭ�Լ�
	}

	memcpy(result, y, m_nOutputs * sizeof(double));
}