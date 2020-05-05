#pragma once
#include "OTPGuideDataStruct.h"
#include "Qt_Excel.h"
#include <json.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <QKeyEvent>
#include <QTextEdit>
#include <QMessageBox>
#include <QInputDialog>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/property_tree/ini_parser.hpp>

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
bool GetJsonStringMap(map<int, string>& out_map);

bool BatchReadFile(void* ptr, const size_t& slaveid, const size_t& start, const size_t& end, std::map<size_t, size_t>& out_map, string& log);
void GetEEPROMParse_Log(void* ptr, const char* log);

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

template<class T>
class Serialization : public T
{
public:

	void serialization(std::ostringstream& ostream) {
		boost::archive::text_oarchive oa(ostream);
		oa << *this;
	}

	void unserialization(std::istringstream& istream) {
		boost::archive::text_iarchive ia(istream);
		ia >> *this;
	}
private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & boost::serialization::base_object<T>(*this);
	}
};

template<class T, class T1>
class MapSerialization : public Serialization<std::map<T, T1> > {
};