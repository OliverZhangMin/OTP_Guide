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
	vector<char>	data; //���ֻ���jpgͼƬ����
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
	vector<ContentData_TextImage> burnContentBeforeExcel;		//δ�õ�
	ExcelProp m_burnExcelTable;
	vector<ContentData_TextImage> burnContentAfterExcel;		//δ�õ�

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
	vector<ContentData_TextImage> contentBeforeSubContent;		//���ʼ���ַ�������
	vector<BurnItem_SubContent> burnItemSubContents;			//ÿ���㷨ģ����,��ͬ�����ò���

	ExcelProp m_BurnRlueExcel;				//��¼������
	ExcelProp m_CheckSumRangeExcel;			//checksum�õ��ķ�Χ
	ExcelProp m_CheckSumAddrExcel;			//checksum��ַ������
	ExcelProp m_CheckSumDataSourceRnage;	//checksum������Դ����

	//std::vector<std::vector<std::string>> m_vecBurnRule;
	//std::vector<std::vector<std::string>> m_vecCheckSumRange;
	//std::vector<std::vector<std::string>> m_vecCheckSumAddr;

	//std::vector<std::string> m_vecBurnRuleHeaderLabels;
	//std::vector<std::string> m_vecCheckSumRangeHeaderLabels;
	//std::vector<std::string> m_vecCheckSumAddrHeaderLabels;


	//std::vector<std::vector<std::string>> m_vecChecksumDataSourceRnage;		//checksum�õ��ĵ�ַ��Χ


	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version) {
		ar & title;
		ar & contentBeforeSubContent;
		ar & burnItemSubContents;
	}

};

struct  MapSectionAddr
{
	string m_strName = "";		//�û������Guide�еĲ�����
	int m_iAddr_start = 0;			//�������еĵ�ַ
	int m_iAddr_end = 0;		//�������Ľ�������

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
	��¼���� -3
	������� -4
	��¼���� -5*/
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
	//д��������
	bool m_bProtectEnable = false;
	string m_str_protect_slaveid = "";
	string m_str_protectAddr = "";
	string m_str_protectVal = "";
	string m_str_ProtectIICMode = "";
	//EFlash����
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
	//vector<pair<string,ExcelProp>> m_VecBurnStationAddr;	//�ṹ���޸�
	map<string, m_mapStationInfo> m_mapStationInfo;

	//map<string, ExcelProp> m_mapBurnAddrs;				//�ṹ���޸�,�޸ĺ�汾,(��¼վ��ĵ�ַ)
	//map<string, vector<string>> m_mapVecNeedBurnName;		//ÿ����¼վ�����¼��ģ��,(��¼վ���ģ��)
	//map<string, vector<string>> m_mapVecNeedChecksumName;	//ÿ����¼վ����õ���checksumģ��,()
	ExcelProp m_vecChangeHistroy;						
	ExcelProp m_vecProjectInfo;							//(��Ŀ��Ϣ)
	vector<std::shared_ptr<BurnItem>> m_vecBurnItems;	//(ģ������)
	vector<string> m_vecAgreeOnInfo;
	EEPROMAddrExcelProp m_EEPROMAddrExcel;		
	EEPROM_InitInfo m_eepromInfo;
	//vector<string> m_vecSpaceUsageCheckSum;			//Ŀǰ������ʹ�õ�checksum����
	//vector<string> m_vecSpaceUsageBurnModel;		//Ŀǰδ���õ���¼ģ��

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