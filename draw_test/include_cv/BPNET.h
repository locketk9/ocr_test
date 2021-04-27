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
	// �н��� �ʿ��� ���� ����

	double**	x;	// �Է���
	double**	t;	// ��ǥ

	double**	v;	// �Է���-������ �� ���ᰭ��
	double**	w;	// ������-����� �� ���ᰭ��

public:

	int		m_nInputs;
	int		m_nSize;
	int		m_nNeurons;
	int		m_nOutputs;

public:

	////////////////////////////////////////////
	// �н��� �ʿ��� �Լ� ����

	// ��Ʈ��ũ �ʱ�ȭ
	void	BP_INIT_NET(int nInputs, int nSize, int nNeurons, int nOutputs);
	void	BP_FREE_NET();

	// ���ᰭ�� �ʱ�ȭ
	void	BP_INIT_WEIGHT();
	
public:

	////////////////////////////////////////////
	// ���� �н� �Լ�

	/* BP �˰��� */
	bool	BP_LEARNING(double	alpha,	/* �н��� */
						double	Emax,	/* �ִ� ��� ���� */
						int		nCnt,	/* �ִ� �ݺ� Ƚ�� */
						double*	Efinal,	/* ���� ��� ���� */
						int*	Cfinal);/* ���� �ݺ� Ƚ�� */

	/* ����� BP �˰��� */
	bool	BP_LEARNING(double	alpha,	/* �н��� */
						double	beta,	/* ����� ��� */ 
						double	Emax,	/* �ִ� ��� ���� */
						int		Cmax,	/* �ִ� �ݺ� Ƚ�� */
						double*	Efinal,	/* ���� ��� ���� */
						int*	Cfinal);/* ���� �ݺ� Ƚ�� */

public:

	////////////////////////////////////////////
	// ���� ����� ���� �Լ�
	void	BP_LOAD_NET(char* szFilePath);
	void	BP_SAVE_NET(char* szFilePath);


	////////////////////////////////////////////
	// �ν� ��� ���� �Լ�
	void	BP_RECOGNIZE(double* vector, double* result);
};
