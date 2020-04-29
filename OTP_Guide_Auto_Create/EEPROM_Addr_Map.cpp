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
	ui.tableWidget->clearSpans();		//������еĺϲ�
	ui.tableWidget->clearContents();
	ui.tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//��ͷ�������
	int row_count = addr_map.size();// +m_GuideInfo.m_EEPROMAddrExcel.m_vecSectionAddrInfo.size();	//�����������ж�����Ҫ��ʾ
	ui.tableWidget->setRowCount(row_count);
	int column_count = addr_map.begin()->second.size() + 1;
	ui.tableWidget->setColumnCount(column_count);

	//QStringList list_headerLabels;
	//for (const auto& label : m_vecBurnRuleHeaderLabels)
	//	list_headerLabels.push_back(QString::fromLocal8Bit(label.c_str()));
	//ui.m_BurnRuleTable->setHorizontalHeaderLabels(list_headerLabels);
	//�Ȳ������е�����
	int cur_row = 0;
	char buff[64];
	for (auto iter = addr_map.begin(); iter != addr_map.end(); iter++)
	{
		sprintf(buff, "0x%x", iter->first);
		ui.tableWidget->setItem(cur_row, 0, new QTableWidgetItem(QString::fromLocal8Bit(buff)));
		ui.tableWidget->item(cur_row, 0)->setFlags(Qt::ItemIsSelectable| Qt::ItemIsEnabled);	//��ַ��λ���ɱ��༭
		for (int i = 1; i < 7; i++)
		{
			ui.tableWidget->setItem(cur_row, i, new QTableWidgetItem(QString::fromLocal8Bit(iter->second[i-1].c_str())));
			//��4,5,6�б�ɲ��ɱ��༭
			if (i >= 4 && i <= 6)
				ui.tableWidget->item(cur_row, i)->setFlags(0);
		}
		cur_row++;
	}

	//�ڲ������ĵ�ַ������������
	for (const auto& SubItem : m_GuideInfo.m_EEPROMAddrExcel.m_vecSectionAddrInfo)
	{
		int find_row = FindRowByAddr(SubItem.m_iAddr_start);
		ui.tableWidget->insertRow(find_row);
		ui.tableWidget->setItem(find_row, 0, new QTableWidgetItem(QString::fromLocal8Bit(SubItem.m_strName.c_str())));
		ui.tableWidget->setSpan(find_row, 0, 1, 7);			//�ϲ���ʾ�������ĸ���
		ui.tableWidget->item(find_row, 0)->setBackground(Qt::green);
		ui.tableWidget->item(find_row, 0)->setFlags(Qt::NoItemFlags);
		ui.tableWidget->item(find_row, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}
	
	//�ҵ�checksum��row,����ǻ�ɫ
	auto vec_checksum =  m_GuideInfo.m_EEPROMAddrExcel.m_vecCheckSumAddrs;
	for (const auto& checksum_addr : vec_checksum)
	{
		int find_row = FindRowByAddr(checksum_addr);
		for (int i = 0; i < column_count; i++)
			ui.tableWidget->item(find_row, i)->setBackground(Qt::yellow);
	}

	//ɾ����Ҫ�ϲ�����
	int find_row;
	for (const auto& it : m_mapSpan[0])	//����Ƿ���Ҫ�ϲ��ĵ�ַ
	{
		find_row = FindRowByAddr(it.first);
		ui.tableWidget->insertRow(find_row);
		string str_addr = (boost::format("0x%x~0x%x") % it.first % it.second).str();
		ui.tableWidget->setItem(find_row, 0, new QTableWidgetItem(QString::fromLocal8Bit(str_addr.c_str())));
		ui.tableWidget->item(find_row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		//2~4��д...,���Ҳ��ɱ��༭
		for (int i = 1; i < 7; i++)
		{
			ui.tableWidget->setItem(find_row, i, new QTableWidgetItem(QString::fromLocal8Bit("...")));
			ui.tableWidget->item(find_row, i)->setFlags(Qt::NoItemFlags);
		}
		for(int i=0;i<7;i++)
			ui.tableWidget->item(find_row, i)->setBackground(QColor(255, 97, 0));	//���óɳ�ɫ
		for (int i = it.first; i <= it.second; i++)
		{
			find_row = FindRowByAddr(i);
			if(find_row == -1)
				continue;
			else
				ui.tableWidget->removeRow(find_row);
		}
	}

	//���ݵ�ַ�ϲ�
	for (const auto& spans : m_mapSpan)
	{
		if (spans.first == 0)		//��0��Ϊ��ַ��,�˴���������в���
			continue;
		int col = spans.first;			//�ڼ���
		for (const auto& addr_range : spans.second)		//��ַ����
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
	//��ȡ����BurnItem,�����浽map
	auto& addr_map = m_GuideInfo.m_EEPROMAddrExcel.m_mapAddrProp;
	addr_map.clear();
	m_GuideInfo.m_EEPROMAddrExcel.m_vecSectionAddrInfo.clear();	//����������ĵ�ַ������
	m_GuideInfo.m_EEPROMAddrExcel.m_vecCheckSumAddrs.clear();	//ɾ����ǰchecksum�ĵ�ַ

	for (auto& item : m_GuideInfo.m_vecBurnItems)
	{
		MapSectionAddr map_section;
		map_section.m_strName = item->title;		//��������
		//�ҳ�����㷨ģ�����С �� ���ֵ
		vector<int> vec_addrs;
		for (auto& burn : item->m_BurnRlueExcel.m_vecData)
		{
			sscanf(burn[0].c_str(), "0x%x", &addr);	//4-.>1
			vec_addrs.push_back(addr);
			int len = boost::lexical_cast<int>(burn[1]);
			if(std::find(vec_addrs.begin(),vec_addrs.end(), addr + len - 1) == vec_addrs.end())	//���û���ҵ����õ�ַ������
				vec_addrs.push_back(addr + len - 1);

			if (addr_map[addr].empty())
			{
				vector<string> vec_str;
				/*boost::assign::push_back(vec_str)(burn[0])("")("")("")(burn[1])(burn[2])(burn[3]);*/
				boost::assign::push_back(vec_str)("")("")("")(burn[1])(burn[2])(burn[3]);
				addr_map[addr] = vec_str;			//����һ�в�������
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

		//����checksum�ĵ�ַ
		if (!item->m_CheckSumAddrExcel.m_vecData.empty())
		{
			//��chekcsum�ĵ�ַ���뵽ӳ�����
			sscanf(item->m_CheckSumAddrExcel.m_vecData[0][0].c_str(), "0x%x", &addr);
			int len = boost::lexical_cast<int>(item->m_CheckSumAddrExcel.m_vecData[0][1]);
			//����chekcsum�õ��ĵ�ַ
			for (int i = addr; i < addr + len; i++)
			{
				m_GuideInfo.m_EEPROMAddrExcel.m_vecCheckSumAddrs.push_back(i);			//���checksum��ַ
				vec_addrs.push_back(i);
			}
			//�����õ���chekcsum��ַ,���map
			if (addr_map[addr].empty())
			{
				vector<string> vec_str;
				/*boost::assign::push_back(vec_str)(item->m_vecCheckSumAddr[0][0])("")("")("")("")("")(item->m_vecCheckSumAddr[0][3]);*/
				boost::assign::push_back(vec_str)("")("")("")("")("")(item->m_CheckSumAddrExcel.m_vecData[0][3]);
				addr_map[addr] = vec_str;			//����һ�в�������
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

		//����������õ��ĵ�ַ
		auto iter_min = std::min_element(vec_addrs.begin(), vec_addrs.end());
		auto iter_max = std::max_element(vec_addrs.begin(), vec_addrs.end());
		if (iter_min != vec_addrs.end() && iter_max != vec_addrs.end())
		{
			map_section.m_iAddr_start = *iter_min;		//�������Сֵ
			map_section.m_iAddr_end = *iter_max;		//��������ֵ
			m_GuideInfo.m_EEPROMAddrExcel.m_vecSectionAddrInfo.push_back(map_section);	//׷���������ĵ�ַ������
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
			map_rows[m_SelectItems.at(i)->row()];					//��ȡѡ�е���
			map_columns[m_SelectItems.at(i)->column()];
		}
		//������Ƿ�����
		if (map_rows.size() <= 1)
		{
			string str_log = (boost::format("��ǰѡ����%d��,%d��,���Ǻϲ�EEPROM�ĵ�Ԫ��������Ҫѡ��2��") % map_rows.size() % map_columns.size()).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		//������Ƿ�����
		if (map_columns.size() != 1)
		{
			string str_log = (boost::format("��ǰѡ����%d��,%d��,���Ǻϲ�EEPROM�ĵ�Ԫ��ֻ����1���Ͻ���") % map_rows.size() % map_columns.size()).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		//��ʼ�ϲ���Ԫ��
		//1.��������Ƿ�����
		if ((map_rows.crbegin()->first - map_rows.begin()->first + 1) != map_rows.size())
		{
			string str_log = (boost::format("��ǰѡ����%d��,%d��,���Ǻϲ�EEPROM�ĵ�Ԫ����б�����������") % map_rows.size() % map_columns.size()).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		int span_column = map_columns.begin()->first;	//�ϲ�����ʼ��
		if (span_column == 0)	//��ַ��λ�ĺϲ�
		{
			//���С��5�оͲ�Ҫ�ϲ���
			if (map_rows.size() < 5)
			{
				string str_log = (boost::format("�ϲ���ַ��λ,ѡ�еĸ���������Ҫ5��")).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return;
			}
			//�жϺϲ��ĵ�ַ�Ƿ�����
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
					string str_log = (boost::format("�ϲ���ַ��λ,�ڵ�%d�д�������,���ܺϲ�") % addr.first).str();
					QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
					return;
				}
			}
			//��¼�ϲ���ַ����ʼ��ַ
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
			//�����1��,�ǾͲ�Ҫ�ϲ���
			int row_start = map_rows.begin()->first;		//�ϲ�����ʼ��

			int span_rows = map_rows.size();				//�ϲ�������
															//ui.m_tableWidget->setSpan(row_start, span_column, span_rows, 1);
			ui.tableWidget->item(row_start, span_column)->setSelected(true);	//ѡ�кϲ���item
			int i_start_addr = 0, i_end_addr = 0;
			sscanf(ui.tableWidget->item(row_start, 0)->text().toLocal8Bit().data(), "0x%x", &i_start_addr);
			sscanf(ui.tableWidget->item(row_start + span_rows - 1, 0)->text().toLocal8Bit().data(), "0x%x", &i_end_addr);
			m_mapSpan[span_column].push_back(make_pair(i_start_addr, i_end_addr));		//����ϲ��Ĳ���
			ShowExcel();
		}
		else
		{
			string str_log = (boost::format("%s[ERROR]:�ӵ�4�п�ʼ�޷����ϲ�")%__FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
			return;
		}
	}
	else if ((k->key()&Qt::Key_N) == Qt::Key_N && (k->modifiers()&Qt::CTRL) == Qt::CTRL)
	{
		auto m_SelectItems = ui.tableWidget->selectedItems();
		//�����Ԫ��ϲ�
		int count = m_SelectItems.count();
		map<int, int> map_columns;
		map<int, int> map_rows;
		for (int i = 0; i < count; i++)
		{
			map_rows[m_SelectItems.at(i)->row()];					//��ȡѡ�е���
			map_columns[m_SelectItems.at(i)->column()];
		}
		if (count != 1)
		{
			string str_log = (boost::format("�뵥��ѡ��1����Ԫ�����ϲ�")).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		int col = map_columns.begin()->first;
		if (col == 0)
		{
			string str_text(ui.tableWidget->item(m_SelectItems.at(0)->row(), 0)->text().toLocal8Bit().data());	//��ȡ��һ��λ�ĵ�ַ����
			if (str_text.find('~') == std::string::npos)
			{
				string str_log = (boost::format("��ǰѡ�еĵ�ַ��%s,�޷������ַ�ϲ�") % str_text).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return;
			}
			//�ҳ���ʼ��ַ
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
				string str_log = (boost::format("��%d��,û�кϲ�����Ԫ��,����ϲ�ʧ��") % col).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return;
			}

			//���Ҹ��з��ϸõ�ַ�����pair,����о�ɾ����
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