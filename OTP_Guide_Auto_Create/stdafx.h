#pragma once
#include "OTPGuideDataStruct.h"
#include "Qt_Excel.h"
#include<QMessageBox>
#include <vector>
#include <string>
#include <QKeyEvent>
#include <QInputDialog>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include<boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>
#include <Windows.h>
//ss
//�ж��Ƿ���16���Ƶ��ַ���
bool IsHex(const string& str);
bool IsInteger(const string& str);
bool IsFrontBy(const string& src, const string& reg);

wstring string2wstring(string str);

//��wstringת����string  
string wstring2string(wstring wstr);