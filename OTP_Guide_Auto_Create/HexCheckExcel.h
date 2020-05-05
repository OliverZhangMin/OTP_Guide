#pragma once
#include "Qt_Excel.h"
class CHexCheckExcel :
	public Qt_Excel
{
	Q_OBJECT
public:
	CHexCheckExcel(ExcelProp&, QWidget *parent = Q_NULLPTR);
	~CHexCheckExcel();
protected:
	virtual void ShowExcel() override;
};