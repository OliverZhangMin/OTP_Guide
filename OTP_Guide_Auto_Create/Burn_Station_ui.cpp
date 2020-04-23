#include "stdafx.h"
#include "Burn_Station_ui.h"


Burn_Station_ui::Burn_Station_ui(ExcelProp& prop, QWidget *parent):Qt_Excel(prop)
{
	HideTextDesc();
	boost::assign::push_back(m_vecHeaderLabels)("烧录段起始地址")("烧录段结束地址");
	if (prop.m_vecHeaderLabels.size() == 0)
		prop.m_vecHeaderLabels = m_vecHeaderLabels;
	//connect(ui.m_textEdit, SIGNAL(textChanged()), (Burn_Station_ui*)this, SLOT(callback_textChanged));
}

Burn_Station_ui::~Burn_Station_ui()
{
	//
}

void Burn_Station_ui::ShowExcel()
{
	Qt_Excel::ShowExcel();
	for (int row = 0; row < ui.m_tableWidget->rowCount(); row++)
	{
		for (int col = 0; col < ui.m_tableWidget->columnCount(); col++)
		{
			if (!IsHex(ui.m_tableWidget->item(row, col)->text().toLocal8Bit().data()))
			{
				ui.m_tableWidget->item(row, col)->setBackgroundColor(QColor(Qt::red));
			}
			else
			{
				ui.m_tableWidget->item(row, col)->setBackgroundColor(QColor(Qt::green));
			}
		}
	}
}

void Burn_Station_ui::callback_textChanged()
{
	Qt_Excel::callback_textChanged();
	/*string str = ui.m_textEdit->document()->toPlainText().toLocal8Bit().data();
	if (!IsHex(str))
	{
		if (str == "0x")
		{
			Qt_Excel::callback_textChanged();
			return;
		}
		else
		{
			string str_log = (boost::format("%s[ERROR]:输入的必须是像0x1B这样的16进制字符串") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			ui.m_textEdit->setText("0x");
			ui.m_textEdit->setFocus();
			QTextCursor cursor = ui.m_textEdit->textCursor();
			cursor.movePosition(QTextCursor::End);
			ui.m_textEdit->setTextCursor(cursor);
			return;
		}
	}
	else
	{
		Qt_Excel::callback_textChanged();
	}*/
	ShowExcel();
}

bool Burn_Station_ui::InsertOneRow()
{
	auto selectItems = ui.m_tableWidget->selectedItems();

	vector<string> vec_str(m_outExcel.m_vecHeaderLabels.size());
	vec_str[0] = "0x";
	vec_str[1] = "0x";
	if (selectItems.size() == 0)
		m_outExcel.m_vecData.push_back(vec_str);
	else if (selectItems.size() == 1)
		m_outExcel.m_vecData.insert(m_outExcel.m_vecData.begin() + selectItems[0]->row(), vec_str);
	return true;
}