#include "stdafx.h"
#include "EEPROM_Addr_Map.h"

EEPROM_Addr_Map::EEPROM_Addr_Map(OTPGuideInfo& guide_info,QWidget *parent)
	: QMainWindow(parent),m_GuideInfo(guide_info)
{
	ui.setupUi(this);
}

EEPROM_Addr_Map::~EEPROM_Addr_Map()
{

}