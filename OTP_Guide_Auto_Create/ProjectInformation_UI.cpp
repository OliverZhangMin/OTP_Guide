#include "stdafx.h"
#include "ProjectInformation_UI.h"


ProjectInformation_UI::ProjectInformation_UI(OTPGuideInfo& guide_info, QWidget *parent):Qt_Excel(guide_info.m_vecProjectInfo), m_GuideInfo(guide_info)
{
	//HideTextDesc();
	boost::assign::push_back(m_vecImportantNames)("机种名称")("模组类型")("Sensor")("烧录介质")("平台")("EEPROM SlaveID")("Sensor SlaveID");	//关键的字段
	boost::assign::push_back(m_vecHeaderLabels)("内容")("信息")("备注");	//表头
	
	if (guide_info.m_vecProjectInfo.m_vecHeaderLabels.size() == 0)
	{
		guide_info.m_vecProjectInfo.m_vecHeaderLabels = m_vecHeaderLabels;
	}
	if (guide_info.m_vecProjectInfo.m_vecData.size() == 0)
	{
		int Lables_size = m_vecHeaderLabels.size();
		for (const auto& name: m_vecImportantNames)
		{
			vector<string> vec_tmp;
			vec_tmp.push_back(name);
			for (int j = 1; j < Lables_size; j++)
				vec_tmp.push_back("");

			guide_info.m_vecProjectInfo.m_vecData.push_back(vec_tmp);		//插入一行数据的数据结构
		}
	}

	this->ShowExcel();
}


ProjectInformation_UI::~ProjectInformation_UI()
{

}

void ProjectInformation_UI::ShowExcel()
{
	Qt_Excel::ShowExcel();
	////模板中的5个参数不能被修改
	//for (int row = 0; row < ui.m_tableWidget->rowCount(); row++)
	//{
	//	string str_buff = ui.m_tableWidget->item(row, 0)->text().toLocal8Bit().data();
	//	if()
	//}
}

void ProjectInformation_UI::callback_itemDoubleClicked(QTableWidgetItem* item)
{
	int col = item->column();
	if (col == 0)
	{
		string str_text = item->text().toLocal8Bit().data();
		if (std::find_if(m_vecImportantNames.begin(), m_vecImportantNames.end(), [str_text](const string& str)
		{
			return str == str_text;
		}) != m_vecImportantNames.end())
		{
			return;
		}
	}
	Qt_Excel::callback_itemDoubleClicked(item);
}

//void ProjectInformation_UI::callback_itemSelectionChanged()
//{
//	//m_SelectItems = ui.m_tableWidget->selectedItems();
//	//m_iLastSelectRow = m_SelectItems[0]->row();
//	//m_iLastSelectCol = m_SelectItems[0]->column();
//	//cout << "row:" << m_iLastSelectRow << ",col:" << m_iLastSelectCol << endl;
//
//	//if (m_SelectItems.size() == 0)
//	//	return;
//	//string str_buff = m_SelectItems[0]->text().toLocal8Bit().data();
//	//if (m_SelectItems[0]->column() == 0)
//	//{
//	//	if (std::find_if(m_vecImportantNames.begin(), m_vecImportantNames.end(), [str_buff](string& str_tmp) {
//	//		return str_tmp == str_buff;
//	//	}) != m_vecImportantNames.end())
//	//	{
//	//		ui.m_textEdit->setEnabled(false);
//	//		return;
//	//	}
//	//	else
//	//	{
//	//		ui.m_textEdit->setEnabled(true);
//	//		//Qt_Excel::callback_itemSelectionChanged();
//	//	}
//	//}
//	//else if (m_SelectItems[0]->column() == 1)
//	//{
//	//	if (ui.m_tableWidget->item(m_SelectItems[0]->row(), 0)->text() == QString::fromLocal8Bit("机种名称"))
//	//	{
//	//		ui.m_textEdit->setEnabled(false);
//	//		return;
//	//	}
//	//	ui.m_textEdit->setEnabled(true);
//	//}
//	//else
//	//{
//	//	ui.m_textEdit->setEnabled(true);
//	//	//Qt_Excel::callback_itemSelectionChanged();
//	//}
//	//Qt_Excel::callback_itemSelectionChanged();
//}

//void ProjectInformation_UI::callback_itemClicked(QTableWidgetItem* item)
//{
//	m_iLastSelectRow = item->row();
//	m_iLastSelectCol = item->column();
//	cout << "row:" << m_iLastSelectRow << ",col:" << m_iLastSelectCol << endl;
//
//	string str_buff = item->text().toLocal8Bit().data();
//	if (m_iLastSelectCol == 0)
//	{
//		if (std::find_if(m_vecImportantNames.begin(), m_vecImportantNames.end(), [str_buff](string& str_tmp) {
//			return str_tmp == str_buff;
//		}) != m_vecImportantNames.end())
//		{
//			ui.m_textEdit->setEnabled(false);
//			return;
//		}
//		else
//			ui.m_textEdit->setEnabled(true);
//	}
//	//else if (m_iLastSelectCol == 1)
//	//{
//	//	if (ui.m_tableWidget->item(m_iLastSelectRow, 0)->text() == QString::fromLocal8Bit("机种名称"))
//	//	{
//	//		ui.m_textEdit->setEnabled(false);
//	//		ui.m_tableWidget->setFocus();
//	//		return;
//	//	}
//	//	else
//	//		ui.m_textEdit->setEnabled(true);
//	//}
//	else
//		ui.m_textEdit->setEnabled(true);
//	
//	Qt_Excel::callback_itemClicked(item);
//}

//void ProjectInformation_UI::callback_textChanged()
//{
//	/*if (ui.m_tableWidget->rowCount() == 0)
//		return;
//	auto str = ui.m_textEdit->document()->toPlainText();
//	if (m_iLastSelectCol < 0 || m_iLastSelectCol >= m_outExcel.m_vecHeaderLabels.size())
//	{
//		string str_log = (boost::format("当前修改的第%d行,第%d列,但是当前软件并没有维护到该列") % m_iLastSelectRow % m_iLastSelectCol).str();
//		QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
//		return;
//	}*/
//	Qt_Excel::callback_textChanged();	//将界面上的数据,刷新到结构体中
//	if (m_outExcel.m_vecData.empty())
//		return;
//	if (m_outExcel.m_vecData[m_iLastSelectRow][0] == "机种名称")
//		if (m_iLastSelectCol == 1)
//			m_GuideInfo.m_strProjectName = m_outExcel.m_vecData[m_iLastSelectRow][1];
//}