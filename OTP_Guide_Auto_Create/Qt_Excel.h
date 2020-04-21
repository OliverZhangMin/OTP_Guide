#pragma once
#include <QWidget>
#include "ui_Qt_Excel.h"

using namespace std;
struct ExcelProp;

class Qt_Excel : public QWidget
{
	Q_OBJECT

public:
	Qt_Excel(ExcelProp&,QWidget *parent = Q_NULLPTR);
	~Qt_Excel();
private:
	
	int m_iLastSelectRow = -1;
	int m_iLastSelectCol = -1;
protected:
	Ui::Qt_Excel ui;
	ExcelProp& m_outExcel;
	QList<QTableWidgetItem*> m_SelectItems;
	virtual void keyPressEvent(QKeyEvent * k);
	virtual void ShowExcel();
	virtual bool InsertOneRow();

	std::vector<std::string> m_vecImportantNames;
	std::vector<std::string> m_vecHeaderLabels;
protected slots :
	void callback_itemChanged(QTableWidgetItem*);
	virtual void callback_textChanged();
	virtual void callback_itemSelectionChanged();
};
