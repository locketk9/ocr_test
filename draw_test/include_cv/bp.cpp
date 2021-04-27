// bp.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "BPNET.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CBPNET		bp;

	// 1. BP ��Ʈ��ũ �ʱ�ȭ
/*	bp.BP_INIT_NET( int nInputs,	: �Է� ������ ��
					int nSize,		: �Է� ������ ũ��
					int nNeurons,	: ������ ���� �� : �������� ���� �� p�� �Ϲ������� log2(M)�� �Ѵ�. ���⼭ M�� �����и� ������ ������ ����
					int nOutputs);	: ��� �� */
	
	bp.BP_INIT_NET( 3, 9, 3, 2 );		// ��, ��, �� ����
//	bp.BP_INIT_NET( 4, 2, 2, 1 );		// XOR ����

	// 2. �Է� ������ ����
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

	// 3. ��ǥġ �Է� t;
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

	// 4. ���ᰭ�� �ʱ�ȭ
	bp.BP_INIT_WEIGHT();

	// 5. �н� ����
	
	///////////////////////////////////////////////////////////
	printf("START LEARNING...\n\n");
	///////////////////////////////////////////////////////////

	double	E = .0f;
	int		cnt = 0;

	while( !bp.BP_LEARNING(	5.f,	/* �н��� : �Ϲ������� 0.001 ~ 10 ������ ���� ���Ƿ� ��� */
							.5f,	/* ����� ��� : �Ϲ������� 0 ~ 0.8 ������ ���� ��� */
							.01f,	/* �ִ� ��� ���� */
							20000,	/* �ִ� �н� ȸ�� */
							&E,		/* ���� ��� ���� */
							&cnt ))	/* ���� �н� ȸ�� */
	{
		printf("ERR : %.2f, CNT : %5d\n", E, cnt);
		
		// ���ᰭ�� �缳��
		bp.BP_INIT_WEIGHT();

	}

	printf("\nRESULT >>>\n\n");
	
	// 6. ���� ���ᰭ��, ���� ��� ���� ȭ�鿡 ���
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

	// ���� ����
	x[0][0] = 0.f;	x[0][8] = 0.f;
	x[1][5] = 1.f;	x[1][6] = 0.f;
	x[2][4] = 1.f;	x[2][8] = 0.f;

	bp.BP_RECOGNIZE( x[0], result );	printf("[ %.2f %.2f ]\n", result[0], result[1]);
	bp.BP_RECOGNIZE( x[1], result );	printf("[ %.2f %.2f ]\n", result[0], result[1]);
	bp.BP_RECOGNIZE( x[2], result );	printf("[ %.2f %.2f ]\n", result[0], result[1]);

	scanf("%c", &c);	

	return 0;
}

