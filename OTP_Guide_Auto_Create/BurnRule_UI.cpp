#include "stdafx.h"
#include "BurnRule_UI.h"

MyCombox::MyCombox(QList<QString> qlist,QString qstr, string& str, BurnRule_UI* p, UI_TYPE type) :m_str(str), m_pBurnRule(p),ui_type(type)
{
	for (auto it : qlist)
	{
		addItem(it);
	}
	if (qstr.isEmpty())
	{
		setCurrentIndex(-1);
	}
	else
	{
		setCurrentText(qstr);
	}
	connect(this, SIGNAL(currentTextChanged(QString)), this, SLOT(callback_currentTextChanged(QString)));
}

void MyCombox::callback_currentTextChanged(QString qstr)
{
	m_str = qstr.toLocal8Bit().data();
	m_pBurnRule->ShowExcel(ui_type);
}

//void MyComboxSplit::focusOutEvent(QFocusEvent *e)
//{
//	m_str = currentText().toLocal8Bit().data();
//	m_pBurnRule->ShowBurnRuleExcel();
//}

BurnRule_UI::BurnRule_UI(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	
	//string filePath((strAppFolder + "\\DataSource.ini").GetBuffer());

	//boost::filesystem::path DataSourcePath(boost::filesystem::current_path());
	//DataSourcePath = DataSourcePath.parent_path().parent_path().append("DataSource.ini");
	ui.m_BurnRuleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.m_CalSumRangeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.m_ChecksumAddrTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	boost::assign::push_back(m_vecBurnRuleHeaderLabels)("地址")("烧录长度")("烧录数据")("拆分类型");
	boost::assign::push_back(m_vecCheckSumRangeHeaderLabels)("计算CheckSum开始地址")("计算CheckSum结束地址")("区间数据来源");
	boost::assign::push_back(m_vecCheckSumAddrHeaderLabels)("CheckSum填充地址")("CheckSum地址个数")("地址排序")("类型");
	
	m_QlistSplit.push_back("4Byte_H_L");
	m_QlistSplit.push_back("4Byte_L_H");
	m_QlistSplit.push_back("array");
	m_QlistSplit.push_back("ASCII");
	m_QlistSplit.push_back("Float_4Byte_H_L");
	m_QlistSplit.push_back("Float_4Byte_L_H");
	m_QlistSplit.push_back("HL");
	m_QlistSplit.push_back("L");
	m_QlistSplit.push_back("LH");
	m_QlistChecksumDataSource.push_back(QString::fromLocal8Bit("共享内存"));
	m_QlistChecksumDataSource.push_back(QString::fromLocal8Bit("EEPROM"));
	m_QlistChecksumAddrOrder.push_back("H_L");
	m_QlistChecksumAddrOrder.push_back("L");
	m_QlistChecksumAddrOrder.push_back("L_H");
	m_QlistChecksumCalType.push_back("0 (%255+1)");
	m_QlistChecksumCalType.push_back("1 (%256)");
	m_QlistChecksumCalType.push_back("2 (%255)");
	m_QlistChecksumCalType.push_back("3 (%65535)");
	m_QlistChecksumCalType.push_back("4 (%65535+1)");
	m_QlistChecksumCalType.push_back(QString::fromLocal8Bit("5 (直接求和)"));
	m_QlistChecksumCalType.push_back("6 (CRC)");
	m_QlistChecksumCalType.push_back("7 (CRC For DJ)");
	m_QlistChecksumCalType.push_back("8 (CRC32)");

	boost::property_tree::ptree pt;
	boost::property_tree::read_ini("DataSource.ini", pt);
	boost::property_tree::basic_ptree<string, string> AllKey_SubItems = pt.get_child("AllKey");
	//先存储所有的key
	for (const auto& AllKeyItemName : AllKey_SubItems)
	{
		if (m_mapJsonBurnKeyString.find(AllKeyItemName.first) == m_mapJsonBurnKeyString.end())
			m_mapJsonBurnKeyString[AllKeyItemName.first] = AllKeyItemName.second.data();
	}
	//找到所有key对应的struct
	for (const auto& keyProp : m_mapJsonBurnKeyString)
	{
		if (m_mapVecJsonStruct.find(keyProp.second) == m_mapVecJsonStruct.end())
		{
			boost::property_tree::basic_ptree<string, string> StructProperties = pt.get_child(keyProp.second);	//获取对应struct的节点总信息
			for (const auto& ItemNames : StructProperties)
				m_mapVecJsonStruct[keyProp.second].push_back(ItemNames.second.data());		//将每个小的struct的结构放入vector中
		}
	}
	//清空datasource列表
	for (const auto& KeyName : m_mapJsonBurnKeyString)
		ui.m_DataTypelistWidget->addItem(QString::fromLocal8Bit(KeyName.first.c_str()));
}

BurnRule_UI::~BurnRule_UI()
{
}

void BurnRule_UI::keyPressEvent(QKeyEvent * k)
{
	if ((k->key() == Qt::Key_Insert))
	{
		if (ui.m_BurnRuleTable->hasFocus())		//如果烧录规则有焦点,则往烧录规则表格中插入
		{
			int cur_row = ui.m_BurnRuleTable->currentRow();
			if (cur_row == -1)
			{
				vector<string> vec_tmp;
				boost::assign::push_back(vec_tmp)("0x")("1")("")("");
				m_vecBurnRule.push_back(vec_tmp);
			}
			else
			{
				vector<string> vec_tmp;
				boost::assign::push_back(vec_tmp)("0x")("1")("")("");
				m_vecBurnRule.insert(m_vecBurnRule.begin() + cur_row, vec_tmp);
			}
			ShowExcel(BurnRule);
		}
		else if (ui.m_CalSumRangeTable->hasFocus())
		{
			int cur_row = ui.m_CalSumRangeTable->currentRow();
			if (cur_row == -1)
			{
				vector<string> vec_tmp;
				boost::assign::push_back(vec_tmp)("0x")("0x")("");
				m_vecCheckSumRange.push_back(vec_tmp);
			}
			else
			{
				vector<string> vec_tmp;
				boost::assign::push_back(vec_tmp)("0x")("0x")("");
				m_vecCheckSumRange.insert(m_vecBurnRule.begin() + cur_row, vec_tmp);
			}
			ShowExcel(CheckSumRange);
		}
		else if (ui.m_ChecksumAddrTable->hasFocus())
		{
			if (ui.m_ChecksumAddrTable->rowCount() >= 1)
			{
				string str_log = (boost::format("%s[ERROR]:一个测试项只能有一段checksum!!!!!!") % __FUNCTION__).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return;
				return;
			}

			int cur_row = ui.m_ChecksumAddrTable->currentRow();
			if (cur_row == -1)
			{
				vector<string> vec_tmp;
				boost::assign::push_back(vec_tmp)("0x")("")("")("");
				m_vecCheckSumAddr.push_back(vec_tmp);
			}
			else
			{
				vector<string> vec_tmp;
				boost::assign::push_back(vec_tmp)("0x")("")("")("");
				m_vecCheckSumAddr.insert(m_vecBurnRule.begin() + cur_row, vec_tmp);
			}
			ShowExcel(ChecksumAddr);
		}
	}
	if (k->key() == Qt::Key_Delete)
	{
		/*	m_outExcel.m_vecData.erase(m_outExcel.m_vecData.begin() + m_iLastSelectRow);
			ShowExcel();
			return;*/
	}
	QWidget::keyPressEvent(k);

}

void BurnRule_UI::ShowExcel(UI_TYPE type)
{
	switch (type)
	{
	case BurnRule:
		ShowBurnRuleExcel();
		break;
	case CheckSumRange:
		ShowCheckSumRange();
		break;
	case ChecksumAddr:
		ShowCheckSumAddr();
		break;
	default:
		break;
	}
}

void BurnRule_UI::ShowBurnRuleExcel()
{
	disconnect(ui.m_BurnRuleTable, SIGNAL(itemChanged(QTableWidgetItem*)),this, SLOT(callback_BurnRuleItemChanged(QTableWidgetItem*)));
	ui.m_BurnRuleTable->clearSpans();		//清空所有的合并
	ui.m_BurnRuleTable->clear();
	ui.m_BurnRuleTable->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//表头字体居中
	ui.m_BurnRuleTable->setRowCount(m_vecBurnRule.size());
	ui.m_BurnRuleTable->setColumnCount(m_vecBurnRuleHeaderLabels.size());

	QStringList list_headerLabels;
	for (const auto& label : m_vecBurnRuleHeaderLabels)
		list_headerLabels.push_back(QString::fromLocal8Bit(label.c_str()));
	ui.m_BurnRuleTable->setHorizontalHeaderLabels(list_headerLabels);

	for(int row = 0;row<m_vecBurnRule.size();row++)
		for (int col = 0; col < m_vecBurnRuleHeaderLabels.size(); col++)
			ui.m_BurnRuleTable->setItem(row, col, new QTableWidgetItem(QString::fromLocal8Bit(m_vecBurnRule[row][col].c_str())));

	//数据有效性检查
	for (int row = 0; row < m_vecBurnRule.size(); row++)
		for (int col = 0; col < m_vecBurnRuleHeaderLabels.size(); col++)
		{
			string str = ui.m_BurnRuleTable->item(row, col)->text().toLocal8Bit().data();
			if (col == 0)
			{
				if (!IsHex(str))
					ui.m_BurnRuleTable->item(row, col)->setBackgroundColor(Qt::red);
				else
					ui.m_BurnRuleTable->item(row, col)->setBackgroundColor(Qt::green);
			}
			else
			{
				if (str.empty())
					ui.m_BurnRuleTable->item(row, col)->setBackgroundColor(Qt::red);
				else
					ui.m_BurnRuleTable->item(row, col)->setBackgroundColor(Qt::green);
			}
		}
	connect(ui.m_BurnRuleTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_BurnRuleItemChanged(QTableWidgetItem*)));
}

void BurnRule_UI::ShowCheckSumRange()
{
	disconnect(ui.m_CalSumRangeTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_ChecksumRangeItemChanged(QTableWidgetItem*)));
	ui.m_CalSumRangeTable->clearSpans();		//清空所有的合并
	ui.m_CalSumRangeTable->clear();
	ui.m_CalSumRangeTable->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//表头字体居中
	int rows = m_vecCheckSumRange.size();
	ui.m_CalSumRangeTable->setRowCount(rows);
	int cols = m_vecCheckSumRangeHeaderLabels.size();
	ui.m_CalSumRangeTable->setColumnCount(cols);
	
	QStringList list_headerLabels;
	for (const auto& label : m_vecCheckSumRangeHeaderLabels)
		list_headerLabels.push_back(QString::fromLocal8Bit(label.c_str()));
	ui.m_CalSumRangeTable->setHorizontalHeaderLabels(list_headerLabels);

	for (int row = 0; row < rows; row++)
		for (int col = 0; col < cols; col++)
			ui.m_CalSumRangeTable->setItem(row, col, new QTableWidgetItem(QString::fromLocal8Bit(m_vecCheckSumRange[row][col].c_str())));

	//数据有效性检查
	for (int row = 0; row < rows; row++)
		for (int col = 0; col < cols; col++)
		{
			string str = ui.m_CalSumRangeTable->item(row, col)->text().toLocal8Bit().data();
			if (0<= col && col <=1)
			{
				if (!IsHex(str))
					ui.m_CalSumRangeTable->item(row, col)->setBackgroundColor(Qt::red);
				else
					ui.m_CalSumRangeTable->item(row, col)->setBackgroundColor(Qt::green);
			}
			else
			{
				if (str.empty())
					ui.m_CalSumRangeTable->item(row, col)->setBackgroundColor(Qt::red);
				else
					ui.m_CalSumRangeTable->item(row, col)->setBackgroundColor(Qt::green);
			}
		}
	connect(ui.m_CalSumRangeTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_ChecksumRangeItemChanged(QTableWidgetItem*)));
}

void BurnRule_UI::ShowCheckSumAddr()
{
	disconnect(ui.m_ChecksumAddrTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_ChecksumAddrItemChanged(QTableWidgetItem*)));
	ui.m_ChecksumAddrTable->clearSpans();		//清空所有的合并
	ui.m_ChecksumAddrTable->clear();
	ui.m_ChecksumAddrTable->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//表头字体居中
	int rows = m_vecCheckSumAddr.size();
	ui.m_ChecksumAddrTable->setRowCount(rows);
	int cols = m_vecCheckSumAddrHeaderLabels.size();
	ui.m_ChecksumAddrTable->setColumnCount(cols);

	QStringList list_headerLabels;
	for (const auto& label : m_vecCheckSumAddrHeaderLabels)
		list_headerLabels.push_back(QString::fromLocal8Bit(label.c_str()));
	ui.m_ChecksumAddrTable->setHorizontalHeaderLabels(list_headerLabels);

	for (int row = 0; row < rows; row++)
		for (int col = 0; col < cols; col++)
			ui.m_ChecksumAddrTable->setItem(row, col, new QTableWidgetItem(QString::fromLocal8Bit(m_vecCheckSumAddr[row][col].c_str())));

	//数据有效性检查
	for (int row = 0; row < rows; row++)
		for (int col = 0; col < cols; col++)
		{
			string str = ui.m_ChecksumAddrTable->item(row, col)->text().toLocal8Bit().data();
			if (0 == col)
			{
				if (!IsHex(str))
					ui.m_ChecksumAddrTable->item(row, col)->setBackgroundColor(Qt::red);
				else
					ui.m_ChecksumAddrTable->item(row, col)->setBackgroundColor(Qt::green);
			}
			else if ( 1== col)
			{
				if (!IsInteger(str))
					ui.m_ChecksumAddrTable->item(row, col)->setBackgroundColor(Qt::red);
				else
					ui.m_ChecksumAddrTable->item(row, col)->setBackgroundColor(Qt::green);
			}
			else
			{
				if (str.empty())
					ui.m_ChecksumAddrTable->item(row, col)->setBackgroundColor(Qt::red);
				else
					ui.m_ChecksumAddrTable->item(row, col)->setBackgroundColor(Qt::green);
			}
		}
	connect(ui.m_ChecksumAddrTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_ChecksumAddrItemChanged(QTableWidgetItem*)));
}

void BurnRule_UI::callback_BurnRlueCurrentItemChanged(QTableWidgetItem* cur, QTableWidgetItem* pre)
{
	if (pre)
	{
		if (pre->column() == 3)
		{
			int pre_row = pre->row();
			int pre_col = pre->column();
			ui.m_BurnRuleTable->removeCellWidget(pre_row,pre_col);
		}
	}
	if (cur)
	{
		if (cur->column() == 2)
		{
			ui.m_DataSourceSplitlist->setEnabled(true);
		}
		else
			ui.m_DataSourceSplitlist->setEnabled(false);

		m_iLastRow = cur->row();
		m_iLastCol = cur->column();
		if (cur->column() == 3)
		{
			MyCombox* com = new MyCombox(m_QlistSplit,cur->text(), m_vecBurnRule[m_iLastRow][m_iLastCol], this,BurnRule);
			ui.m_BurnRuleTable->setCellWidget(cur->row(), 3, com);
		}
	}
}

void BurnRule_UI::callback_ChecksumRangeCurrentItemChanged(QTableWidgetItem* cur, QTableWidgetItem* pre)
{
	if (pre)
	{
		int pre_row = pre->row();
		int pre_col = pre->column();
		if (pre->column() == 2)
		{
			
			ui.m_CalSumRangeTable->removeCellWidget(pre_row, pre_col);
		}
	}
	if (cur)
	{
		m_iLastRow = cur->row();
		m_iLastCol = cur->column();
		if (cur->column() == 2)
		{
			MyCombox* com = new MyCombox(m_QlistChecksumDataSource, cur->text(), m_vecCheckSumRange[m_iLastRow][2], this, CheckSumRange);
			ui.m_CalSumRangeTable->setCellWidget(cur->row(), 2, com);
		}
	}
}

void BurnRule_UI::callback_ChecksumAddrCurrentItemChanged(QTableWidgetItem* cur, QTableWidgetItem* pre)
{
	if (pre)
	{
		int pre_row = pre->row();
		int pre_col = pre->column();
		if (pre->column() == 2 || pre->column() == 3)
		{
			ui.m_ChecksumAddrTable->removeCellWidget(pre_row, pre_col);
		}
	}
	if (cur)
	{
		m_iLastRow = cur->row();
		m_iLastCol = cur->column();
		if (cur->column() == 2)
		{
			MyCombox* com = new MyCombox(m_QlistChecksumAddrOrder, cur->text(), m_vecCheckSumAddr[m_iLastRow][2], this, ChecksumAddr);
			ui.m_ChecksumAddrTable->setCellWidget(cur->row(), 2, com);
		}
		else if (cur->column() == 3)
		{
			MyCombox* com = new MyCombox(m_QlistChecksumCalType, cur->text(), m_vecCheckSumAddr[m_iLastRow][3], this, ChecksumAddr);
			ui.m_ChecksumAddrTable->setCellWidget(cur->row(), 3, com);
		}
	}
}

void BurnRule_UI::callback_BurnRuleItemChanged(QTableWidgetItem* item)
{
	int row = item->row();
	int col = item->column();
	m_vecBurnRule[row][col] = item->text().toLocal8Bit().data();
	ShowExcel(BurnRule);
}

void BurnRule_UI::callback_ChecksumRangeItemChanged(QTableWidgetItem* item)
{
	int row = item->row();
	int col = item->column();
	m_vecCheckSumRange[row][col] = item->text().toLocal8Bit().data();
	ShowExcel(CheckSumRange);
}

void BurnRule_UI::callback_ChecksumAddrItemChanged(QTableWidgetItem* item)
{
	int row = item->row();
	int col = item->column();
	m_vecCheckSumAddr[row][col] = item->text().toLocal8Bit().data();
	ShowExcel(ChecksumAddr);
}

void BurnRule_UI::callback_DataFilterTextChanged(QString qstr)
{
	string cstr_edit = qstr.toLocal8Bit().data();
	if (cstr_edit.empty())
	{
		ui.m_DataTypelistWidget->clear();
		for (const auto& key : m_mapJsonBurnKeyString)
			ui.m_DataTypelistWidget->addItem(QString::fromLocal8Bit(key.first.c_str()));
	}
	else
	{
		ui.m_DataTypelistWidget->clear();
		vector<string> vec_str;
		//先保存有哪些key满足要求的
		for (const auto& key : m_mapJsonBurnKeyString)
		{
			if (IsFrontBy(key.first, cstr_edit))
				vec_str.push_back(key.first);
		}
		//一次性刷新这些key
		for (const auto& key : vec_str)
			ui.m_DataTypelistWidget->addItem(QString::fromLocal8Bit(key.c_str()));
	}
}

void BurnRule_UI::callback_DataTypeCurrentTextChanged(QString qstring)
{
	ui.m_DataSourceSplitlist->clear();
	string str_cur_sel = qstring.toLocal8Bit().data();
	for (const auto& struct_prop : m_mapVecJsonStruct[m_mapJsonBurnKeyString[str_cur_sel]])
	{
		string str_result = str_cur_sel + "." + struct_prop;
		ui.m_DataSourceSplitlist->addItem(QString::fromLocal8Bit(str_result.c_str()));
	}
}

void BurnRule_UI::callback_DataSourceCurrentTextChanged(QString qstr)
{
	m_vecBurnRule[m_iLastRow][2] = qstr.toLocal8Bit().data();
	ShowBurnRuleExcel();
}