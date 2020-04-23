#pragma once

#include <QWidget>
#include "ui_TestItemContainer_UI.h"
#include<json.h>

struct OTPGuideInfo;

class TestItemContainer_UI : public QWidget
{
	Q_OBJECT

public:
	TestItemContainer_UI(OTPGuideInfo& guide_info,QWidget *parent = Q_NULLPTR);
	~TestItemContainer_UI();
	Json::Value GetJsons();
private:
	Ui::TestItemContainer_UI ui;
	QMenu * m_pTestItemContainertMenu = nullptr;
	OTPGuideInfo& m_guideInfo;
private slots:
	void callback_customContextMenuRequested(QPoint);
	void TestItemInsert();
};
