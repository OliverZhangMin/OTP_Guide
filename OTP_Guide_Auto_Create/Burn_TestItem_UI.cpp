#include "stdafx.h"
#include "Burn_TestItem_UI.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include <Windows.h>
#include <QKeyEvent>
#include <QMessageBox>
#include "Qt_Excel.h"
#include <boost/assign.hpp>
#include <QMimeData>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
Ini_GuideTemplateNames::Ini_GuideTemplateNames() {
	std::locale utf8Locale(std::locale(), new std::codecvt_utf8<wchar_t>);
	boost::property_tree::ini_parser::read_ini(m_strFileName, pt, utf8Locale);
	auto item = pt.get_child(L"TestItem");
	for (auto it = item.begin(); it != item.end(); it++)
		m_vecItemNames.push_back(wstring2string(pt.get<wstring>((boost::wformat(L"TestItem.%s") % it->first).str())));
}

bool Ini_GuideTemplateNames::GetTemplateDataByUserName(const string& str_userTestName, map<string, vector<string>>& map_vecName)
{
	map_vecName.clear();
	string low_str = boost::to_lower_copy(str_userTestName);
	auto iter = std::find_if(m_vecItemNames.begin(), m_vecItemNames.end(), [low_str](const string& str) {
		if (boost::contains(low_str, boost::to_lower_copy(str)))
			return true;
		else
			return false;
	});
	auto item = pt.get_child(string2wstring(*iter));
	for (auto it = item.begin(); it != item.end(); it++)
	{
		wstring item_name = it->first;
		wstring detail_item_section = pt.get<wstring>((boost::wformat(L"%s.%s") % string2wstring(*iter) % it->first).str());
		//获取该section下的所有字段
		auto detail_item = pt.get_child(detail_item_section);
		for (auto sub_it = detail_item.begin(); sub_it != detail_item.end(); sub_it++)
			map_vecName[wstring2string(item_name)].push_back(wstring2string(pt.get<wstring>((boost::wformat(L"%s.%s") % detail_item_section % sub_it->first).str())));
	}
	return true;
}

map<string,  vector<vector<string>>> Ini_GuideTemplateNames::GetDefaultSectionByTemplateINI()
{
	map<string, vector<vector<string>>> out_mapSections;
	//map<string, vector<subSection>> out_mapSections;
	auto item = pt.get_child(L"DefaultSection");
	for (auto it = item.begin(); it != item.end(); it++)
	{
		wstring item_name = it->first;
		wstring detail_item_section = pt.get<wstring>((boost::wformat(L"DefaultSection.%s")% it->first).str());
		//获取该section下的所有字段
		auto detail_item = pt.get_child(detail_item_section);
		for (auto sub_it = detail_item.begin(); sub_it != detail_item.end(); sub_it++)
		{
			vector<string> vec_tmp(3);
			/*subSection sc;
			sc.m_strItem = wstring2string(pt.get<wstring>((boost::wformat(L"%s.%s") % detail_item_section % sub_it->first).str()));*/
			vec_tmp[0]= wstring2string(pt.get<wstring>((boost::wformat(L"%s.%s") % detail_item_section % sub_it->first).str()));
			out_mapSections[wstring2string(item_name)].push_back(vec_tmp);
		}
	}
	return out_mapSections;
}

Burn_TestItem_UI::Burn_TestItem_UI(std::shared_ptr<BurnItem> burnItem, QWidget *parent)
	: m_curBurnItem(burnItem), QWidget(parent)
{
	ui.setupUi(this);
	m_mapTemplateSections = ini_guide.GetDefaultSectionByTemplateINI();						//获取默认的模板section
	for (const auto& it : m_mapTemplateSections)
		ui.m_UserSlectlistWidget->addItem(QString::fromLocal8Bit(it.first.c_str()));		//插入默认的模板section

	////根据传入的构造参数,创建tab
	//for (auto& config_param: m_ItemProp.m_mapConfigs)
	//{
	//	Qt_Excel* config_excel_ui = new Qt_Excel(config_param.second);
	//	//Qt_ItemConfigExcel* config_ui = new Qt_ItemConfigExcel(sect.second);		//插入section的数据结构到 tab表格中
	//	m_vecSectionName.push_back(config_param.first);										//记录该section的名字
	//	ui.m_tabWidget->addTab(config_excel_ui, QString::fromLocal8Bit(config_param.first.c_str()));
	//}
	for (auto& sub_item : m_curBurnItem->burnItemSubContents)
	{
		Qt_Excel* config_excel_ui = new Qt_Excel(sub_item.m_burnExcelTable /*.m_burnExcelTable*/);
		m_vecSectionName.push_back(sub_item.subTitle);										//记录该section的名字
		ui.m_tabWidget->addTab(config_excel_ui, QString::fromLocal8Bit(sub_item.subTitle.c_str()));
		ui.m_tabWidget->setCurrentIndex(ui.m_tabWidget->count() - 1);
	}
}

Burn_TestItem_UI::~Burn_TestItem_UI()
{
}

void Burn_TestItem_UI::dragEnterEvent(QDragEnterEvent *event)
{
	event->acceptProposedAction();
}

void Burn_TestItem_UI::dropEvent(QDropEvent *event)
{
	QString name = event->mimeData()->urls().first().toString();
	string str_name = name.toLocal8Bit().data();
	str_name = str_name.substr(8, str_name.length());
	Mat src = imread(str_name);
	if (src.empty())
	{
		string str_log = (boost::format("%s[ERROR]:加载图片%s失败")%__FUNCTION__ % str_name).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	str_name = str_name.substr(0, str_name.rfind('.'));
	string str_file = str_name.substr(str_name.rfind('//')+1, str_name.length());

	string save_img_jpg = str_name + ".jpg";
	imwrite(save_img_jpg,src);
	src = imread(save_img_jpg);
	ui.m_ImglistWidget->addItem(QString::fromLocal8Bit(str_file.c_str()));

}

void Burn_TestItem_UI::keyPressEvent(QKeyEvent * k)
{
	/*if (k->key() == Qt::Key_Delete)
	{
		if (i_currentIndex == -1)
		{
			string str_log = (boost::format("%s[ERROR]:删除的index=-1") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		m_ItemProp.m_mapConfigs.erase(m_vecSectionName[i_currentIndex]);
		m_vecSectionName.erase(m_vecSectionName.begin() + i_currentIndex);
		ui.m_tabWidget->removeTab(i_currentIndex);
	}*/
	QWidget::keyPressEvent(k);
}

void Burn_TestItem_UI::callback_doubleClick(QModelIndex index)
{
	string str_selected(ui.m_UserSlectlistWidget->item(index.row())->text().toLocal8Bit().data());
	if (std::find_if(m_vecSectionName.begin(), m_vecSectionName.end(),
		[str_selected](const string& str)
	{ return str == str_selected; })
		!= m_vecSectionName.end())
	{
		string str_log = (boost::format("Section段%s已经添加过了,不能重复添加") % str_selected).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}

	m_curBurnItem->burnItemSubContents.push_back(BurnItem_SubContent());
	auto& subContent = *(m_curBurnItem->burnItemSubContents.rbegin());
	//检查模板ini文件,如果存在模板,则使用模板内部的参数
	if (m_mapTemplateSections.find(str_selected) != m_mapTemplateSections.end())
	{
		subContent.subTitle = str_selected;
		boost::assign::push_back(subContent.m_burnExcelTable.m_vecHeaderLabels)("Item")("Contents")("Remark");
		subContent.m_burnExcelTable.m_vecData = m_mapTemplateSections[str_selected];
	}
	else
	{
		string str_log = (boost::format("%s[ERROR]:不存在Section段=%s的模板信息")%__FUNCTION__ % str_selected).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}

	Qt_Excel* config_excel_ui = new Qt_Excel(subContent.m_burnExcelTable);
	m_vecSectionName.push_back(str_selected);										//记录该section的名字
	ui.m_tabWidget->addTab(config_excel_ui, QString::fromLocal8Bit(str_selected.c_str()));
	ui.m_tabWidget->setCurrentIndex(ui.m_tabWidget->count() - 1);
}

void Burn_TestItem_UI::callback_addClick()
{
	string input_section(ui.m_lineEditSectionName->text().toLocal8Bit().data());
	if (std::find_if(m_vecSectionName.begin(), m_vecSectionName.end(), 
		[input_section](const string& str) 
	{ return str == input_section; }) 
		!= m_vecSectionName.end())
	{
		string str_log = (boost::format("%s[ERROR]:Section段%s已经添加过了,不能重复添加")%__FUNCTION__ % input_section).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}

	m_curBurnItem->burnItemSubContents.push_back(BurnItem_SubContent());
	auto& subContent = *(m_curBurnItem->burnItemSubContents.rbegin());
	boost::assign::push_back(subContent.m_burnExcelTable.m_vecHeaderLabels)("Item")("Contents")("Remark");
	Qt_Excel* config_excel_ui = new Qt_Excel(subContent.m_burnExcelTable);
	m_vecSectionName.push_back(input_section);										//记录该section的名字
	ui.m_tabWidget->addTab(config_excel_ui, QString::fromLocal8Bit(input_section.c_str()));
	ui.m_tabWidget->setCurrentIndex(ui.m_tabWidget->count() - 1);
}

void Burn_TestItem_UI::callback_tabBarClicked(int index)
{
	i_currentIndex = index;
}

void Burn_TestItem_UI::callback_DescTextEditTextChanged()
{
	ContentData_TextImage text_data;
	text_data.type = 0;
	string str_data = ui.m_textEditDesc->document()->toPlainText().toLocal8Bit().data();
	for (const auto& it : str_data)
		text_data.data.push_back(it);
	text_data.data.push_back('\0');

	m_curBurnItem->contentBeforeSubContent.clear();
	m_curBurnItem->contentBeforeSubContent.push_back(text_data);
}