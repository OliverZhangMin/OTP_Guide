#pragma once
#include <map>
#include <vector>
#pragma once
#pragma pack(8)
#include <string>
using namespace std;

struct CEEPROMParse_Prop
{
	int m_iArea = -1;
	bool m_bCheckProjectName = true;
	bool m_bEEPROMTool = false;
	bool m_bSaveData = false;
	bool m_bConvertCloudData = false;
	bool m_bReadDataToCheck = false;
	string m_strSaveDirectory = "";
	string m_str_ProjectName = "";
	string m_strSN = "";
	string m_strManufacture = "QTech";
	string m_strQr = "";
	string m_strJsonConfigPath = "";
	string m_strJsonINIPath = "";
	string m_strCurExePath = "";
	string m_strJsonFileBuffer = "";
	CEEPROMParse_Prop& operator =(const CEEPROMParse_Prop& str);
};

struct CEEPROMParseResult
{
	string m_strCloudBurnData = "";
	string m_strBurnJsonData = "";
};

typedef bool(*IIC_BatchRead_CALLBACK)(void*, const size_t&,const size_t& ,const size_t&, std::map<size_t, size_t>&,string&);
typedef bool(*IIC_Read_CALLBACK ) (void*, const size_t&, const size_t&, size_t&, string&);
typedef bool(*IIC_Read_CALLBACK_Total) (void*, const size_t&, const size_t&, size_t&, string&, size_t&);
typedef bool(*IIC_Write_CALLBACK) (void*, const size_t& ,const size_t&, const size_t&, string&);
typedef bool(*IIC_BatchWrite_CALLBACK)(void*, const size_t&, const size_t&, const size_t&, std::map<size_t, size_t>&, string&);
typedef void(*GetParseLog_CALLBACK)(void*, const char*);		//Qatar用于获取解析的log,需要反序列化STL容器

extern "C" __declspec(dllexport) void AddParseLog(const char* log);
extern "C" __declspec(dllexport) void GetLog(void*, GetParseLog_CALLBACK);
extern "C" __declspec(dllexport) bool EEPROM_PROP_INIT(const CEEPROMParse_Prop& prop);
extern "C" __declspec(dllexport) void SetIIC_CALLBACK(void* ptr = nullptr, IIC_BatchRead_CALLBACK CALL_BACK_Batch_read = nullptr, IIC_Read_CALLBACK CALL_BACK_read = nullptr, IIC_Write_CALLBACK CALL_BACK_Write = nullptr, IIC_BatchWrite_CALLBACK CALL_BACK_BatchWrite = nullptr, IIC_Read_CALLBACK_Total CALL_BACK_read_total = nullptr);
extern "C" __declspec(dllexport) bool Parse(const char* map_str_json_data);
extern "C" __declspec(dllexport) bool GetEEPROMParseResult(CEEPROMParseResult* result);
extern "C" __declspec(dllexport) void GetCompareData(void*, GetParseLog_CALLBACK);

//烧录用接口
extern "C" __declspec(dllexport) bool Burn(const char*);
