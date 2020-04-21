#include "stdafx.h"
#include "OTP_Guide_Auto_Create.h"

OTP_Guide_Auto_Create::OTP_Guide_Auto_Create(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_ProjectInformation.reset(new ProjectInformation_UI(m_GuideInfo));		//项目信息
	m_pEEPROM_init.reset(new EEPROM_INIT(m_GuideInfo));						//初始化
	m_pEEPROM_addr_Map.reset(new EEPROM_Addr_Map(m_GuideInfo));
	ui.m_tabWidget->addTab(m_pEEPROM_addr_Map.get(), QString::fromLocal8Bit("EEPROM映射表"));
	ui.m_tabWidget->addTab(m_pEEPROM_init.get(), QString::fromLocal8Bit("init"));
	ui.m_tabWidget->addTab(m_ProjectInformation.get(), QString::fromLocal8Bit("项目信息"));
}

void OTP_Guide_Auto_Create::callback_currentChanged(int index)
{
	m_ProjectInformation->ShowExcel();
}