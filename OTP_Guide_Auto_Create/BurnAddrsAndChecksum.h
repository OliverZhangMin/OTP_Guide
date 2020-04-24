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
	BurnAddrsAndChecksum(OTPGuideInfo& guide_info, EEPROM_INIT* p_init, QWidget *parent = Q_NULLPTR);
	~BurnAddrsAndChecksum();
	void AddCheckSumConfigInfo(BurnItem&);
	void ShowExcel();
protected:
	virtual void keyPressEvent(QKeyEvent * k);
private:
	Ui::BurnAddrsAndChecksum ui;
	std::vector<std::vector<std::string>> m_vecBurnAddrs;
	OTPGuideInfo& m_GuideInfo;
	QMenu* m_pMenuCheckSumConfig = nullptr;
	EEPROM_INIT* m_pInitWidget = nullptr;
private slots:
	void callback_BurnAddrWidgetItemChanged(QTableWidgetItem*);
	void callback_DeleteCurrentCheckSumConfig();				//删除当前选中的chekcsum配置界面
	void callback_ChecksumConfigcustomContextMenuRequested(QPoint);
};
