#pragma once
#include <QMainWindow>
#include "ui_EEPROM_INIT.h"
#include <vector>
struct OTPGuideInfo;

class EEPROM_INIT : public QMainWindow
{
	Q_OBJECT

public:
	EEPROM_INIT(OTPGuideInfo& guide_info,QWidget *parent = Q_NULLPTR);
	~EEPROM_INIT();

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
	void callback_textChangedBurnDefaultVal(QString);
};
