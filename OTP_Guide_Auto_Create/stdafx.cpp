#include "stdafx.h"

bool IsHex(const string& str)
{
	boost::regex reg("0x[0-9a-fA-F]{1, }");//ÅÐ¶Ï·Ç·¨µÄ×Ö·û
	boost::smatch m_mat;
	if (!boost::regex_match(str, m_mat, reg))
		return false;
	else
		return true;
}