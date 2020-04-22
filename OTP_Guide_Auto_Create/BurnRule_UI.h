#pragma once

#include <QMainWindow>
#include "ui_BurnRule_UI.h"
#include <string>
#include <vector>
#include <map>
#include<QComboBox>


enum UI_TYPE
{
	BurnRule = 1,
	CheckSumRange,
	ChecksumAddr
};

class BurnRule_UI : public QMainWindow
{
	Q_OBJECT

public:
	BurnRule_UI(QWidget *parent = Q_NULLPTR);
	~BurnRule_UI();
	void ShowExcel(UI_TYPE type);
	void ShowBurnRuleExcel();
	void ShowCheckSumRange();
	void ShowCheckSumAddr();
private:
	Ui::BurnRule_UI ui;
	std::map<std::string,std::string> m_mapJsonBurnKeyString;
	std::map<std::string, std::vector<std::string>> m_mapVecJsonStruct;

	QList<QString> m_QlistSplit;		//�������������
	QList<QString> m_QlistChecksumDataSource;	//checksum������Դ������
	QList<QString> m_QlistChecksumAddrOrder;	//chekcsum�������ݺ�������
	QList<QString> m_QlistChecksumCalType;		//checksum��������

	std::vector<std::vector<std::string>> m_vecBurnRule;	
	std::vector<std::vector<std::string>> m_vecCheckSumRange;
	std::vector<std::vector<std::string>> m_vecCheckSumAddr;

	std::vector<std::string> m_vecBurnRuleHeaderLabels;
	std::vector<std::string> m_vecCheckSumRangeHeaderLabels;
	std::vector<std::string> m_vecCheckSumAddrHeaderLabels;
	QLineEdit* p_lineEdit = nullptr;
protected:
	virtual void keyPressEvent(QKeyEvent * k) override;
	int m_iLastRow = -1;
	int m_iLastCol = -1;
private slots:
	void callback_BurnRlueCurrentItemChanged(QTableWidgetItem*, QTableWidgetItem*);
	void callback_BurnRuleItemChanged(QTableWidgetItem*);	//ˢ�����ݽṹ��

	void callback_DataFilterTextChanged(QString);
	void callback_DataTypeCurrentTextChanged(QString);
	void callback_DataSourceCurrentTextChanged(QString);

	void callback_ChecksumRangeCurrentItemChanged(QTableWidgetItem*, QTableWidgetItem*);
	void callback_ChecksumRangeItemChanged(QTableWidgetItem*);	//ˢ�����ݽṹ��

	void callback_ChecksumAddrCurrentItemChanged(QTableWidgetItem*, QTableWidgetItem*);
	void callback_ChecksumAddrItemChanged(QTableWidgetItem*);	//ˢ�����ݽṹ��
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