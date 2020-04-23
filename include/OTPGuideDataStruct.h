#pragma once

#ifndef   OTP_GUIDE_STRUCT_ECFB9639_CE4B_464B_A18F_97729E33BFE4
#define   OTP_GUIDE_STRUCT_ECFB9639_CE4B_464B_A18F_97729E33BFE4

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <map>

#include <boost/archive/binary_oarchive.hpp> 
#include <boost/archive/binary_iarchive.hpp> 
#include <boost/serialization/map.hpp> 
#include <boost/serialization/vector.hpp> 
#include <boost/serialization/string.hpp>

using namespace std;

// struct CAddr_prop
// {
// 	string m_strData = "";
// 	string m_strContent = "";
// 	string m_strDescription = "";
// 
// 	friend class boost::serialization::access;
// 	template <typename Archive> void serialize(Archive &ar, const unsigned int version) {
// 		ar & m_strData;
// 		ar & m_strContent;
// 		ar & m_strDescription;
// 	}
// };

// struct MapInfo
// {
// 	map<int, CAddr_prop> m_mapAddrProp;
// 
// 	friend class boost::serialization::access;
// 	template <typename Archive> void serialize(Archive &ar, const unsigned int version) {
// 		ar & m_mapAddrProp;
// 	}
// };

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
	vector<ContentData_TextImage> burnContentBeforeExcel;
	ExcelProp m_burnExcelTable;
	vector<ContentData_TextImage> burnContentAfterExcel;

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
	//string content;
	vector<ContentData_TextImage> contentBeforeSubContent;
	vector<BurnItem_SubContent> burnItemSubContents;

	std::vector<std::vector<std::string>> m_vecBurnRule;
	std::vector<std::vector<std::string>> m_vecCheckSumRange;
	std::vector<std::vector<std::string>> m_vecCheckSumAddr;

	std::vector<std::string> m_vecBurnRuleHeaderLabels;
	std::vector<std::string> m_vecCheckSumRangeHeaderLabels;
	std::vector<std::string> m_vecCheckSumAddrHeaderLabels;

	std::vector<std::vector<std::string>> m_vecEEPROM_ChecksumRnage;
	std::vector<std::vector<std::string>> m_vecSharedMemory_ChecksumRnage;


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

struct EEPROMAddrExcelProp
{
	/*////////address
	content- 0
	data   - 1
	description -2
	烧录数据 -3
	拆分类型 -4
	烧录长度 -5*/
	map<int, vector<string>> m_mapAddrProp;
	//MapInfo m_mapInfo;
	vector<int> m_vecCheckSumAddrs;
	vector<MapSectionAddr> m_vecSectionAddrInfo;
	std::map<int, vector<pair<int, int>>> m_mapColnCombInfo;


	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version) {
		ar & m_mapAddrProp;
		ar & m_mapColnCombInfo;
		ar & m_vecSectionAddrInfo;
		ar & m_vecCheckSumAddrs;
	}
};

struct OTPGuideInfo
{
	string m_strProjectName;
	vector<pair<string,ExcelProp>> m_VecBurnStationAddr;
	ExcelProp m_vecChangeHistroy;
	ExcelProp m_vecProjectInfo;
	vector<BurnItem> m_vecBurnItems;
	vector<string> m_vecAgreeOnInfo;
	EEPROMAddrExcelProp m_EEPROMAddrExcel;

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