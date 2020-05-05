#include "stdafx.h"
#include "BurnAddrsAndChecksum.h"
#include "ChecksumConfigurate_ui.h"
#include "EEPROM_INIT.h"
#include <QKeyEvent>
#include <QMenu>
#include "HexCheckExcel.h"

BurnAddrsAndChecksum::BurnAddrsAndChecksum(OTPGuideInfo& guide_info,EEPROM_INIT* p_init, string station_name,QWidget *parent)
	: CMyWidgetBase(parent), 
	m_GuideInfo(guide_info),	//总的OTP Guide的数据
	m_pInitWidget(p_init),		//初始化窗口的指针
	m_BurnExcel(guide_info.m_mapStationInfo[station_name].m_BurnAddrsExcel),	//烧录地址的excel
	m_strStationName(station_name)
{
	ui.setupUi(this);
	//ui.m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	if (m_BurnExcel.m_vecHeaderLabels.empty())
		boost::assign::push_back(m_BurnExcel.m_vecHeaderLabels)("烧录开始地址")("烧录结束地址");

	ui.m_BurnAddrScrollArea->setWidget(new CHexCheckExcel(m_BurnExcel));

	/*m_pMenuCheckSumConfig = new QMenu(ui.m_ChecksumConfigtabWidget);
	QAction* p_DeleteCheckSumConfig = new QAction(QString::fromLocal8Bit("删除当前checksum数据来源"), this);
	m_pMenuCheckSumConfig->addAction(p_DeleteCheckSumConfig);
	connect(p_DeleteCheckSumConfig, &QAction::triggered, this, &BurnAddrsAndChecksum::callback_DeleteCurrentCheckSumConfig);*/


	m_pMenuBurnItem = new QMenu(ui.m_BurnModellistWidget);
	QAction* p_DeleteBurnItem = new QAction(QString::fromLocal8Bit("删除此烧录项目"), this);
	m_pMenuBurnItem->addAction(p_DeleteBurnItem);
	connect(p_DeleteBurnItem, &QAction::triggered, this, &BurnAddrsAndChecksum::callback_DeleteBurnItem);

	//要烧录的模块显示
	if (m_GuideInfo.m_mapStationInfo.find(station_name) != m_GuideInfo.m_mapStationInfo.end())
		for (const auto& item_name : m_GuideInfo.m_mapStationInfo[station_name].m_VecNeedBurnName)
		{
			ui.m_BurnModellistWidget->addItem(QString::fromLocal8Bit(item_name.c_str()));
			auto iter = std::find_if(m_GuideInfo.m_vecBurnItems.begin(), m_GuideInfo.m_vecBurnItems.end(), [item_name](std::shared_ptr<BurnItem> p_item)
			{
				return p_item->title == item_name;
			});
			if (iter != m_GuideInfo.m_vecBurnItems.end())
			{
				if (!(*iter)->m_CheckSumAddrExcel.m_vecData.empty())
				{
					ChecksumConfigurate_ui* p_checksumConfig = new ChecksumConfigurate_ui(**iter);
					ui.m_ChecksumConfigtabWidget->addTab(p_checksumConfig, QString::fromLocal8Bit(item_name.c_str()));
					////m_GuideInfo.m_mapVecNeedChecksumName[m_strStationName].push_back(algo_name);
				}
			}
		}

	////checksum的配置模块显示
	//for (const auto& checksum_item_name : guide_info.m_mapStationInfo[station_name].m_VecNeedChecksumName)
	//{
	//	string algo_name = checksum_item_name;
	//	auto iter = std::find_if(m_GuideInfo.m_vecBurnItems.begin(), m_GuideInfo.m_vecBurnItems.end(), [algo_name](std::shared_ptr<BurnItem> p_item)
	//	{
	//		return p_item->title == algo_name;
	//	});
	//	if (iter != m_GuideInfo.m_vecBurnItems.end())
	//	{
	//		ChecksumConfigurate_ui* p_checksumConfig = new ChecksumConfigurate_ui(**iter);
	//		//m_GuideInfo.m_mapVecNeedChecksumName[m_strStationName].push_back(algo_name);
	//		ui.m_ChecksumConfigtabWidget->addTab(p_checksumConfig, QString::fromLocal8Bit(algo_name.c_str()));
	//	}
	//}

	//ShowExcel();
}

BurnAddrsAndChecksum::~BurnAddrsAndChecksum()
{

}

void BurnAddrsAndChecksum::AddCheckSumConfigInfo(BurnItem& item)
{
	ChecksumConfigurate_ui* p_checksumConfig = new ChecksumConfigurate_ui(item);
	string algo_name = item.title;
	//m_GuideInfo.m_mapStationInfo[m_strStationName].m_VecNeedChecksumName.push_back(algo_name);
	ui.m_ChecksumConfigtabWidget->addTab(p_checksumConfig , QString::fromLocal8Bit(algo_name.c_str()));
}

void BurnAddrsAndChecksum::AddCheckSumTabByAlgoName(const string& str)
{
	auto iter = std::find_if(m_GuideInfo.m_vecBurnItems.begin(), m_GuideInfo.m_vecBurnItems.end(), [str](std::shared_ptr<BurnItem> p_item)
	{
		return str == p_item->title;
	}
	);
	ChecksumConfigurate_ui* p_checksumConfig = new ChecksumConfigurate_ui(**iter);
	ui.m_ChecksumConfigtabWidget->addTab(p_checksumConfig, QString::fromLocal8Bit((*iter)->title.c_str()));

}

void BurnAddrsAndChecksum::RemoveCheckSumTabByAlgoName(const string& str)
{
	auto iter = std::find_if(m_GuideInfo.m_vecBurnItems.begin(), m_GuideInfo.m_vecBurnItems.end(), [str](std::shared_ptr<BurnItem> p_item)
	{
		return str == p_item->title;
	}
	);
	//清除该测项的checksum数据来源数据
	if(iter != m_GuideInfo.m_vecBurnItems.end())
		(*iter)->m_CheckSumDataSourceRnage.clear();
	//删除该checksum的tab(如果存在的话)
	for (int i = 0; i < ui.m_ChecksumConfigtabWidget->count(); i++)
	{
		string str_tab_name = ui.m_ChecksumConfigtabWidget->tabText(i).toLocal8Bit().data();
		if (str_tab_name == str)
		{
			ui.m_ChecksumConfigtabWidget->removeTab(i);
			break;
		}
	}
}

void BurnAddrsAndChecksum::UpdataWidget()
{
	//ShowExcel();
	ui.m_BurnModellistWidget->clear();
	for (const auto& names : m_GuideInfo.m_mapStationInfo[m_strStationName].m_VecNeedBurnName)
		ui.m_BurnModellistWidget->addItem(QString::fromLocal8Bit(names.c_str()));

	for (int i = 0; i < ui.m_ChecksumConfigtabWidget->count(); i++)
		((CMyWidgetBase*)ui.m_ChecksumConfigtabWidget->widget(i))->UpdataWidget();
}

//void BurnAddrsAndChecksum::ShowExcel()
//{
//	disconnect(ui.m_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_BurnAddrWidgetItemChanged(QTableWidgetItem*)));
//	ui.m_tableWidget->clearSpans();		//清空所有的合并
//	ui.m_tableWidget->clearContents();
//	ui.m_tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//表头字体居中
//	ui.m_tableWidget->setRowCount(m_BurnExcel.m_vecData.size());
//	ui.m_tableWidget->setColumnCount(2);
//
//	/*QStringList list_headerLabels;
//	for (const auto& label : m_vecBurnRuleHeaderLabels)
//		list_headerLabels.push_back(QString::fromLocal8Bit(label.c_str()));
//	ui.m_BurnRuleTable->setHorizontalHeaderLabels(list_headerLabels);*/
//
//	for (int row = 0; row < m_BurnExcel.m_vecData.size(); row++)
//		for (int col = 0; col < 2; col++)
//		{
//			ui.m_tableWidget->setItem(row, col, new QTableWidgetItem(QString::fromLocal8Bit(m_BurnExcel.m_vecData[row][col].c_str())));
//			if (!IsHex(m_BurnExcel.m_vecData[row][col]))
//				ui.m_tableWidget->item(row, col)->setBackgroundColor(Qt::red);
//			else
//				ui.m_tableWidget->item(row, col)->setBackgroundColor(Qt::green);
//		}
//	connect(ui.m_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_BurnAddrWidgetItemChanged(QTableWidgetItem*)));
//}

//void BurnAddrsAndChecksum::keyPressEvent(QKeyEvent * k)
//{
//	if ((k->key() == Qt::Key_Insert))
//	{
//		if (ui.m_tableWidget->hasFocus())		//如果烧录地址窗口有焦点,则往烧录规则表格中插入
//		{
//			InsertOneRow();
//			ShowExcel();
//		}
//	}
//	if (k->key() == Qt::Key_Delete)
//	{
//		if (m_iLastSelectRow == -1)
//		{
//			string str_log = (boost::format("%s[ERROR]:未选中某行,不能删除") % __FUNCTION__).str();
//			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
//			return;
//		}
//		if (m_iLastSelectRow == 0 && m_BurnExcel.m_vecData.empty())
//		{
//			string str_log = (boost::format("%s[ERROR]:当前已经没有数据可以删除了") % __FUNCTION__).str();
//			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
//			return;
//		}
//		m_BurnExcel.m_vecData.erase(m_BurnExcel.m_vecData.begin() + m_iLastSelectRow);
//		ShowExcel();
//		return;
//	}
//}

//bool BurnAddrsAndChecksum::InsertOneRow()
//{
//	auto selectItems = ui.m_tableWidget->selectedItems();
//	vector<string> vec_tmp;
//	boost::assign::push_back(vec_tmp)("0x")("0x");
//	if (selectItems.size() == 0)
//		m_BurnExcel.m_vecData.push_back(vec_tmp);
//	else if (selectItems.size() == 1)
//		m_BurnExcel.m_vecData.insert(m_BurnExcel.m_vecData.begin() + selectItems[0]->row(), vec_tmp);
//	return true;
//}

//void BurnAddrsAndChecksum::callback_BurnAddrWidgetItemChanged(QTableWidgetItem* item)
//{
//	int row = item->row();
//	int col = item->column();
//	string m_str = item->text().toLocal8Bit().data();
//	m_BurnExcel.m_vecData[row][col] = m_str;
//	ShowExcel();
//}

//void BurnAddrsAndChecksum::callback_ChecksumConfigcustomContextMenuRequested(QPoint pt)
//{
//	m_pMenuCheckSumConfig->popup(ui.m_ChecksumConfigtabWidget->mapToGlobal(pt));
//}

//void BurnAddrsAndChecksum::callback_DeleteCurrentCheckSumConfig()
//{
//	int cur_select_index = ui.m_ChecksumConfigtabWidget->currentIndex();
//	if (cur_select_index == -1)
//	{
//		string str_log = (boost::format("%s[ERROR]:当前没有选择的tab,无法删除") % __FUNCTION__).str();
//		QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
//		return;
//	}
//	string algo_name = ui.m_ChecksumConfigtabWidget->tabText(cur_select_index).toLocal8Bit().data();	//获取当前选中的tab的名字
//
//	auto iter = std::find_if(m_GuideInfo.m_mapStationInfo[m_strStationName].m_VecNeedChecksumName.begin(), m_GuideInfo.m_mapStationInfo[m_strStationName].m_VecNeedChecksumName.end(), 
//		[algo_name](string& str)	// 找到该tab名字对应的占用chekcsum容器中的迭代器位置
//	{
//		return algo_name == str;
//	}
//	);
//	m_GuideInfo.m_mapStationInfo[m_strStationName].m_VecNeedChecksumName.erase(iter);	//删除该迭代器
//
//
//	////删除当前测试站别下需要用到的checksum配置界面
//	//auto iter2 = std::find_if(m_GuideInfo.m_mapStationInfo[m_strStationName].m_VecNeedChecksumName.begin(), m_GuideInfo.m_mapStationInfo[m_strStationName].m_VecNeedChecksumName.end(), [algo_name](string& str)	// 找到该tab名字对应的占用chekcsum容器中的迭代器位置
//	//{
//	//	return algo_name == str;
//	//}
//	//);
//	//m_GuideInfo.m_mapStationInfo[m_strStationName].m_VecNeedChecksumName.erase(iter2);
//
//	ui.m_ChecksumConfigtabWidget->removeTab(cur_select_index);
//	m_pInitWidget->UpdataWidget();
//}

void BurnAddrsAndChecksum::callback_BurnItemCustomContextMenuRequested(QPoint pt)
{
	m_pMenuBurnItem->popup(ui.m_BurnModellistWidget->mapToGlobal(pt));
}

void BurnAddrsAndChecksum::callback_DeleteBurnItem()
{
	if (ui.m_BurnModellistWidget->currentRow() == -1)
	{
		string str_log = (boost::format("%s[ERROR]:没有选择正确测试项目,不能删除") % __FUNCTION__).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	string str_current_test_item = ui.m_BurnModellistWidget->currentItem()->text().toLocal8Bit().data();
	auto& vec_items = m_GuideInfo.m_mapStationInfo[m_strStationName].m_VecNeedBurnName;
	auto iter = std::find_if(vec_items.begin(), vec_items.end(), [str_current_test_item](string str)
	{
		return str == str_current_test_item;
	});
	if(vec_items.end() != iter)
		vec_items.erase(iter);
	//清除有关这个算法模块的checksum
	RemoveCheckSumTabByAlgoName(str_current_test_item);
	/*for (int i = 0; i < ui.m_ChecksumConfigtabWidget->count(); i++)
	{
		string str_title = ui.m_ChecksumConfigtabWidget->tabText(i).toLocal8Bit().data();
		if (str_title == str_current_test_item)
		{
			ui.m_ChecksumConfigtabWidget->removeTab(i);
			break;
		}
	}*/
//	m_GuideInfo.m_vecSpaceUsageBurnModel.push_back(str_current_test_item);
	m_pInitWidget->UpdataWidget();
}

void BurnAddrsAndChecksum::callback_ItemClicked(QTableWidgetItem* item)
{
	m_iLastSelectRow = item->row();
	m_iLastSelectCol = item->column();
}

void BurnAddrsAndChecksum::callback_CheckSumTableCurrentChanged(int index)
{
	for (int i = 0; i < ui.m_ChecksumConfigtabWidget->count(); i++)
	{
		((CMyWidgetBase*)ui.m_ChecksumConfigtabWidget->widget(i))->ShowExcel();
	}
}