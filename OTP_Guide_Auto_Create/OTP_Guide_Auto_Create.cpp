#include "stdafx.h"
#include "OTP_Guide_Auto_Create.h"
#include <Windows.h>

OTP_Guide_Auto_Create::OTP_Guide_Auto_Create(QWidget *parent)
	: QMainWindow(parent)
{
	QToolBar* toolBar = new QToolBar(this);
	QAction* SaveGuideAction = new QAction(QString::fromLocal8Bit("����guide"), this);
	toolBar->addAction(SaveGuideAction);
	connect(SaveGuideAction, &QAction::triggered, this, &OTP_Guide_Auto_Create::callback_GenerateOTPGuide);
	this->addToolBar(toolBar);


	ui.setupUi(this);
	AllocConsole();                     // �򿪿���̨��Դ
	freopen("CONOUT$", "w+t", stdout);// ����д

	m_ProjectInformation.reset(new ProjectInformation_UI(m_GuideInfo));		//��Ŀ��Ϣ
	m_pEEPROM_init.reset(new EEPROM_INIT(m_GuideInfo));						//��ʼ��
	m_pEEPROM_addr_Map.reset(new EEPROM_Addr_Map(m_GuideInfo));
	m_pBurnTestItem.reset(new TestItemContainer_UI(m_GuideInfo));
	ui.m_tabWidget->addTab(m_pEEPROM_addr_Map.get(), QString::fromLocal8Bit("EEPROMӳ���"));
	ui.m_tabWidget->addTab(m_pEEPROM_init.get(), QString::fromLocal8Bit("init"));
	ui.m_tabWidget->addTab(m_ProjectInformation.get(), QString::fromLocal8Bit("��Ŀ��Ϣ"));
	ui.m_tabWidget->addTab(m_pBurnTestItem.get(), QString::fromLocal8Bit("��¼��������Ϣ"));
}

void OTP_Guide_Auto_Create::callback_currentChanged(int index)
{
	m_ProjectInformation->ShowExcel();
	if (ui.m_tabWidget->tabText(index) == QString::fromLocal8Bit("init"))
	{

	}
}

void OTP_Guide_Auto_Create::callback_GenerateOTPGuide()
{
	((TestItemContainer_UI*)ui.m_tabWidget->widget(3))->GetJsons();
}
