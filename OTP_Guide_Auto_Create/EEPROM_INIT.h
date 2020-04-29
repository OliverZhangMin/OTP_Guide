#pragma once
#include <QMainWindow>
#include "ui_EEPROM_INIT.h"
#include "MyWidgetBase.h"
#include <vector>
struct OTPGuideInfo;

class EEPROM_INIT : public CMyWidgetBase
{
	Q_OBJECT

public:
	EEPROM_INIT(OTPGuideInfo& guide_info,QWidget *parent = Q_NULLPTR);
	~EEPROM_INIT();
	virtual void UpdataWidget() override;
private:
	Ui::EEPROM_INIT ui;
	QMenu * m_pBurnStationWidgetMenu = nullptr;
	OTPGuideInfo& m_GuideInfo;
protected:
	virtual void keyPressEvent(QKeyEvent * k);
private slots:
	void callback_BurnStationAddrWidget_customContextMenuRequested(QPoint);
	void BurnStationInsert();
	void callback_textChanged(QString);
	void callback_doubleClicked(QModelIndex);	//双击按下 未配置checksum的模块 的列表框
	void callback_ConfigBurnModelDoubleClicked(QModelIndex);
	void callback_ProtectcheckboxClick(bool);
	void callback_ProtectTextChanged(QString);
};
