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
	std::vector<std::string> m_vecImportantNames;
	std::vector<std::string> m_vecHeaderLabels;
protected slots:
	virtual void callback_itemSelectionChanged();
};

