#pragma once
#include "OTPGuideDataStruct.h"
#include <QtWidgets/QMainWindow>
#include "MyWidgetBase.h"
#include "ui_OTP_Guide_Auto_Create.h"
#include <memory>
#include <string>
#include <map>

//struct OTPGuideInfo;

class OTP_Guide_Auto_Create : public QMainWindow
{
	Q_OBJECT

public:
	OTP_Guide_Auto_Create(QWidget *parent = Q_NULLPTR);
private:
	Ui::OTP_Guide_Auto_CreateClass ui;
	/*std::shared_ptr<ProjectInformation_UI> m_ProjectInformation;
	std::shared_ptr<EEPROM_INIT> m_pEEPROM_init;
	std::shared_ptr<EEPROM_Addr_Map> m_pEEPROM_addr_Map;
	std::shared_ptr<TestItemContainer_UI> m_pBurnTestItem;*/
	std::map<std::string, std::shared_ptr<CMyWidgetBase>> m_mapWidgets;
	OTPGuideInfo m_GuideInfo;
private slots:
	void callback_currentChanged(int);
	void callback_GenerateOTPGuide();
	void SaveConfig();
	void SaveBurnJsonFile();
};
