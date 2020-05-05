#pragma once

#include <QWidget>
#include "ui_Burn_TestItem_UI.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/format.hpp>
#include <string>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
using namespace std;
class Ini_GuideTemplateNames
{
public:
	Ini_GuideTemplateNames();
	//根据用户提供的名字,推断出模板的内容
	bool GetTemplateDataByUserName(const string& str_userTestName, map<string, vector<string>>& map_vecName);
	map<string, vector<vector<string>>> GetDefaultSectionByTemplateINI();
private:
	vector<string> m_vecItemNames;
	boost::property_tree::wptree pt;
	string m_strFileName = "ini_test.ini";
};

struct BurnItem;

class Burn_TestItem_UI : public QWidget
{
	Q_OBJECT

public:
	Burn_TestItem_UI(std::shared_ptr<BurnItem>,QWidget *parent = Q_NULLPTR);
	~Burn_TestItem_UI();

private:
	std::string m_strLabelName = "";
	std::shared_ptr<BurnItem> m_curBurnItem;
	
	map<string, vector<vector<string>>> m_mapTemplateSections;
	vector<string>m_vecSectionName;
	Ini_GuideTemplateNames ini_guide;
	Ui::Burn_TestItem_UI ui;
	int i_currentIndex = -1;
protected:
	void keyPressEvent(QKeyEvent * k) override;
	virtual void dragEnterEvent(QDragEnterEvent *event) override;
	virtual void dropEvent(QDropEvent *event) override;
	
private slots:
	void callback_doubleClick(QModelIndex);
	void callback_addClick();
	void callback_tabBarClicked(int);
	void callback_DescTextEditTextChanged();
};