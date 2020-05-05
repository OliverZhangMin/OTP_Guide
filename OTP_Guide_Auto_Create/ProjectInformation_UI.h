#pragma once
#include "Qt_Excel.h"
#include<vector>
#include<string>
struct OTPGuideInfo;

class ProjectInformation_UI :
	public Qt_Excel
{
	Q_OBJECT
public:
	ProjectInformation_UI(OTPGuideInfo&, QWidget *parent = Q_NULLPTR);
	~ProjectInformation_UI();
	virtual void ShowExcel() override;
private:
	OTPGuideInfo& m_GuideInfo;
	std::vector<std::string> m_vecImportantNames;
	std::vector<std::string> m_vecHeaderLabels;
protected slots:
	virtual void callback_itemDoubleClicked(QTableWidgetItem*);
	//virtual void callback_itemClicked(QTableWidgetItem*) override;
	//virtual void callback_textChanged();
};

