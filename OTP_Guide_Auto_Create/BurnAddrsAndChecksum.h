#pragma once

#include "MyWidgetBase.h"
#include "ui_BurnAddrsAndChecksum.h"
#include "EEPROM_INIT.h"
#include<vector>
#include<string>

struct OTPGuideInfo;
struct BurnItem;
class EEPROM_INIT;


class BurnAddrsAndChecksum : public CMyWidgetBase
{
	Q_OBJECT
public:
	BurnAddrsAndChecksum(OTPGuideInfo& guide_info, EEPROM_INIT* p_init, string station_name ,QWidget *parent = Q_NULLPTR);
	~BurnAddrsAndChecksum();
	void AddCheckSumConfigInfo(BurnItem&);
	virtual void UpdataWidget() override;  //����UI����
	//void ShowExcel();
	Ui::BurnAddrsAndChecksum ui;
protected:
	//virtual void keyPressEvent(QKeyEvent * k);
private:
	string m_strStationName = "";
	ExcelProp& m_BurnExcel;
	//Qt_Excel& m_BurnExcel;
	//std::vector<std::vector<std::string>> m_vecBurnAddrs;
	OTPGuideInfo& m_GuideInfo;
	QMenu* m_pMenuCheckSumConfig = nullptr;
	QMenu* m_pMenuBurnItem = nullptr;
	EEPROM_INIT* m_pInitWidget = nullptr;
	int m_iLastSelectRow = -1;
	int m_iLastSelectCol = -1;
	//bool InsertOneRow();
private slots:
	//void callback_BurnAddrWidgetItemChanged(QTableWidgetItem*);
	void callback_DeleteCurrentCheckSumConfig();				//ɾ����ǰѡ�е�chekcsum���ý���
	void callback_ChecksumConfigcustomContextMenuRequested(QPoint);

	void callback_DeleteBurnItem();				
	void callback_BurnItemCustomContextMenuRequested(QPoint);	//��վ��Ҫ��¼��ģ���б��,�Ҽ��˵�
	void callback_ItemClicked(QTableWidgetItem*);
};
