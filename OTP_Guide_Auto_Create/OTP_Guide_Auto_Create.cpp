#include "stdafx.h"
#include "OTP_Guide_Auto_Create.h"
#include <Windows.h>
#include "OTPGuidePDFWriter.h"

#include "EEPROM_Addr_Map.h"
#include "EEPROM_INIT.h"
#include "ProjectInformation_UI.h"
#include "TestItemContainer_UI.h"
#include "ChangeLog_UI.h"
OTP_Guide_Auto_Create::OTP_Guide_Auto_Create(QWidget *parent)
	: QMainWindow(parent)
{
	QToolBar* toolBar = new QToolBar(this);
	QAction* SaveGuideAction = new QAction(QString::fromLocal8Bit("����guide"), this);
	toolBar->addAction(SaveGuideAction);
	connect(SaveGuideAction, &QAction::triggered, this, &OTP_Guide_Auto_Create::callback_GenerateOTPGuide);

	QAction* SaveConfig = new QAction(QString::fromLocal8Bit("���浱ǰ�����ļ�"), this);
	toolBar->addAction(SaveConfig);
	connect(SaveConfig, &QAction::triggered, this, &OTP_Guide_Auto_Create::SaveConfig);

	QAction* SaveBurnJson = new QAction(QString::fromLocal8Bit("������¼�ļ�"), this);
	toolBar->addAction(SaveBurnJson);
	connect(SaveBurnJson, &QAction::triggered, this, &OTP_Guide_Auto_Create::SaveBurnJsonFile);

	this->addToolBar(toolBar);


	ui.setupUi(this);
	AllocConsole();                     // �򿪿���̨��Դ
	freopen("CONOUT$", "w+t", stdout);// ����д

	if(1)
		GetOTPGuideConfigByJsonFile(m_GuideInfo);

	m_mapWidgets["EEPROMӳ���"].reset(new EEPROM_Addr_Map(m_GuideInfo));
	m_mapWidgets["��¼��ʼ����"].reset(new EEPROM_INIT(m_GuideInfo));
	m_mapWidgets["��Ŀ��Ϣ"].reset(new ProjectInformation_UI(m_GuideInfo));
	m_mapWidgets["��¼��������Ϣ"].reset(new TestItemContainer_UI(m_GuideInfo));
	m_mapWidgets["�޸���ʷ"].reset(new ChangeLog_UI(m_GuideInfo));
	for (auto& wid : m_mapWidgets)
		ui.m_tabWidget->addTab(wid.second.get(), QString::fromLocal8Bit(wid.first.c_str()));
}

void OTP_Guide_Auto_Create::callback_currentChanged(int index)
{
	string tab_name = ui.m_tabWidget->tabText(index).toLocal8Bit().data();
	if(m_mapWidgets[tab_name].get())
		m_mapWidgets[tab_name]->UpdataWidget();
}

void OTP_Guide_Auto_Create::SaveConfig()
{
	m_mapWidgets["��¼��ʼ����"]->UpdataWidget();
	Json::Value all_save_json;
	all_save_json["��Ŀ��"] = m_GuideInfo.m_strProjectName;

	/*---------------��Ŀ��ϢJSON����*/
	Json::Value js_project_info;		//��Ŀ��Ϣ������
	GetJsonByExcelProp(js_project_info, m_GuideInfo.m_vecProjectInfo);
	all_save_json["��Ŀ��Ϣ"] = js_project_info;
	/*--------------��Ŀ��ϢJSON����--<*/

	///*--------------Ŀǰδ���õ���¼ģ��*/
	//Json::Value js_SpaceUsageBurnModel;
	//for (const auto& data : m_GuideInfo.m_vecSpaceUsageBurnModel)
	//	js_SpaceUsageBurnModel.append(data);
	//all_save_json["��������¼վ��ģ��"] = js_SpaceUsageBurnModel;
	///*--------------Ŀǰδ���õ���¼ģ��--<*/

	///*--------------Ŀǰδ���õ�Checksumģ��*/
	//Json::Value js_SpaceUsageCheckSum;
	//for (const auto& data : m_GuideInfo.m_vecSpaceUsageCheckSum)
	//	js_SpaceUsageCheckSum.append(data);
	//all_save_json["������CheckSum��ģ��"] = js_SpaceUsageCheckSum;
	///*--------------Ŀǰδ���õ�CheckSumģ��--<*/

	Json::Value js_BurnStationAddr;
	Json::Value js_BurnStationModule;
	Json::Value js_BurnStationCheckSumModule;
	for (const auto& stations : m_GuideInfo.m_mapStationInfo)
	{
		//*--------------��¼վ��ĵ�ַ*/
		GetJsonByExcelProp(js_BurnStationAddr[stations.first], stations.second.m_BurnAddrsExcel);
		all_save_json["��¼վ���ַ"] = js_BurnStationAddr;
		//*--------------��¼վ��ĵ�ַ--<*/

		/*--------------��¼վ��Ҫ��¼��ģ��*/
		
		Json::Value js_modules;
		for (const auto& burn_name : stations.second.m_VecNeedBurnName)
			js_modules.append(burn_name);	
		js_BurnStationModule[stations.first] = js_modules;
		/*--------------��¼վ��Ҫ��¼��ģ��--<*/

		/*--------------��¼վ��Ҫѡ���Checksumģ��*/
		Json::Value js_CheckSumModules;
		for (const auto& name : stations.second.m_VecNeedChecksumName)
			js_CheckSumModules.append(name);
		js_BurnStationCheckSumModule[stations.first] = js_CheckSumModules;
		/*--------------��¼վ��Ҫѡ���Checksumģ��--<*/
	}
	all_save_json["��¼վ���ģ��"] = js_BurnStationModule;
	all_save_json["��¼վ�����õ�Checksumģ��"] = js_BurnStationCheckSumModule;
	///*--------------��¼վ��ĵ�ַ*/
	//Json::Value js_BurnStationAddr;
	//for (const auto& stations : m_GuideInfo.m_mapBurnAddrs)
	//	GetJsonByExcelProp(js_BurnStationAddr[stations.first], stations.second);
	//all_save_json["��¼վ���ַ"] = js_BurnStationAddr;
	///*--------------��¼վ��ĵ�ַ--<*/

	///*--------------��¼վ��Ҫ��¼��ģ��*/
	//Json::Value js_BurnStationModule;
	//for (const auto& stations : m_GuideInfo.m_mapVecNeedBurnName)
	//{
	//	Json::Value js_modules;
	//	for (const auto& name : stations.second)
	//		js_modules.append(name);

	//	js_BurnStationModule[stations.first] = js_modules;
	//}
	//all_save_json["��¼վ���ģ��"] = js_BurnStationModule;
	///*--------------��¼վ��Ҫ��¼��ģ��--<*/

	/*--------------EEPROM�ĳ�ʼ��Ϣ*/
	Json::Value js_eeprom_init;
	js_eeprom_init["eeprom_slaveid"] = m_GuideInfo.m_eepromInfo.m_str_eeprom_slaveid;
	js_eeprom_init["sensor_slaveid"] = m_GuideInfo.m_eepromInfo.m_str_sensor_slaveid;
	js_eeprom_init["��¼Ĭ��ֵ"] = m_GuideInfo.m_eepromInfo.m_str_DefaultVal;
	js_eeprom_init["д��������"] = m_GuideInfo.m_eepromInfo.m_bProtectEnable;
	js_eeprom_init["д����slaveid"] = m_GuideInfo.m_eepromInfo.m_str_protect_slaveid;
	js_eeprom_init["д�����Ĵ���"] = m_GuideInfo.m_eepromInfo.m_str_protectAddr;
	js_eeprom_init["д����ֵ"] = m_GuideInfo.m_eepromInfo.m_str_protectVal;
	js_eeprom_init["д����IICģʽ"] = m_GuideInfo.m_eepromInfo.m_str_ProtectIICMode;
	all_save_json["EEPROM��ʼ��"] = js_eeprom_init;
	/*--------------EEPROM�ĳ�ʼ��Ϣ--<*/

	/*--------------ģ����¼����,��������*/
	Json::Value js_Items_info;
	for (const auto& items : m_GuideInfo.m_vecBurnItems)
	{
		Json::Value js_Item;
		js_Item["ģ������"] = items->title;

		Json::Value js_rule;
		GetJsonByExcelProp(js_rule, items->m_BurnRlueExcel);
		js_Item["��¼����"] = js_rule;

		Json::Value js_CheckSumRange;
		GetJsonByExcelProp(js_CheckSumRange, items->m_CheckSumRangeExcel);
		js_Item["CheckSum��Χ"] = js_CheckSumRange;

		Json::Value js_CheckSumAddr;
		GetJsonByExcelProp(js_CheckSumAddr, items->m_CheckSumAddrExcel);
		js_Item["CheckSum��ַ"] = js_CheckSumAddr;

		Json::Value js_CheckSumDataSource;
		GetJsonByExcelProp(js_CheckSumDataSource, items->m_CheckSumDataSourceRnage);
		js_Item["CheckSum����Դ����"] = js_CheckSumDataSource;

		//������ģ������ݽṹ�洢
		for (const auto& sub_items : items->burnItemSubContents)
		{
			Json::Value js_sub_item;
			js_sub_item["������"] = sub_items.subTitle;
			GetJsonByExcelProp(js_sub_item, sub_items.m_burnExcelTable);
			js_Item["����������"].append(js_sub_item);
		}

		//�㷨ģ�����õ���ͼƬ���ַ���
		if(items->contentBeforeSubContent.size() == 1)
			if (items->contentBeforeSubContent[0].type == 0)
			{
				string str;
				for (const auto data : items->contentBeforeSubContent[0].data)
				{
					str.push_back(data);
				}
				str.push_back('\0');
				js_Item["����"] = str;
			}

		js_Items_info.append(js_Item);
	}
	all_save_json["ģ������"] = js_Items_info;
	/*--------------ģ����¼����,��������--<*/

	/*--------------�޸���ʷ*/
	Json::Value js_changeHistory;
	GetJsonByExcelProp(js_changeHistory,m_GuideInfo.m_vecChangeHistroy);
	all_save_json["�޸���ʷ"] = js_changeHistory;
	/*--------------�޸���ʷ--<*/

	ostringstream os;
	Json::StreamWriterBuilder swb_cloud_burn;
	unique_ptr<Json::StreamWriter> writer_cloud(swb_cloud_burn.newStreamWriter());
	writer_cloud->write(all_save_json, &os);
	ofstream ofile("out.json", ios::binary | ios::out);
	ofile << os.str();
	ofile.close();
}

void OTP_Guide_Auto_Create::SaveBurnJsonFile()
{
	//((TestItemContainer_UI*)ui.m_tabWidget->widget(3))->GetJsons();

	map<string, Json::Value> map_res_json;
	//������¼վ��������initģ��
	if (m_GuideInfo.m_mapStationInfo.empty())
	{
		cout << "û���κ���¼վ�����Ϣ,�޷�������¼�ļ�" << endl;
		return;
	}
	else
	{
		for (const auto& stations : m_GuideInfo.m_mapStationInfo)
		{
			Json::Value& json_BurnRange = map_res_json[stations.first]["init"]["BurnRange"];
			for (const auto& burn_addrs : stations.second.m_BurnAddrsExcel.m_vecData)
			{
				Json::Value tmp_json;
				tmp_json["start"] = burn_addrs[0];
				tmp_json["end"] = burn_addrs[1];
				json_BurnRange.append(tmp_json);
			}
		}
	}

	for (auto& jsons : map_res_json)
	{
		Json::Value& json_init = jsons.second["init"];
		json_init["ProductName"] = m_GuideInfo.m_strProjectName;
		json_init["ProtectAddr"] = m_GuideInfo.m_eepromInfo.m_str_protectAddr;
		json_init["ProtectEnable"] = m_GuideInfo.m_eepromInfo.m_bProtectEnable ? "true" : "false";
		json_init["ProtectIICMode"] = m_GuideInfo.m_eepromInfo.m_str_ProtectIICMode;
		json_init["ProtectSlaveID"] = m_GuideInfo.m_eepromInfo.m_str_protect_slaveid;
		json_init["ProtectVal"] = m_GuideInfo.m_eepromInfo.m_str_protectVal;
		json_init["SlaveID"] = m_GuideInfo.m_eepromInfo.m_str_eeprom_slaveid;
		json_init["version"] = "V1.0.0.2";
	}

	TestItemContainer_UI* p_container = nullptr;
	//��ȡ��¼��������Ϣtab UIָ��
	for (int i = 0; i < ui.m_tabWidget->count(); i++)
	{
		string str_tab_name = ui.m_tabWidget->tabText(i).toLocal8Bit().data();
		if (str_tab_name == "��¼��������Ϣ")
		{
			p_container = (TestItemContainer_UI*)ui.m_tabWidget->widget(i);
			break;
		}
	}

	for (const auto& burn_item : m_GuideInfo.m_mapStationInfo)
	{
		Json::Value& js_station =  map_res_json[burn_item.first];	//ÿ��վ���json�ṹ��
		for (const auto& item : burn_item.second.m_VecNeedBurnName)
		{
			string str_item_name = item;	//��ȡ��ǰ���㷨ģ����
			auto iter = std::find_if(m_GuideInfo.m_vecBurnItems.begin(), m_GuideInfo.m_vecBurnItems.end(), [str_item_name](std::shared_ptr<BurnItem> p_item)
			{
				return str_item_name == p_item->title;
			});
			if (iter == m_GuideInfo.m_vecBurnItems.end())
			{
				string str_log = (boost::format("%s[ERROR]:û���ҵ�ģ��%s����Ϣ") % __FUNCTION__%str_item_name).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return;
			}
			js_station[str_item_name] = p_container->GetJsonByAlgoName(str_item_name);
		}
	}

	for (const auto& json : map_res_json)
	{
		string str_file = (boost::format("%s_%s.json") % json.first % m_GuideInfo.m_strProjectName).str();
		ostringstream os;
		Json::StreamWriterBuilder swb_cloud_burn;
		unique_ptr<Json::StreamWriter> writer_cloud(swb_cloud_burn.newStreamWriter());
		writer_cloud->write(json.second, &os);
		ofstream ofile(str_file, ios::binary | ios::out);
		ofile << os.str();
		ofile.close();
	}

}

void OTP_Guide_Auto_Create::callback_GenerateOTPGuide()
{
	/*ExcelProp &vecChangeHistroy = m_GuideInfo.m_vecChangeHistroy;

	vecChangeHistroy.m_vecHeaderLabels.push_back("�汾");
	vecChangeHistroy.m_vecHeaderLabels.push_back("����");
	vecChangeHistroy.m_vecHeaderLabels.push_back("����");
	vecChangeHistroy.m_vecHeaderLabels.push_back("�༭��");
	vecChangeHistroy.m_vecHeaderLabels.push_back("���");

	vector<string> table_line1;
	table_line1.push_back("V1.0.0.1");
	table_line1.push_back("��ʼ��");
	table_line1.push_back("2020/4/1");
	table_line1.push_back("����");
	table_line1.push_back("������");
	vecChangeHistroy.m_vecData.push_back(table_line1);

	table_line1.clear();
	table_line1.push_back("V1.0.0.2");
	table_line1.push_back("�޸�MTK PDAF Checksum �ۼӷ�Χ");
	table_line1.push_back("2020/4/3");
	table_line1.push_back("����");
	table_line1.push_back("������");
	vecChangeHistroy.m_vecData.push_back(table_line1);

	table_line1.clear();
	table_line1.push_back("V1.0.0.3");
	table_line1.push_back("Qualcomm AWB/LSC AE����Ϊ��OB�����Ӳ��������ϸ��Ϣ��");
	table_line1.push_back("2020/4/9");
	table_line1.push_back("����");
	table_line1.push_back("������");
	vecChangeHistroy.m_vecData.push_back(table_line1);

	OTPGuideInfo2Pdf(m_GuideInfo, "D:\\");*/
}
