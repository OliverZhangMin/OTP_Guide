#include "stdafx.h"
#include "EEPROM_Addr_Map.h"

EEPROM_Addr_Map::EEPROM_Addr_Map(OTPGuideInfo& guide_info,QWidget *parent)
	: CMyWidgetBase(parent),m_GuideInfo(guide_info),m_mapSpan(guide_info.m_EEPROMAddrExcel.m_mapColnCombInfo)
{
	ui.setupUi(this);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

EEPROM_Addr_Map::~EEPROM_Addr_Map()
{
		
}
void EEPROM_Addr_Map::ShowExcel()
{
	disconnect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_AddrsItemChanged(QTableWidgetItem*)));
	auto& addr_map = m_GuideInfo.m_EEPROMAddrExcel.m_mapAddrProp;
	ui.tableWidget->clearSpans();		//清空所有的合并
	ui.tableWidget->clearContents();
	ui.tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//表头字体居中
	int row_count = addr_map.size();// +m_GuideInfo.m_EEPROMAddrExcel.m_vecSectionAddrInfo.size();	//计算这个表格有多少行要显示
	ui.tableWidget->setRowCount(row_count);
	int column_count = addr_map.begin()->second.size() + 1;
	ui.tableWidget->setColumnCount(column_count);

	//QStringList list_headerLabels;
	//for (const auto& label : m_vecBurnRuleHeaderLabels)
	//	list_headerLabels.push_back(QString::fromLocal8Bit(label.c_str()));
	//ui.m_BurnRuleTable->setHorizontalHeaderLabels(list_headerLabels);
	//先插入所有的数据
	int cur_row = 0;
	char buff[64];
	for (auto iter = addr_map.begin(); iter != addr_map.end(); iter++)
	{
		sprintf(buff, "0x%x", iter->first);
		ui.tableWidget->setItem(cur_row, 0, new QTableWidgetItem(QString::fromLocal8Bit(buff)));
		ui.tableWidget->item(cur_row, 0)->setFlags(Qt::ItemIsSelectable| Qt::ItemIsEnabled);	//地址栏位不可被编辑
		for (int i = 1; i < 7; i++)
		{
			ui.tableWidget->setItem(cur_row, i, new QTableWidgetItem(QString::fromLocal8Bit(iter->second[i-1].c_str())));
			//将4,5,6列变成不可被编辑
			if (i >= 4 && i <= 6)
				ui.tableWidget->item(cur_row, i)->setFlags(0);
		}
		cur_row++;
	}

	//在测项名的地址上增加其名字
	for (const auto& SubItem : m_GuideInfo.m_EEPROMAddrExcel.m_vecSectionAddrInfo)
	{
		int find_row = FindRowByAddr(SubItem.m_iAddr_start);
		ui.tableWidget->insertRow(find_row);
		ui.tableWidget->setItem(find_row, 0, new QTableWidgetItem(QString::fromLocal8Bit(SubItem.m_strName.c_str())));
		ui.tableWidget->setSpan(find_row, 0, 1, 7);			//合并显示测项名的该行
		ui.tableWidget->item(find_row, 0)->setBackground(Qt::green);
		ui.tableWidget->item(find_row, 0)->setFlags(Qt::NoItemFlags);
		ui.tableWidget->item(find_row, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}
	
	//找到checksum的row,并标记黄色
	auto vec_checksum =  m_GuideInfo.m_EEPROMAddrExcel.m_vecCheckSumAddrs;
	for (const auto& checksum_addr : vec_checksum)
	{
		int find_row = FindRowByAddr(checksum_addr);
		for (int i = 0; i < column_count; i++)
			ui.tableWidget->item(find_row, i)->setBackground(Qt::yellow);
	}

	//删除需要合并的行
	int find_row;
	for (const auto& it : m_mapSpan[0])	//检查是否有要合并的地址
	{
		find_row = FindRowByAddr(it.first);
		ui.tableWidget->insertRow(find_row);
		string str_addr = (boost::format("0x%x~0x%x") % it.first % it.second).str();
		ui.tableWidget->setItem(find_row, 0, new QTableWidgetItem(QString::fromLocal8Bit(str_addr.c_str())));
		ui.tableWidget->item(find_row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		//2~4列写...,并且不可被编辑
		for (int i = 1; i < 7; i++)
		{
			ui.tableWidget->setItem(find_row, i, new QTableWidgetItem(QString::fromLocal8Bit("...")));
			ui.tableWidget->item(find_row, i)->setFlags(Qt::NoItemFlags);
		}
		for(int i=0;i<7;i++)
			ui.tableWidget->item(find_row, i)->setBackground(QColor(255, 97, 0));	//设置成橙色
		for (int i = it.first; i <= it.second; i++)
		{
			find_row = FindRowByAddr(i);
			if(find_row == -1)
				continue;
			else
				ui.tableWidget->removeRow(find_row);
		}
	}

	//根据地址合并
	for (const auto& spans : m_mapSpan)
	{
		if (spans.first == 0)		//第0列为地址段,此处不对其进行操作
			continue;
		int col = spans.first;			//第几列
		for (const auto& addr_range : spans.second)		//地址区间
		{
			int row_start = FindRowByAddr(addr_range.first);
			int row_end = FindRowByAddr(addr_range.second);
			int span_size = row_end - row_start + 1;
			ui.tableWidget->setSpan(row_start, col, span_size, 1);
		}
	}
	connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_AddrsItemChanged(QTableWidgetItem*)));
}

void EEPROM_Addr_Map::UpdataWidget()
{
	int addr = 0;
	char buff[64];
	//获取所有BurnItem,并保存到map
	auto& addr_map = m_GuideInfo.m_EEPROMAddrExcel.m_mapAddrProp;
	addr_map.clear();
	m_GuideInfo.m_EEPROMAddrExcel.m_vecSectionAddrInfo.clear();	//清空这个区域的地址段数据
	m_GuideInfo.m_EEPROMAddrExcel.m_vecCheckSumAddrs.clear();	//删除先前checksum的地址

	for (auto& item : m_GuideInfo.m_vecBurnItems)
	{
		MapSectionAddr map_section;
		map_section.m_strName = item->title;		//加入名字
		//找出这个算法模块的最小 和 最大值
		vector<int> vec_addrs;
		for (auto& burn : item->m_BurnRlueExcel.m_vecData)
		{
			sscanf(burn[0].c_str(), "0x%x", &addr);	//4-.>1
			vec_addrs.push_back(addr);
			int len = boost::lexical_cast<int>(burn[1]);
			if(std::find(vec_addrs.begin(),vec_addrs.end(), addr + len - 1) == vec_addrs.end())	//如果没有找到过该地址就新增
				vec_addrs.push_back(addr + len - 1);

			if (addr_map[addr].empty())
			{
				vector<string> vec_str;
				/*boost::assign::push_back(vec_str)(burn[0])("")("")("")(burn[1])(burn[2])(burn[3]);*/
				boost::assign::push_back(vec_str)("")("")("")(burn[1])(burn[2])(burn[3]);
				addr_map[addr] = vec_str;			//插入一行测试数据
				int loop_count = boost::lexical_cast<int>(burn[1]);
				for (int i = 1; i < loop_count; i++)
				{
					int tmp_addr = ++addr;
					vec_str.clear();
					/*sprintf(buff, "0x%x", tmp_addr);
					boost::assign::push_back(vec_str)(buff)("")("")("")("")("")("");*/
					boost::assign::push_back(vec_str)("")("")("")("")("")(""); 
					addr_map[addr] = vec_str;
				}
			}
			else
			{/*
				addr_map[addr][4] = burn[1];
				addr_map[addr][5] = burn[2];
				addr_map[addr][6] = burn[3];*/

				addr_map[addr][3] = burn[1];
				addr_map[addr][4] = burn[2];
				addr_map[addr][5] = burn[3];
			}
		}

		//加入checksum的地址
		if (!item->m_CheckSumAddrExcel.m_vecData.empty())
		{
			//将chekcsum的地址加入到映射表中
			sscanf(item->m_CheckSumAddrExcel.m_vecData[0][0].c_str(), "0x%x", &addr);
			int len = boost::lexical_cast<int>(item->m_CheckSumAddrExcel.m_vecData[0][1]);
			//计算chekcsum用到的地址
			for (int i = addr; i < addr + len; i++)
			{
				m_GuideInfo.m_EEPROMAddrExcel.m_vecCheckSumAddrs.push_back(i);			//添加checksum地址
				vec_addrs.push_back(i);
			}
			//根据用到的chekcsum地址,填充map
			if (addr_map[addr].empty())
			{
				vector<string> vec_str;
				/*boost::assign::push_back(vec_str)(item->m_vecCheckSumAddr[0][0])("")("")("")("")("")(item->m_vecCheckSumAddr[0][3]);*/
				boost::assign::push_back(vec_str)("")("")("")("")("")(item->m_CheckSumAddrExcel.m_vecData[0][3]);
				addr_map[addr] = vec_str;			//插入一行测试数据
				int loop_count = boost::lexical_cast<int>(item->m_CheckSumAddrExcel.m_vecData[0][1]);
				for (int i = 1; i < loop_count; i++)
				{
					int tmp_addr = ++addr;
					vec_str.clear();
					/*sprintf(buff, "0x%x", tmp_addr);
					boost::assign::push_back(vec_str)(buff)("")("")("")("")("")("");*/
					boost::assign::push_back(vec_str)("")("")("")("")("")("");
					addr_map[addr] = vec_str;
				}
			}
		}

		//计算该区域用到的地址
		auto iter_min = std::min_element(vec_addrs.begin(), vec_addrs.end());
		auto iter_max = std::max_element(vec_addrs.begin(), vec_addrs.end());
		if (iter_min != vec_addrs.end() && iter_max != vec_addrs.end())
		{
			map_section.m_iAddr_start = *iter_min;		//区域的最小值
			map_section.m_iAddr_end = *iter_max;		//区域的最大值
			m_GuideInfo.m_EEPROMAddrExcel.m_vecSectionAddrInfo.push_back(map_section);	//追加这个区域的地址段数据
		}
	}
	if(addr_map.size())
		ShowExcel();
}

void EEPROM_Addr_Map::keyPressEvent(QKeyEvent * k)
{
	if ((k->key()&Qt::Key_M) == Qt::Key_M && (k->modifiers()&Qt::CTRL) == Qt::CTRL)
	{
		QList<QTableWidgetItem*>& m_SelectItems = ui.tableWidget->selectedItems();
		int count = m_SelectItems.count();
		map<int, int> map_columns;
		map<int, int> map_rows;
		for (int i = 0; i < count; i++)
		{
			map_rows[m_SelectItems.at(i)->row()];					//获取选中的行
			map_columns[m_SelectItems.at(i)->column()];
		}
		//监测行是否满足
		if (map_rows.size() <= 1)
		{
			string str_log = (boost::format("当前选中了%d行,%d列,但是合并EEPROM的单元格至少需要选中2行") % map_rows.size() % map_columns.size()).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		//监测列是否满足
		if (map_columns.size() != 1)
		{
			string str_log = (boost::format("当前选中了%d行,%d列,但是合并EEPROM的单元格只能在1列上进行") % map_rows.size() % map_columns.size()).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		//开始合并单元格
		//1.检查行数是否连续
		if ((map_rows.crbegin()->first - map_rows.begin()->first + 1) != map_rows.size())
		{
			string str_log = (boost::format("当前选中了%d行,%d列,但是合并EEPROM的单元格的行必须是连续的") % map_rows.size() % map_columns.size()).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		int span_column = map_columns.begin()->first;	//合并的起始列
		if (span_column == 0)	//地址栏位的合并
		{
			//如果小于5行就不要合并了
			if (map_rows.size() < 5)
			{
				string str_log = (boost::format("合并地址栏位,选中的个数至少需要5个")).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return;
			}
			//判断合并的地址是否都连续
			bool b_first = true;
			int i_previous = 0;
			int i_current = 0;
			for (const auto& addr : map_rows)
			{
				if (b_first)
				{
					sscanf(ui.tableWidget->item(addr.first, 0)->text().toLocal8Bit().data(), "0x%x", &i_previous);
					b_first = false;
					continue;
				}
				sscanf(ui.tableWidget->item(addr.first, 0)->text().toLocal8Bit().data(), "0x%x", &i_current);
				if (i_current - i_previous == 1)
				{
					sscanf(ui.tableWidget->item(addr.first, 0)->text().toLocal8Bit().data(), "0x%x", &i_previous);
					continue;
				}
				else
				{
					string str_log = (boost::format("合并地址栏位,在第%d行处不连续,不能合并") % addr.first).str();
					QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
					return;
				}
			}
			//记录合并地址的起始地址
			int i_start;
			sscanf(ui.tableWidget->item(map_rows.begin()->first, 0)->text().toLocal8Bit().data(), "0x%x", &i_start);
			m_mapSpan[span_column].push_back(make_pair(i_start, i_start + map_rows.size() - 1));
			ShowExcel();
			int select_row = FindRowByAddr(i_start);
			if (select_row != -1)
				ui.tableWidget->selectRow(select_row);
		}
		else if (span_column < 4)
		{
			//如果就1行,那就不要合并了
			int row_start = map_rows.begin()->first;		//合并的起始行

			int span_rows = map_rows.size();				//合并的行数
															//ui.m_tableWidget->setSpan(row_start, span_column, span_rows, 1);
			ui.tableWidget->item(row_start, span_column)->setSelected(true);	//选中合并的item
			int i_start_addr = 0, i_end_addr = 0;
			sscanf(ui.tableWidget->item(row_start, 0)->text().toLocal8Bit().data(), "0x%x", &i_start_addr);
			sscanf(ui.tableWidget->item(row_start + span_rows - 1, 0)->text().toLocal8Bit().data(), "0x%x", &i_end_addr);
			m_mapSpan[span_column].push_back(make_pair(i_start_addr, i_end_addr));		//计算合并的参数
			ShowExcel();
		}
		else
		{
			string str_log = (boost::format("%s[ERROR]:从第4列开始无法被合并")%__FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
			return;
		}
	}
	else if ((k->key()&Qt::Key_N) == Qt::Key_N && (k->modifiers()&Qt::CTRL) == Qt::CTRL)
	{
		auto m_SelectItems = ui.tableWidget->selectedItems();
		//解除单元格合并
		int count = m_SelectItems.count();
		map<int, int> map_columns;
		map<int, int> map_rows;
		for (int i = 0; i < count; i++)
		{
			map_rows[m_SelectItems.at(i)->row()];					//获取选中的行
			map_columns[m_SelectItems.at(i)->column()];
		}
		if (count != 1)
		{
			string str_log = (boost::format("请单独选择1个单元格解除合并")).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		int col = map_columns.begin()->first;
		if (col == 0)
		{
			string str_text(ui.tableWidget->item(m_SelectItems.at(0)->row(), 0)->text().toLocal8Bit().data());	//获取这一栏位的地址区间
			if (str_text.find('~') == std::string::npos)
			{
				string str_log = (boost::format("当前选中的地址段%s,无法解除地址合并") % str_text).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return;
			}
			//找出起始地址
			vector<string> vec_str;
			boost::split(vec_str, str_text, boost::is_any_of("~"), boost::token_compress_on);
			int i_start = 0;
			sscanf(vec_str[0].c_str(), "0x%x", &i_start);
			m_mapSpan[col].erase(std::find_if(m_mapSpan[col].begin(), m_mapSpan[col].end(), [i_start](pair<int, int> p) { return p.first == i_start; }));
			ShowExcel();

			ui.tableWidget->selectRow(FindRowByAddr(i_start));
		}
		else if (col > 0)
		{
			string select_addr(ui.tableWidget->item(map_rows.begin()->first, 0)->text().toLocal8Bit().data());
			int addr = 0;
			sscanf(select_addr.c_str(), "0x%x", &addr);
			if (m_mapSpan.find(col) == m_mapSpan.end())
			{
				string str_log = (boost::format("第%d列,没有合并过单元格,解除合并失败") % col).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return;
			}

			//查找该列符合该地址区间的pair,如果有就删除它
			auto vec_iter = std::find_if(m_mapSpan[col].begin(), m_mapSpan[col].end(), [addr](pair<int, int> p) {
				return p.first == addr;
			});
			if (vec_iter != m_mapSpan[col].end())
			{
				m_mapSpan[col].erase(vec_iter);
				ShowExcel();
			}
		}
	}
	QWidget::keyPressEvent(k);
}

int EEPROM_Addr_Map::FindRowByAddr(const int& addr)
{
	char buff[64];
	sprintf(buff, "0x%x", addr);
	string str_buff(buff);
	for (int i = 0; i < ui.tableWidget->rowCount(); i++)
	{
		string str_item(ui.tableWidget->item(i, 0)->text().toLocal8Bit().data());
		/*if (boost::contains(str_item, str_buff))
			return i;*/
		if (str_item == str_buff)
			return i;
	}
	return -1;
}

void EEPROM_Addr_Map::callback_AddrsItemChanged(QTableWidgetItem* item)
{
	//auto& addrs_map = m_GuideInfo.m_EEPROMAddrExcel.m_mapAddrProp;
	int row = item->row();
	int col = item->column();
	string str_data = item->text().toLocal8Bit().data();
	string str_addr = ui.tableWidget->item(row, 0)->text().toLocal8Bit().data();
	int addr = -1;
	sscanf(str_addr.c_str(), "0x%x", &addr);
	m_GuideInfo.m_EEPROMAddrExcel.m_mapAddrProp[addr][col - 1] = str_data;
}