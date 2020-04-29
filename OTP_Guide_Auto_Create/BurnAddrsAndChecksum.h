#pragma once

#include <QWidget>
#include "ui_BurnAddrsAndChecksum.h"
#include "EEPROM_INIT.h"
#include<vector>
#include<string>
struct OTPGuideInfo;
struct BurnItem;
class EEPROM_INIT;

class BurnAddrsAndChecksum : public QWidget
{
	Q_OBJECT
public:
	BurnAddrsAndChecksum(OTPGuideInfo& guide_info, EEPROM_INIT* p_init, string station_name ,QWidget *parent = Q_NULLPTR);
	~BurnAddrsAndChecksum();
	void AddCheckSumConfigInfo(BurnItem&);
	void ShowExcel();
	Ui::BurnAddrsAndChecksum ui;
protected:
	virtual void keyPressEvent(QKeyEvent * k);
private:
	ExcelProp& m_BurnExcel;
	//Qt_Excel& m_BurnExcel;
	//std::vector<std::vector<std::string>> m_vecBurnAddrs;
	OTPGuideInfo& m_GuideInfo;
	QMenu* m_pMenuCheckSumConfig = nullptr;
	EEPROM_INIT* m_pInitWidget = nullptr;
	int m_iLastSelectRow = -1;
	int m_iLastSelectCol = -1;
	bool InsertOneRow();
private slots:
	void callback_BurnAddrWidgetItemChanged(QTableWidgetItem*);
	void callback_DeleteCurrentCheckSumConfig();				//删除当前选中的chekcsum配置界面
	void callback_ChecksumConfigcustomContextMenuRequested(QPoint);
	void callback_ItemClicked(QTableWidgetItem*);
};
