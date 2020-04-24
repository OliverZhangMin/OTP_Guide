#pragma once

#include <QMainWindow>
#include "ui_EEPROM_Addr_Map.h"
#include "MyWidgetBase.h"
struct OTPGuideInfo;

class EEPROM_Addr_Map : public CMyWidgetBase
{
	Q_OBJECT

public:
	EEPROM_Addr_Map(OTPGuideInfo&,QWidget *parent = Q_NULLPTR);
	~EEPROM_Addr_Map();
	virtual void UpdataWidget() override;
private:
	Ui::EEPROM_Addr_Map ui;
	OTPGuideInfo& m_GuideInfo;
};
