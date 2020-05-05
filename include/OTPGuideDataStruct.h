#pragma once

#ifndef   OTP_GUIDE_STRUCT_ECFB9639_CE4B_464B_A18F_97729E33BFE4
#define   OTP_GUIDE_STRUCT_ECFB9639_CE4B_464B_A18F_97729E33BFE4

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <map>
#include <memory>
#include "json.h"

#include <boost/archive/binary_oarchive.hpp> 
#include <boost/archive/binary_iarchive.hpp> 
#include <boost/serialization/map.hpp> 
#include <boost/serialization/vector.hpp> 
#include <boost/serialization/string.hpp>

using namespace std;

class ToJsonConfigBase
{
public:
	virtual Json::Value ToJsonConfig() = 0;
	virtual void GetInfoByJson(const Json::Value& js) = 0;
};

struct ExcelProp
{
	vector<string> m_vecHeaderLabels;
	vector<vector<string>> m_vecData;


	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version) {
		ar & m_vecHeaderLabels;
		ar & m_vecData;
	}
};

struct ContentData_TextImage
{
	vector<char>	data; //文字或者jpg图片数据
	int				type; //0 text, 1 jpg_image

	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version) {
		ar & data;
		ar & type;
	}

};

struct BurnItem_SubContent
{
	string subTitle;
	vector<ContentData_TextImage> burnContentBeforeExcel;		//未用到
	ExcelProp m_burnExcelTable;
	vector<ContentData_TextImage> burnContentAfterExcel;		//未用到

	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version) {
		ar & subTitle;
		ar & m_burnExcelTable;
		ar & burnContentBeforeExcel;
		ar & burnContentAfterExcel;
	}
};

struct BurnItem
{
	string title;
	vector<ContentData_TextImage> contentBeforeSubContent;		//表单最开始的字符串数据
	vector<BurnItem_SubContent> burnItemSubContents;			//每个算法模块下,不同的配置参数

	ExcelProp m_BurnRlueExcel;				//烧录规则表格
	ExcelProp m_CheckSumRangeExcel;			//checksum用到的范围
	ExcelProp m_CheckSumAddrExcel;			//checksum地址的配置
	ExcelProp m_CheckSumDataSourceRnage;	//checksum数据来源配置

	//std::vector<std::vector<std::string>> m_vecBurnRule;
	//std::vector<std::vector<std::string>> m_vecCheckSumRange;
	//std::vector<std::vector<std::string>> m_vecCheckSumAddr;

	//std::vector<std::string> m_vecBurnRuleHeaderLabels;
	//std::vector<std::string> m_vecCheckSumRangeHeaderLabels;
	//std::vector<std::string> m_vecCheckSumAddrHeaderLabels;


	//std::vector<std::vector<std::string>> m_vecChecksumDataSourceRnage;		//checksum用到的地址范围


	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version) {
		ar & title;
		ar & contentBeforeSubContent;
		ar & burnItemSubContents;
	}

};

struct  MapSectionAddr
{
	string m_strName = "";		//用户插入的Guide中的测项名
	int m_iAddr_start = 0;			//插入那行的地址
	int m_iAddr_end = 0;		//这个测项的结束名字

	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version) {
		ar & m_strName;
		ar & m_iAddr_start;
		ar & m_iAddr_end;
	}
};

class EEPROMAddrExcelProp :public ToJsonConfigBase
{
public:
	virtual Json::Value ToJsonConfig() override;
	virtual void GetInfoByJson(const Json::Value& js) override;
	/*////////address
	content- 0
	data   - 1
	description -2
	烧录数据 -3
	拆分类型 -4
	烧录长度 -5*/
	map<int, vector<string>> m_mapAddrProp;
	std::map<int, vector<pair<int, int>>> m_mapColnCombInfo;

	vector<int> m_vecCheckSumAddrs;
	vector<MapSectionAddr> m_vecSectionAddrInfo;
};

class EEPROM_InitInfo:public ToJsonConfigBase
{
public:
	string m_str_eeprom_slaveid = "";
	string m_str_sensor_slaveid = "";
	string m_str_DefaultVal = "";
	//写保护部分
	bool m_bProtectEnable = false;
	string m_str_protect_slaveid = "";
	string m_str_protectAddr = "";
	string m_str_protectVal = "";
	string m_str_ProtectIICMode = "";
	//EFlash部分
	ExcelProp m_EFlashExcel;
	virtual Json::Value ToJsonConfig() override;
	virtual void GetInfoByJson(const Json::Value& js) override;
};

struct m_mapStationInfo
{
	vector<string> m_VecNeedBurnName;
	vector<string> m_VecNeedChecksumName;
	ExcelProp m_BurnAddrsExcel;
};

struct OTPGuideInfo
{
	string m_strProjectName;
	//vector<pair<string,ExcelProp>> m_VecBurnStationAddr;	//结构体修改
	map<string, m_mapStationInfo> m_mapStationInfo;

	//map<string, ExcelProp> m_mapBurnAddrs;				//结构体修改,修改后版本,(烧录站别的地址)
	//map<string, vector<string>> m_mapVecNeedBurnName;		//每个烧录站别会烧录的模块,(烧录站别的模块)
	//map<string, vector<string>> m_mapVecNeedChecksumName;	//每个烧录站别会用到的checksum模块,()
	ExcelProp m_vecChangeHistroy;						
	ExcelProp m_vecProjectInfo;							//(项目信息)
	vector<std::shared_ptr<BurnItem>> m_vecBurnItems;	//(模块配置)
	vector<string> m_vecAgreeOnInfo;
	EEPROMAddrExcelProp m_EEPROMAddrExcel;		
	EEPROM_InitInfo m_eepromInfo;
	//vector<string> m_vecSpaceUsageCheckSum;			//目前被配置使用的checksum名字
	//vector<string> m_vecSpaceUsageBurnModel;		//目前未配置的烧录模块

	bool m_bIsChanged = false;
	int m_iDefaultValue = 0;

	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version) {
		ar & m_strProjectName;
		ar & m_vecChangeHistroy;
		ar & m_vecProjectInfo;
		ar & m_vecBurnItems;
		ar & m_vecAgreeOnInfo;
		ar & m_EEPROMAddrExcel;
		ar & m_bIsChanged;
	}
};
#endif