#include "stdafx.h"
#include "ChangeLog_UI.h"
#include <chrono>

ChangeLog_UI::ChangeLog_UI(OTPGuideInfo& guide_info):
	m_GuideInfo(guide_info),
	Qt_Excel(guide_info.m_vecChangeHistroy)
{
	if (m_outExcel.m_vecHeaderLabels.empty())
		boost::assign::push_back(m_outExcel.m_vecHeaderLabels)("版本")("描述")("日期")("编辑者")("审核");
	this->ShowExcel();
}

ChangeLog_UI::~ChangeLog_UI()
{

}

void ChangeLog_UI::ShowExcel()
{
	Qt_Excel::ShowExcel();
	int rows = ui.m_tableWidget->rowCount(), cols = ui.m_tableWidget->columnCount();
	/*---------------除了最后一行,其他行都不能被修改*/
	for (int i = 0; i < rows-1; i++)
		for (int j = 0; j < cols; j++)
			ui.m_tableWidget->item(i, j)->setFlags(0);
	if (rows >= 1)
	{
		//ui.m_tableWidget->item(rows-1, 0)->setFlags(0);
		ui.m_tableWidget->item(rows - 1, 2)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	}
	/*---------------除了最后一行,其他行都不能被修改--<*/
}

void ChangeLog_UI::callback_itemDoubleClicked(QTableWidgetItem* item)
{
	int row = item->row();
	int col = item->column();
	int can_alter_row = ui.m_tableWidget->rowCount() - 1;
	if (row == can_alter_row)
	{
		if (col == 2)
		{
			auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			struct tm* ptm = localtime(&tt);
			m_outExcel.m_vecData[row][col] = (boost::format("%d/%d/%d")%(ptm->tm_year + 1900) %(ptm->tm_mon + 1) %(ptm->tm_mday)).str();
			ShowExcel();
		}
	}
}