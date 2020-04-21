#pragma once

#include <QMainWindow>
#include "ui_EEPROM_Addr_Map.h"
struct OTPGuideInfo;

class EEPROM_Addr_Map : public QMainWindow
{
	Q_OBJECT

public:
	EEPROM_Addr_Map(OTPGuideInfo&,QWidget *parent = Q_NULLPTR);
	~EEPROM_Addr_Map();

private:
	Ui::EEPROM_Addr_Map ui;
	OTPGuideInfo& m_GuideInfo;
};
