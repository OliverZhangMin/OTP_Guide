#pragma once
#include <QWidget>
#include "ui_Qt_Excel.h"
#include "MyWidgetBase.h"
using namespace std;
struct ExcelProp;

class Qt_Excel : public CMyWidgetBase
{
	Q_OBJECT

public:
	Qt_Excel(ExcelProp&,QWidget *parent = Q_NULLPTR);
	~Qt_Excel();
	virtual void UpdataWidget() override;
private:
protected:
	Ui::Qt_Excel ui;
	ExcelProp& m_outExcel;
	QList<QTableWidgetItem*> m_SelectItems;
	virtual void keyPressEvent(QKeyEvent * k);
	virtual void ShowExcel();
	virtual bool InsertOneRow();
	int m_iLastSelectRow = -1;
	int m_iLastSelectCol = -1;
	std::vector<std::string> m_vecImportantNames;
	std::vector<std::string> m_vecHeaderLabels;
	//void HideTextDesc();
protected slots :
	virtual void callback_itemClicked(QTableWidgetItem*);
	virtual void callback_textChanged();
	virtual void callback_itemDoubleClicked(QTableWidgetItem*) {}
	//virtual void callback_itemSelectionChanged();
	//virtual void callback_cellClicked(int row, int col);
};
