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
	this->addToolBar(toolBar);


	ui.setupUi(this);
	AllocConsole();                     // �򿪿���̨��Դ
	freopen("CONOUT$", "w+t", stdout);// ����д

	if (1)
	{
		GetOTPGuideConfigByJsonFile(m_GuideInfo);
	}

	m_mapWidgets["EEPROMӳ���"].reset(new EEPROM_Addr_Map(m_GuideInfo));
	m_mapWidgets["��¼��ʼ����"].reset(new EEPROM_INIT(m_GuideInfo));
	m_mapWidgets["��Ŀ��Ϣ"].reset(new ProjectInformation_UI(m_GuideInfo));
	m_mapWidgets["��¼��������Ϣ"].reset(new TestItemContainer_UI(m_GuideInfo));
	m_mapWidgets["�޸���ʷ"].reset(new ChangeLog_UI(m_GuideInfo));
	for (auto& wid : m_mapWidgets)
	{
		ui.m_tabWidget->addTab(wid.second.get(), QString::fromLocal8Bit(wid.first.c_str()));
	}
}

void OTP_Guide_Auto_Create::callback_currentChanged(int index)
{
	string tab_name = ui.m_tabWidget->tabText(index).toLocal8Bit().data();
	if(m_mapWidgets[tab_name].get())
		m_mapWidgets[tab_name]->UpdataWidget();
	//m_ProjectInformation->ShowExcel();
	//if (ui.m_tabWidget->tabText(index) == QString::fromLocal8Bit("init"))
	//{
	//}
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
	//Json::Value js_project_info_Header;	//��Ŀ��Ϣÿ�е�����
	//for (const auto& head : m_GuideInfo.m_vecProjectInfo.m_vecHeaderLabels)
	//	js_project_info_Header.append(head);
	//js_project_info["��ͷ"] = js_project_info_Header;

	//Json::Value js_project_info_line;	//��Ŀ��Ϣÿ�е�����
	//for (const auto& datas: m_GuideInfo.m_vecProjectInfo.m_vecData)
	//{
	//	js_project_info_line.clear();
	//	for (const auto& info : datas)
	//	{
	//		js_project_info_line.append(info);
	//	}
	//	js_project_info["����"].append(js_project_info_line);
	//}
	/*--------------��Ŀ��ϢJSON����--<*/

	/*--------------Ŀǰδ���õ���¼ģ��*/
	Json::Value js_SpaceUsageBurnModel;
	for (const auto& data : m_GuideInfo.m_vecSpaceUsageBurnModel)
		js_SpaceUsageBurnModel.append(data);
	all_save_json["δ������¼վ��ģ��"] = js_SpaceUsageBurnModel;
	/*--------------Ŀǰδ���õ���¼ģ��--<*/

	/*--------------Ŀǰδ���õ�Checksumģ��*/
	Json::Value js_SpaceUsageCheckSum;
	for (const auto& data : m_GuideInfo.m_vecSpaceUsageCheckSum)
		js_SpaceUsageCheckSum.append(data);
	all_save_json["δ������¼վ��ģ��"] = js_SpaceUsageCheckSum;
	/*--------------Ŀǰδ���õ�CheckSumģ��--<*/

	/*--------------��¼վ��ĵ�ַ*/
	Json::Value js_BurnStationAddr;
	for (const auto& stations : m_GuideInfo.m_mapBurnAddrs)
		GetJsonByExcelProp(js_BurnStationAddr[stations.first], stations.second);
	all_save_json["��¼վ���ַ"] = js_BurnStationAddr;
	/*--------------��¼վ��ĵ�ַ--<*/

	/*--------------��¼վ��Ҫ��¼��ģ��*/
	Json::Value js_BurnStationModule;
	for (const auto& stations : m_GuideInfo.m_mapVecNeedBurnName)
	{
		Json::Value js_modules;
		for (const auto& name : stations.second)
			js_modules.append(name);

		js_BurnStationModule[stations.first] = js_modules;
	}
	all_save_json["��¼վ���ģ��"] = js_BurnStationModule;
	/*--------------��¼վ��Ҫ��¼��ģ��--<*/

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

void OTP_Guide_Auto_Create::callback_GenerateOTPGuide()
{
	((TestItemContainer_UI*)ui.m_tabWidget->widget(3))->GetJsons();
	Json::Value res_json;

	Json::Value json_init = res_json["init"];
	if (m_GuideInfo.m_mapBurnAddrs.empty())
	{
		cout << "�յ�" << endl;
	}
	for (const auto& stations : m_GuideInfo.m_mapBurnAddrs)
	{
		Json::Value& json_BurnRange = json_init["BurnRange"];
		string str_station = "";
		for (const auto& burn_addrs : stations.second.m_vecData)
		{
			Json::Value tmp_json;
			tmp_json["start"] = burn_addrs[0];
			tmp_json["end"] = burn_addrs[1];
			json_BurnRange.append(tmp_json);
		}
	}
	json_init["ProductName"] = m_GuideInfo.m_strProjectName;
	json_init["ProtectAddr"] = m_GuideInfo.m_eepromInfo.m_str_protectAddr;
	json_init["ProtectEnable"] = m_GuideInfo.m_eepromInfo.m_bProtectEnable ? "true" : "false";
	json_init["ProtectIICMode"] = m_GuideInfo.m_eepromInfo.m_str_ProtectIICMode;
	json_init["ProtectSlaveID"] = m_GuideInfo.m_eepromInfo.m_str_protect_slaveid;
	json_init["ProtectVal"] = m_GuideInfo.m_eepromInfo.m_str_protectVal;
	json_init["SlaveID"] = m_GuideInfo.m_eepromInfo.m_str_eeprom_slaveid;
	json_init["version"] = "V1.0.0.2";

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
