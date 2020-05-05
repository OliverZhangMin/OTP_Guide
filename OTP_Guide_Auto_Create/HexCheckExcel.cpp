#include "stdafx.h"
#include "HexCheckExcel.h"


CHexCheckExcel::CHexCheckExcel(ExcelProp& excel, QWidget *parent) :Qt_Excel(excel)
{
	ShowExcel();
}


CHexCheckExcel::~CHexCheckExcel()
{
}

void CHexCheckExcel::ShowExcel()
{
	Qt_Excel::ShowExcel();
	for (int row = 0; row < m_outExcel.m_vecData.size(); row++)
		for (int col = 0; col < m_outExcel.m_vecData[row].size(); col++)
		{
			string str = ui.m_tableWidget->item(row, col)->text().toLocal8Bit().data();
			if (IsHex(str))
				ui.m_tableWidget->item(row, col)->setBackgroundColor(Qt::green);
			else
				ui.m_tableWidget->item(row, col)->setBackgroundColor(Qt::red);
		}
}