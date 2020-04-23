#pragma once

#include <QWidget>
#include "ui_BurnAddrsAndChecksum.h"
#include<vector>
#include<string>
struct OTPGuideInfo;

class BurnAddrsAndChecksum : public QWidget
{
	Q_OBJECT
public:
	BurnAddrsAndChecksum(OTPGuideInfo& guide_info,QWidget *parent = Q_NULLPTR);
	~BurnAddrsAndChecksum();
	void ShowExcel();
protected:
	virtual void keyPressEvent(QKeyEvent * k);
private:
	Ui::BurnAddrsAndChecksum ui;
	std::vector<std::vector<std::string>> m_vecBurnAddrs;

};
