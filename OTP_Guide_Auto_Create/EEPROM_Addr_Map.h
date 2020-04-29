#pragma once

#include <QMainWindow>
#include "ui_EEPROM_Addr_Map.h"
#include "MyWidgetBase.h"
#include <vector>
struct OTPGuideInfo;

class EEPROM_Addr_Map : public CMyWidgetBase
{
	Q_OBJECT

public:
	EEPROM_Addr_Map(OTPGuideInfo&,QWidget *parent = Q_NULLPTR);
	~EEPROM_Addr_Map();
	void ShowExcel();
	virtual void UpdataWidget() override;

protected:
	virtual void keyPressEvent(QKeyEvent * k);
	
private:
	Ui::EEPROM_Addr_Map ui;
	OTPGuideInfo& m_GuideInfo;
	std::map<int, std::vector<std::pair<int, int>>>& m_mapSpan;	//每列被合并的地址区间
	int FindRowByAddr(const int& addr);
private slots:
	void callback_AddrsItemChanged(QTableWidgetItem*);
};
