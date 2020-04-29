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
	QAction* SaveGuideAction = new QAction(QString::fromLocal8Bit("生成guide"), this);
	toolBar->addAction(SaveGuideAction);
	connect(SaveGuideAction, &QAction::triggered, this, &OTP_Guide_Auto_Create::callback_GenerateOTPGuide);

	QAction* SaveConfig = new QAction(QString::fromLocal8Bit("保存当前配置文件"), this);
	toolBar->addAction(SaveConfig);
	connect(SaveConfig, &QAction::triggered, this, &OTP_Guide_Auto_Create::SaveConfig);
	this->addToolBar(toolBar);


	ui.setupUi(this);
	AllocConsole();                     // 打开控制台资源
	freopen("CONOUT$", "w+t", stdout);// 申请写

	if (1)
	{
		GetOTPGuideConfigByJsonFile(m_GuideInfo);
	}

	m_mapWidgets["EEPROM映射表"].reset(new EEPROM_Addr_Map(m_GuideInfo));
	m_mapWidgets["烧录初始配置"].reset(new EEPROM_INIT(m_GuideInfo));
	m_mapWidgets["项目信息"].reset(new ProjectInformation_UI(m_GuideInfo));
	m_mapWidgets["烧录测试项信息"].reset(new TestItemContainer_UI(m_GuideInfo));
	m_mapWidgets["修改历史"].reset(new ChangeLog_UI(m_GuideInfo));
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
	m_mapWidgets["烧录初始配置"]->UpdataWidget();
	Json::Value all_save_json;
	all_save_json["项目名"] = m_GuideInfo.m_strProjectName;

	/*---------------项目信息JSON数据*/
	Json::Value js_project_info;		//项目信息总数据
	GetJsonByExcelProp(js_project_info, m_GuideInfo.m_vecProjectInfo);
	all_save_json["项目信息"] = js_project_info;
	//Json::Value js_project_info_Header;	//项目信息每行的数据
	//for (const auto& head : m_GuideInfo.m_vecProjectInfo.m_vecHeaderLabels)
	//	js_project_info_Header.append(head);
	//js_project_info["表头"] = js_project_info_Header;

	//Json::Value js_project_info_line;	//项目信息每行的数据
	//for (const auto& datas: m_GuideInfo.m_vecProjectInfo.m_vecData)
	//{
	//	js_project_info_line.clear();
	//	for (const auto& info : datas)
	//	{
	//		js_project_info_line.append(info);
	//	}
	//	js_project_info["内容"].append(js_project_info_line);
	//}
	/*--------------项目信息JSON数据--<*/

	/*--------------目前未配置的烧录模块*/
	Json::Value js_SpaceUsageBurnModel;
	for (const auto& data : m_GuideInfo.m_vecSpaceUsageBurnModel)
		js_SpaceUsageBurnModel.append(data);
	all_save_json["未配置烧录站的模块"] = js_SpaceUsageBurnModel;
	/*--------------目前未配置的烧录模块--<*/

	/*--------------目前未配置的Checksum模块*/
	Json::Value js_SpaceUsageCheckSum;
	for (const auto& data : m_GuideInfo.m_vecSpaceUsageCheckSum)
		js_SpaceUsageCheckSum.append(data);
	all_save_json["未配置烧录站的模块"] = js_SpaceUsageCheckSum;
	/*--------------目前未配置的CheckSum模块--<*/

	/*--------------烧录站别的地址*/
	Json::Value js_BurnStationAddr;
	for (const auto& stations : m_GuideInfo.m_mapBurnAddrs)
		GetJsonByExcelProp(js_BurnStationAddr[stations.first], stations.second);
	all_save_json["烧录站别地址"] = js_BurnStationAddr;
	/*--------------烧录站别的地址--<*/

	/*--------------烧录站别要烧录的模块*/
	Json::Value js_BurnStationModule;
	for (const auto& stations : m_GuideInfo.m_mapVecNeedBurnName)
	{
		Json::Value js_modules;
		for (const auto& name : stations.second)
			js_modules.append(name);

		js_BurnStationModule[stations.first] = js_modules;
	}
	all_save_json["烧录站别的模块"] = js_BurnStationModule;
	/*--------------烧录站别要烧录的模块--<*/

	/*--------------EEPROM的初始信息*/
	Json::Value js_eeprom_init;
	js_eeprom_init["eeprom_slaveid"] = m_GuideInfo.m_eepromInfo.m_str_eeprom_slaveid;
	js_eeprom_init["sensor_slaveid"] = m_GuideInfo.m_eepromInfo.m_str_sensor_slaveid;
	js_eeprom_init["烧录默认值"] = m_GuideInfo.m_eepromInfo.m_str_DefaultVal;
	js_eeprom_init["写保护开关"] = m_GuideInfo.m_eepromInfo.m_bProtectEnable;
	js_eeprom_init["写保护slaveid"] = m_GuideInfo.m_eepromInfo.m_str_protect_slaveid;
	js_eeprom_init["写保护寄存器"] = m_GuideInfo.m_eepromInfo.m_str_protectAddr;
	js_eeprom_init["写保护值"] = m_GuideInfo.m_eepromInfo.m_str_protectVal;
	js_eeprom_init["写保护IIC模式"] = m_GuideInfo.m_eepromInfo.m_str_ProtectIICMode;
	all_save_json["EEPROM初始化"] = js_eeprom_init;
	/*--------------EEPROM的初始信息--<*/

	/*--------------模块烧录规则,参数配置*/
	Json::Value js_Items_info;
	for (const auto& items : m_GuideInfo.m_vecBurnItems)
	{
		Json::Value js_Item;
		js_Item["模块名称"] = items->title;

		Json::Value js_rule;
		GetJsonByExcelProp(js_rule, items->m_BurnRlueExcel);
		js_Item["烧录规则"] = js_rule;

		Json::Value js_CheckSumRange;
		GetJsonByExcelProp(js_CheckSumRange, items->m_CheckSumRangeExcel);
		js_Item["CheckSum范围"] = js_CheckSumRange;

		Json::Value js_CheckSumAddr;
		GetJsonByExcelProp(js_CheckSumAddr, items->m_CheckSumAddrExcel);
		js_Item["CheckSum地址"] = js_CheckSumAddr;

		//个别子模块的数据结构存储
		for (const auto& sub_items : items->burnItemSubContents)
		{
			Json::Value js_sub_item;
			js_sub_item["配置名"] = sub_items.subTitle;
			GetJsonByExcelProp(js_sub_item, sub_items.m_burnExcelTable);
			js_Item["参数配置项"].append(js_sub_item);
		}

		//算法模块名用到的图片或字符串
		if(items->contentBeforeSubContent.size() == 1)
			if (items->contentBeforeSubContent[0].type == 0)
			{
				string str;
				for (const auto data : items->contentBeforeSubContent[0].data)
				{
					str.push_back(data);
				}
				str.push_back('\0');
				js_Item["描述"] = str;
			}

		js_Items_info.append(js_Item);
	}
	all_save_json["模块配置"] = js_Items_info;
	/*--------------模块烧录规则,参数配置--<*/

	/*--------------修改历史*/
	Json::Value js_changeHistory;
	GetJsonByExcelProp(js_changeHistory,m_GuideInfo.m_vecChangeHistroy);
	all_save_json["修改历史"] = js_changeHistory;
	/*--------------修改历史--<*/

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
		cout << "空的" << endl;
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

	vecChangeHistroy.m_vecHeaderLabels.push_back("版本");
	vecChangeHistroy.m_vecHeaderLabels.push_back("描述");
	vecChangeHistroy.m_vecHeaderLabels.push_back("日期");
	vecChangeHistroy.m_vecHeaderLabels.push_back("编辑者");
	vecChangeHistroy.m_vecHeaderLabels.push_back("审核");

	vector<string> table_line1;
	table_line1.push_back("V1.0.0.1");
	table_line1.push_back("初始版");
	table_line1.push_back("2020/4/1");
	table_line1.push_back("张敏");
	table_line1.push_back("蒋明杰");
	vecChangeHistroy.m_vecData.push_back(table_line1);

	table_line1.clear();
	table_line1.push_back("V1.0.0.2");
	table_line1.push_back("修改MTK PDAF Checksum 累加范围");
	table_line1.push_back("2020/4/3");
	table_line1.push_back("张敏");
	table_line1.push_back("蒋明杰");
	vecChangeHistroy.m_vecData.push_back(table_line1);

	table_line1.clear();
	table_line1.push_back("V1.0.0.3");
	table_line1.push_back("Qualcomm AWB/LSC AE更改为扣OB，增加测试项的详细信息。");
	table_line1.push_back("2020/4/9");
	table_line1.push_back("张敏");
	table_line1.push_back("蒋明杰");
	vecChangeHistroy.m_vecData.push_back(table_line1);

	OTPGuideInfo2Pdf(m_GuideInfo, "D:\\");*/
}
