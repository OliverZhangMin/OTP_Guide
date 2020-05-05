#pragma once

#include "MyWidgetBase.h"
#include "ui_ChecksumConfigurate_ui.h"
#include <vector>
#include <string>
#include <QComboBox>
using namespace std;
struct BurnItem;

class ChecksumConfigurate_ui : public CMyWidgetBase
{
	Q_OBJECT

public:
	ChecksumConfigurate_ui(BurnItem& item, QWidget *parent = Q_NULLPTR);
	~ChecksumConfigurate_ui();
	virtual void UpdataWidget() override;
	void ShowExcel();
private:
	BurnItem& m_Item;
	Ui::ChecksumConfigurate_ui ui;
	//std::vector<std::vector<std::string>>& m_vecCheckSumRange;
	QList<QString> m_QlistEEPROM_DataSource;				//EEPROM������Դ����������
	int m_iLastRow = -1;
	int m_iLastCol = -1;
private slots:
	void callback_itemChanged(QTableWidgetItem*);			//���ؼ����ݱ䶯ʱ,�������ݽṹ
	void callback_customContextMenuRequested(QPoint);
	virtual void keyPressEvent(QKeyEvent * k);				//�����ɾ����
	void callback_DataSourceCurrentItemChanged(QTableWidgetItem*, QTableWidgetItem*);	//����Դ�䶯
};


class MyEEPROMDataSourceComboBox :public QComboBox
{
	Q_OBJECT
public:
	MyEEPROMDataSourceComboBox(QList<QString> qlist, QString qstr, string& str, ChecksumConfigurate_ui* p);
private:
	ChecksumConfigurate_ui* m_pBurnRule = nullptr;
	string& m_str;
	private slots:
	void callback_currentTextChanged(QString);
};