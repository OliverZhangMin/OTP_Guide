#include "stdafx.h"

bool IsHex(const string& str)
{
	boost::regex reg("0x[0-9a-fA-F]{1, }");//�жϷǷ����ַ�
	boost::smatch m_mat;
	if (!boost::regex_match(str, m_mat, reg))
		return false;
	else
		return true;
}

bool IsInteger(const string& str)
{
	boost::regex reg("[^0123456789]");//�жϷǷ����ַ�
	boost::sregex_iterator it_begin(str.begin(), str.end(), reg);
	boost::sregex_iterator it_end;
	for (auto it = it_begin; it != it_end; it++)
		return false;

	return true;
}

bool IsFrontBy(const string& src, const string& reg)
{
	if (boost::starts_with(src, reg))
		return true;
	else
		return false;
}

wstring string2wstring(string str)
{
	wstring result;
	//��ȡ��������С��������ռ䣬��������С���ַ�����  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	WCHAR* buffer = new WCHAR[len + 1];
	//���ֽڱ���ת���ɿ��ֽڱ���  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //����ַ�����β  
									//ɾ��������������ֵ  
	result.append(buffer);
	delete[] buffer;
	return result;
}

//��wstringת����string  
string wstring2string(wstring wstr)
{
	string result;
	//��ȡ��������С��������ռ䣬��������С�°��ֽڼ����  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//���ֽڱ���ת���ɶ��ֽڱ���  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//ɾ��������������ֵ  
	result.append(buffer);
	delete[] buffer;
	return result;
}