#include "stdafx.h"
#include<OTPGuideDataStruct.h>

Json::Value EEPROMAddrExcelProp::ToJsonConfig()
{
	char buff[32];
	Json::Value js_res;
	Json::Value js_addr_info;
	for (const auto& addrs_desc : m_mapAddrProp)
	{
		Json::Value js_tmp;
		//先append地址
		sprintf(buff, "0x%x", addrs_desc.first);
		js_tmp.append(buff);
		js_tmp.append(addrs_desc.second[0]);
		js_tmp.append(addrs_desc.second[1]);
		js_tmp.append(addrs_desc.second[2]);

		js_addr_info.append(js_tmp);
	}
	js_res["地址附加信息"] = js_addr_info;

	//合并地址的区间
	Json::Value js_span_info;
	for (const auto& span_info : m_mapColnCombInfo)
	{
		Json::Value js_tmp_range;
		string str_span_col = boost::lexical_cast<string>(span_info.first);
		for (const auto& span_range : span_info.second)
		{
			Json::Value js_tmp_array;
			sprintf(buff, "0x%x", span_range.first);
			js_tmp_array.append(buff);
			sprintf(buff, "0x%x", span_range.second);
			js_tmp_array.append(buff);

			js_tmp_range.append(js_tmp_array);	//将数组加入区间内
		}
		js_span_info[str_span_col] = js_tmp_range;
	}
	js_res["地址合并区间"] = js_span_info;

	return js_res;
}

void EEPROMAddrExcelProp::GetInfoByJson(const Json::Value& js)
{
	const Json::Value& js_addr_info = js["地址附加信息"];
	if (!js_addr_info.isNull())
	{
		int addr = -1;
		int array_count = js_addr_info.size();
		for (int i = 0; i < array_count; i++)
		{
			const Json::Value& js_array_index = js_addr_info[i];
			sscanf(js_array_index[0].asString().c_str(), "0x%x", &addr);
			boost::assign::push_back(m_mapAddrProp[addr])
				(js_array_index[1].asString())
				(js_array_index[2].asString())
				(js_array_index[3].asString())
				("")("")("");
		}
	}

	const Json::Value& js_span_info = js["地址合并区间"];
	if (!js_span_info.isNull())
	{
		int addr_start = -1,addr_end = -1;
		string str1 = "", str2 = "";
		vector<string> vec_cols_name = js_span_info.getMemberNames();
		for (const auto col_name : vec_cols_name)
		{
			int col_index = boost::lexical_cast<int>(col_name);
			const Json::Value&  js_tmp_array = js_span_info[col_name];		//找到对应列的合并信息
			int array_count = js_tmp_array.size();
			for (int i = 0; i < array_count; i++)
			{
				str1 = js_tmp_array[i][0].asString();
				str2 = js_tmp_array[i][1].asString();
				sscanf(str1.c_str(), "0x%x", &addr_start);	
				sscanf(str2.c_str(), "0x%x", &addr_end);
				m_mapColnCombInfo[col_index].push_back(make_pair(addr_start, addr_end));
			}
		}
	}
}

Json::Value EEPROM_InitInfo::ToJsonConfig()
{
	Json::Value js_eeprom_init;
	js_eeprom_init["eeprom_slaveid"] = m_str_eeprom_slaveid;
	js_eeprom_init["sensor_slaveid"] = m_str_sensor_slaveid;
	js_eeprom_init["烧录默认值"] = m_str_DefaultVal;
	js_eeprom_init["写保护开关"] =m_bProtectEnable;
	js_eeprom_init["写保护slaveid"] = m_str_protect_slaveid;
	js_eeprom_init["写保护寄存器"] = m_str_protectAddr;
	js_eeprom_init["写保护值"] = m_str_protectVal;
	js_eeprom_init["写保护IIC模式"] = m_str_ProtectIICMode;
	return js_eeprom_init;
	//all_save_json["EEPROM初始化"] = js_eeprom_init;
}

void EEPROM_InitInfo::GetInfoByJson(const Json::Value& js)
{

}