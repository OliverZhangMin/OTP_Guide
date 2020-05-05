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
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>
#include <json.h>
#include <QTextEdit>
//ss
//判断是否是16进制的字符串
bool IsHex(const string& str);
bool IsInteger(const string& str);
bool IsFrontBy(const string& src, const string& reg);

wstring string2wstring(string str);

//将wstring转换成string  
string wstring2string(wstring wstr);

bool GetJsonByExcelProp(Json::Value&,const ExcelProp&);
bool GetExcelPropByJson(const Json::Value&, ExcelProp&);

bool GetOTPGuideConfigByJsonFile(OTPGuideInfo& out);


class CMyTextEdit :public QTextEdit
{
	Q_OBJECT
public:
	CMyTextEdit(CMyWidgetBase* p_base,string& str_source, const string& init_str);
protected:
	virtual void focusOutEvent(QFocusEvent *event) override;
private:
	CMyWidgetBase* m_pBase = nullptr;
	string& m_strText;
	private slots:
	void callback_MyTextEditTextChanged();
};