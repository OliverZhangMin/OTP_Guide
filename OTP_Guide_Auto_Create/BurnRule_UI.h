#pragma once

#include <QWidget>
#include "ui_BurnRule_UI.h"
#include <string>
#include <vector>
#include <map>
#include <json.h>
#include<QComboBox>

struct BurnItem;

enum UI_TYPE
{
	BurnRule = 1,
	CheckSumRange,
	ChecksumAddr
};

enum BurnMedium
{
	EEPROM_Medium = 0,
	SENSOR_Medium,
};

struct CSubBurnProp
{
	int address = 0;
	int length = 0;
	std::string m_strData ="";
	std::string m_strSplit ="";
};

class BurnRule_UI : public QWidget
{
	Q_OBJECT

public:
	BurnRule_UI(BurnItem& burnItem,QWidget *parent = Q_NULLPTR);
	~BurnRule_UI();
	void ShowExcel(UI_TYPE type);
	void ShowBurnRuleExcel();
	void ShowCheckSumRange();
	void ShowCheckSumAddr();
	bool GetRuleJson(Json::Value& out_json);
	bool CommonConvert(Json::Value& out_json, const CSubBurnProp& prop, BurnMedium);
private:
	Ui::BurnRule_UI ui;
	std::map<std::string, std::string> m_mapJsonBurnKeyString;
	std::map<std::string, std::vector<std::string>> m_mapVecJsonStruct;

	QList<QString> m_QlistSplit;				//拆分类型下拉框
	//QList<QString> m_QlistChecksumDataSource;	//checksum数据来源下拉框
	QList<QString> m_QlistChecksumAddrOrder;	//chekcsum计算数据后拆分排序
	QList<QString> m_QlistChecksumCalType;		//checksum计算类型

	std::vector<std::vector<std::string>>& m_vecBurnRule;	
	std::vector<std::vector<std::string>>& m_vecCheckSumRange;
	std::vector<std::vector<std::string>>& m_vecCheckSumAddr;
	std::vector<std::string>& m_vecBurnRuleHeaderLabels;
	std::vector<std::string>& m_vecCheckSumRangeHeaderLabels;
	std::vector<std::string>& m_vecCheckSumAddrHeaderLabels;

	QLineEdit* p_lineEdit = nullptr;
protected:
	virtual void keyPressEvent(QKeyEvent * k) override;
	int m_iLastRow = -1;
	int m_iLastCol = -1;
private slots:
	void callback_BurnRlueCurrentItemChanged(QTableWidgetItem*, QTableWidgetItem*);
	void callback_BurnRuleItemChanged(QTableWidgetItem*);	//刷新数据结构用

	void callback_DataFilterTextChanged(QString);
	void callback_DataTypeCurrentTextChanged(QString);
	void callback_DataSourceCurrentTextChanged(QString);

	void callback_ChecksumRangeCurrentItemChanged(QTableWidgetItem*, QTableWidgetItem*);
	void callback_ChecksumRangeItemChanged(QTableWidgetItem*);	//刷新数据结构用

	void callback_ChecksumAddrCurrentItemChanged(QTableWidgetItem*, QTableWidgetItem*);
	void callback_ChecksumAddrItemChanged(QTableWidgetItem*);	//刷新数据结构用
};


class MyCombox :public QComboBox
{
	Q_OBJECT
public:
	MyCombox(QList<QString> qlist, QString qstr, string& str, BurnRule_UI* p,UI_TYPE type);
private:
	BurnRule_UI* m_pBurnRule = nullptr;
	string& m_str;
	UI_TYPE ui_type;
private slots:
	void callback_currentTextChanged(QString);
};