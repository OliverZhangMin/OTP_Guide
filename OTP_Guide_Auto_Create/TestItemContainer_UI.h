#pragma once

#include <QWidget>
#include "ui_TestItemContainer_UI.h"
#include "MyWidgetBase.h"
#include <json.h>
#include <string>

struct OTPGuideInfo;

class TestItemContainer_UI : public CMyWidgetBase
{
	Q_OBJECT

public:
	TestItemContainer_UI(OTPGuideInfo& guide_info,QWidget *parent = Q_NULLPTR);
	~TestItemContainer_UI();
	Json::Value GetJsonByAlgoName(const std::string& str_algo);
	virtual void UpdataWidget() override;
private:
	Ui::TestItemContainer_UI ui;
	QMenu * m_pTestItemContainertMenu = nullptr;
	OTPGuideInfo& m_guideInfo;
private slots:
	void callback_customContextMenuRequested(QPoint);
	void TestItemInsert();
	void DeleteTestItem();
};
