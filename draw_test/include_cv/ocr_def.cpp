/// @file	ocr_def.cpp
/// @brief	ocr_def.h impl
/// @author	locketk9@gmail.com

#include "stdafx.h"

#include "ocr.h"
#include "ocr_def.h"

/// @brief	imgRECT == imgRECT
bool operator == (const imgRECT &lhs, const imgRECT &rhs)
{
	return (lhs.x==rhs.x&&lhs.y==rhs.y&&lhs.cx==rhs.cx&&lhs.cy==rhs.cy)
		? true : false ;
}
/// @brief imgRECT < imgRECT
bool operator < (const imgRECT &lhs, const imgRECT &rhs)
{
	if (lhs.x < rhs.x && lhs.y < rhs.y)
		return true;
	if (lhs.x+lhs.cx < rhs.x+rhs.cx 
		|| lhs.y+lhs.cy < rhs.y+rhs.cy)
		return true;

	return false;
}
