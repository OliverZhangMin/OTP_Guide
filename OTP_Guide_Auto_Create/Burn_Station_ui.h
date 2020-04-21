#pragma once
#include "Qt_Excel.h"

struct ExcelProp;

class Burn_Station_ui :
	public Qt_Excel
{
	Q_OBJECT
public:
	Burn_Station_ui(ExcelProp&, QWidget *parent = nullptr);
	~Burn_Station_ui();
protected:
	virtual void ShowExcel() override;
	virtual bool InsertOneRow() override;
protected slots :
	virtual void callback_textChanged() override;
};

