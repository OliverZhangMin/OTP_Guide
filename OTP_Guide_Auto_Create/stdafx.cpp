#include "stdafx.h"
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

string m_strParseLog;

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

	if (str.empty())
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

CMyTextEdit::CMyTextEdit(CMyWidgetBase* p_base,string& str_source, const string& init_str) :m_pBase(p_base),m_strText(str_source)
{
	setText(QString::fromLocal8Bit(init_str.c_str()));
	connect(this, SIGNAL(textChanged()), this, SLOT(callback_MyTextEditTextChanged()));
}

void CMyTextEdit::callback_MyTextEditTextChanged()
{
	m_strText = this->document()->toPlainText().toLocal8Bit().data();
}

void CMyTextEdit::focusOutEvent(QFocusEvent *event)
{
	m_pBase->ShowExcel();
	this->destroy(true);
}

bool GetJsonByExcelProp(Json::Value& js, const ExcelProp& prop)
{
	Json::Value js_project_info;		//��Ŀ��Ϣ������
	Json::Value js_Header;	//��Ŀ��Ϣÿ�е�����
	for (const auto& head : prop.m_vecHeaderLabels)
		js_Header.append(head);
	js["��ͷ"] = js_Header;

	Json::Value js_line;	//��Ŀ��Ϣÿ�е�����
	for (const auto& datas : prop.m_vecData)
	{
		js_line.clear();
		for (const auto& info : datas)
		{
			js_line.append(info);
		}
		js["����"].append(js_line);
	}
	return true;
}

bool GetExcelPropByJson(const Json::Value& js, ExcelProp& excel)
{
	if (!js.isNull())
	{
		if (!js["��ͷ"].isNull())
		{
			const Json::Value& js_array = js["��ͷ"];
			for (int i = 0; i < js_array.size(); i++)
				excel.m_vecHeaderLabels.push_back(js_array[i].asString());
		}

		if (!js["����"].isNull())
		{
			const Json::Value& js_array1 = js["����"];
			for (int i = 0; i < js_array1.size(); i++)
			{
				const Json::Value& js_array2 = js_array1[i];
				vector<string> vec_str;
				for (int k = 0; k < js_array2.size(); k++)
					vec_str.push_back(js_array2[k].asString());
				excel.m_vecData.push_back(vec_str);
			}
		}
	}
	return true;
}

bool GetOTPGuideConfigByJsonFile(OTPGuideInfo& out)
{
	ifstream ifile("out.json", ios::binary | ios::in);
	if (!ifile.is_open())
	{
		cout << "�������ļ�ʧ��" << endl;
		return false;
	}
	Json::Value root;
	Json::CharReaderBuilder crb;
	unique_ptr<Json::CharReader> reader(crb.newCharReader());
	ostringstream os;
	os << ifile.rdbuf();
	string buff = os.str();
	string strErrMsg;
	if (!reader->parse(buff.c_str(), buff.c_str() + buff.length(), &root, &strErrMsg))
	{
		string str_log = (boost::format("����%s��json����ʧ��,�﷨����:%s") %("out.json") %strErrMsg).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return false;
	}

	if (!root["��Ŀ��"].isNull())
		out.m_strProjectName = root["��Ŀ��"].asString();

	Json::Value& js_eeprom_init = root["EEPROM��ʼ��"];
	if (!js_eeprom_init.isNull())
	{
		out.m_eepromInfo.GetInfoByJson(js_eeprom_init);
		/*out.m_eepromInfo.m_str_eeprom_slaveid     = js_eeprom_init["eeprom_slaveid"].asString();
		out.m_eepromInfo.m_str_sensor_slaveid     = js_eeprom_init["sensor_slaveid"].asString();
		out.m_eepromInfo.m_str_DefaultVal         = js_eeprom_init["��¼Ĭ��ֵ"].asString();
		out.m_eepromInfo.m_str_ProtectIICMode	  = js_eeprom_init["д����IICģʽ"].asString();
		out.m_eepromInfo.m_str_protect_slaveid    = js_eeprom_init["д����slaveid"].asString();
		out.m_eepromInfo.m_str_protectAddr	      = js_eeprom_init["д�����Ĵ���"].asString();
		out.m_eepromInfo.m_bProtectEnable		  = js_eeprom_init["д��������"].asBool();
		out.m_eepromInfo.m_str_protectVal	      = js_eeprom_init["д����ֵ"].asString();*/
	}

	Json::Value& js_burn_station_addr = root["��¼վ���ַ"];
	if (!js_burn_station_addr.isNull())
	{
		auto vec_names = js_burn_station_addr.getMemberNames();
		for(const auto& name:vec_names)
			GetExcelPropByJson(js_burn_station_addr[name], out.m_mapStationInfo[name].m_BurnAddrsExcel);
	}

	Json::Value& js_BurnStationModule = root["��¼վ���ģ��"];
	if (!js_BurnStationModule.isNull())
	{
		//�Ȼ�ȡվ����
		auto vec_names = js_BurnStationModule.getMemberNames();
		//�ڰ�ÿ��վ���µ�ģ����䵽���ݽṹ
		for (const auto& name : vec_names)
		{
			Json::Value& js_station_name = js_BurnStationModule[name];
			for (int i = 0; i < js_station_name.size(); i++)
				out.m_mapStationInfo[name].m_VecNeedBurnName.push_back(js_station_name[i].asString());
		}
	}

	//Json::Value& js_BurnStationCheckSumModule = root["��¼վ�����õ�Checksumģ��"];
	//if (!js_BurnStationCheckSumModule.isNull())
	//{
	//	//�Ȼ�ȡվ����
	//	auto vec_names = js_BurnStationCheckSumModule.getMemberNames();
	//	//�ڰ�ÿ��վ���µ�ģ����䵽���ݽṹ
	//	for (const auto& name : vec_names)
	//	{
	//		Json::Value& js_station_name = js_BurnStationCheckSumModule[name];
	//		for (int i = 0; i < js_station_name.size(); i++)
	//			out.m_mapStationInfo[name].m_VecNeedChecksumName.push_back(js_station_name[i].asString());
	//	}
	//}

	//Json::Value& js_BurnStationBurnModule = root["��������¼վ��ģ��"];
	//if (!js_BurnStationBurnModule.isNull())
	//{
	//	//�Ȼ�ȡվ����
	//	auto vec_names = js_BurnStationBurnModule.getMemberNames();
	//	//�ڰ�ÿ��վ���µ�ģ����䵽���ݽṹ
	//	for (const auto& name : vec_names)
	//	{
	//		Json::Value& js_ItemNames = js_BurnStationBurnModule[name];
	//		for (int i = 0; i < js_ItemNames.size(); i++)
	//			out.m_vecSpaceUsageBurnModel.push_back(js_ItemNames[i].asString());
	//	}
	//}

	//Json::Value& js_SpaceUsageCheckSum = root["������CheckSum��ģ��"];
	//if (!js_SpaceUsageCheckSum.isNull())
	//{
	//	//�Ȼ�ȡվ����
	//	auto vec_names = js_SpaceUsageCheckSum.getMemberNames();
	//	//�ڰ�ÿ��վ���µ�ģ����䵽���ݽṹ
	//	for (const auto& name : vec_names)
	//	{
	//		Json::Value& js_ItemNames = js_SpaceUsageCheckSum[name];
	//		for (int i = 0; i < js_ItemNames.size(); i++)
	//			out.m_vecSpaceUsageCheckSum.push_back(js_ItemNames[i].asString());
	//	}
	//}

	Json::Value& js_projectInfo = root["��Ŀ��Ϣ"];
	GetExcelPropByJson(js_projectInfo, out.m_vecProjectInfo);

	Json::Value& js_Items = root["ģ������"];
	if (!js_Items.isNull())
	{
		int count = js_Items.size();
		for (int i = 0; i < count; i++)
		{
			Json::Value& js_item = js_Items[i];
			std::shared_ptr<BurnItem> p_BurnItem(new BurnItem);
			p_BurnItem->title = js_item["ģ������"].asString();
			out.m_vecBurnItems.push_back(p_BurnItem);
			
			if (!js_item["��¼����"].isNull())
				GetExcelPropByJson(js_item["��¼����"]	   , p_BurnItem->m_BurnRlueExcel);
			 
			if(!js_item["CheckSum��Χ"].isNull())
				GetExcelPropByJson(js_item["CheckSum��Χ"] , p_BurnItem->m_CheckSumRangeExcel);

			if (!js_item["CheckSum��ַ"].isNull())
				GetExcelPropByJson(js_item["CheckSum��ַ"] , p_BurnItem->m_CheckSumAddrExcel);

			if (!js_item["CheckSum����Դ����"].isNull())
				GetExcelPropByJson(js_item["CheckSum����Դ����"], p_BurnItem->m_CheckSumDataSourceRnage);

			if (!js_item["����������"].isNull())
			{
				Json::Value& js_item_config = js_item["����������"];
				int configs_count = js_item_config.size();
				for (int k = 0; k < configs_count; k++)
				{
					Json::Value& js_array = js_item_config[k];
					BurnItem_SubContent subCont;
					subCont.subTitle = js_array["������"].asString();
					GetExcelPropByJson(js_array, subCont.m_burnExcelTable);
					p_BurnItem->burnItemSubContents.push_back(subCont);
				}
			}
			//��ȡ������Ϣ
			if (!js_item["����"].isNull())
			{
				ContentData_TextImage txtImage;
				txtImage.type = 0;
				string str_data = js_item["����"].asString();
				for (const auto& data : str_data)
					txtImage.data.push_back(data);
				p_BurnItem->contentBeforeSubContent.push_back(txtImage);
			}
		}
	}

	//��ȡ�޸���ʷ
	GetExcelPropByJson(root["�޸���ʷ"], out.m_vecChangeHistroy);

	/*--------------���û����õĵ�ַ��������Ϣ����*/
	out.m_EEPROMAddrExcel.GetInfoByJson(root["EEPROMӳ�����Ϣ"]);
	/*--------------���û����õĵ�ַ��������Ϣ����--<*/
	return true;
}

bool GetJsonStringMap(map<int, string>& out_map)
{
	boost::iostreams::file_source file_json0("./JsonVirtualData0.txt", ios::binary | ios::in);
	if (!file_json0.is_open())
	{
		string str_log = (boost::format("%s[ERROR]:���ļ�JsonVirtualData0.txtʧ��")%__FUNCTION__).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return false;
	}

	boost::iostreams::file_source file_json1("./JsonVirtualData1.txt", ios::binary | ios::in);
	if (!file_json1.is_open())
	{
		string str_log = (boost::format("%s[ERROR]:���ļ�JsonVirtualData1.txtʧ��") % __FUNCTION__).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return false;
	}
	boost::iostreams::copy(file_json0, boost::iostreams::back_inserter(out_map[0]));
	boost::iostreams::copy(file_json1, boost::iostreams::back_inserter(out_map[1]));
	return true;
}

bool BatchReadFile(void* ptr, const size_t& slaveid, const size_t& start, const size_t& end, std::map<size_t, size_t>& out_map, string& log)
{
	//out.clear();
	ifstream read_eeprom("EEPROM.txt", ios::binary | ios::in);
	if (!read_eeprom.is_open())
	{
		string str_log = (boost::format("%s[ERROR]:��EEPROM.txt�ļ�ʧ��") % __FUNCTION__).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return false;
	}
	stringstream os;
	os << read_eeprom.rdbuf();
	read_eeprom.close();

	string line = "";
	int addr = 0, value = 0, line_index = 0;;
	string left = "", right = "";
	while (getline(os, line))
	{
		if (line_index >= start && line_index <= end)
		{
			left = line.substr(0, line.find('='));
			right = line.substr(line.find('=') + 1, line.length());
			sscanf_s(left.c_str(), "%x", &addr);
			sscanf_s(right.c_str(), "%x", &value);
			out_map[addr] = value;
			line_index++;
		}
		else
			line_index++;
	}
	return true;
}

void GetEEPROMParse_Log(void* ptr, const char* log)
{
	m_strParseLog.clear();
	m_strParseLog.assign(log);
}