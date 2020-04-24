#include "stdafx.h"
#include "OTP_Guide_Auto_Create.h"
#include <Windows.h>

OTP_Guide_Auto_Create::OTP_Guide_Auto_Create(QWidget *parent)
	: QMainWindow(parent)
{
	QToolBar* toolBar = new QToolBar(this);
	QAction* SaveGuideAction = new QAction(QString::fromLocal8Bit("生成guide"), this);
	toolBar->addAction(SaveGuideAction);
	connect(SaveGuideAction, &QAction::triggered, this, &OTP_Guide_Auto_Create::callback_GenerateOTPGuide);
	this->addToolBar(toolBar);


	ui.setupUi(this);
	AllocConsole();                     // 打开控制台资源
	freopen("CONOUT$", "w+t", stdout);// 申请写

	m_mapWidgets["EEPROM映射表"].reset(new EEPROM_Addr_Map(m_GuideInfo));
	m_mapWidgets["烧录初始配置"].reset(new EEPROM_INIT(m_GuideInfo));
	m_mapWidgets["项目信息"].reset(new ProjectInformation_UI(m_GuideInfo));
	m_mapWidgets["烧录测试项信息"].reset(new TestItemContainer_UI(m_GuideInfo));

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

void OTP_Guide_Auto_Create::callback_GenerateOTPGuide()
{
	((TestItemContainer_UI*)ui.m_tabWidget->widget(3))->GetJsons();
}
