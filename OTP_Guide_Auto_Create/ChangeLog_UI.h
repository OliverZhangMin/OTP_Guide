#pragma once
#include "Qt_Excel.h"

struct ExcelProp;

class ChangeLog_UI :
	public Qt_Excel
{
	Q_OBJECT
public:
	ChangeLog_UI(OTPGuideInfo&);
	~ChangeLog_UI();
private:
	OTPGuideInfo& m_GuideInfo;
protected:
	virtual void ShowExcel() override;
	virtual void callback_itemDoubleClicked(QTableWidgetItem*) override;
//protected slots:
//	virtual void callback_itemClicked(QTableWidgetItem*) override;
//	virtual void callback_textChanged();
};

