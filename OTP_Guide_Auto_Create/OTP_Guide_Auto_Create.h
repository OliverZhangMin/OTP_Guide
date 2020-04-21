#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OTP_Guide_Auto_Create.h"
#include "EEPROM_Addr_Map.h"
#include "EEPROM_INIT.h"
#include "ProjectInformation_UI.h"
#include "TestItemContainer_UI.h"
#include <memory>

struct OTPGuideInfo;

class OTP_Guide_Auto_Create : public QMainWindow
{
	Q_OBJECT

public:
	OTP_Guide_Auto_Create(QWidget *parent = Q_NULLPTR);

private:
	Ui::OTP_Guide_Auto_CreateClass ui;
	std::shared_ptr<ProjectInformation_UI> m_ProjectInformation;
	std::shared_ptr<EEPROM_INIT> m_pEEPROM_init;
	std::shared_ptr<EEPROM_Addr_Map> m_pEEPROM_addr_Map;
	std::shared_ptr<TestItemContainer_UI> m_pBurnTestItem;
	OTPGuideInfo m_GuideInfo;
private slots:
	void callback_currentChanged(int);
};