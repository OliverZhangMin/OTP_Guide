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

BurnRule_UI::BurnRule_UI(BurnItem& bItem,QWidget *parent)
	: QWidget(parent),m_vecBurnRule(bItem.m_vecBurnRule),
	m_vecCheckSumRange(bItem.m_vecCheckSumRange),
	m_vecCheckSumAddr(bItem.m_vecCheckSumAddr),
	m_vecBurnRuleHeaderLabels(bItem.m_vecBurnRuleHeaderLabels),
	m_vecCheckSumRangeHeaderLabels(bItem.m_vecCheckSumRangeHeaderLabels),
	m_vecCheckSumAddrHeaderLabels(bItem.m_vecCheckSumAddrHeaderLabels)
{
	ui.setupUi(this);
	//string filePath((strAppFolder + "\\DataSource.ini").GetBuffer());

	//boost::filesystem::path DataSourcePath(boost::filesystem::current_path());
	//DataSourcePath = DataSourcePath.parent_path().parent_path().append("DataSource.ini");
	ui.m_BurnRuleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.m_CalSumRangeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.m_ChecksumAddrTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	
	if(m_vecBurnRuleHeaderLabels.empty())
		boost::assign::push_back(m_vecBurnRuleHeaderLabels)("地址")("烧录长度")("烧录数据")("拆分类型");
	if (m_vecCheckSumRangeHeaderLabels.empty())
		boost::assign::push_back(m_vecCheckSumRangeHeaderLabels)("计算CheckSum开始地址")("计算CheckSum结束地址")/*("区间数据来源")*/;
	if (m_vecCheckSumAddrHeaderLabels.empty())
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
	/*m_QlistChecksumDataSource.push_back(QString::fromLocal8Bit("共享内存"));
	m_QlistChecksumDataSource.push_back(QString::fromLocal8Bit("EEPROM"));*/
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
				boost::assign::push_back(vec_tmp)("0x")("0x");
				m_vecCheckSumRange.push_back(vec_tmp);
			}
			else
			{
				vector<string> vec_tmp;
				boost::assign::push_back(vec_tmp)("0x")("0x");
				m_vecCheckSumRange.insert(m_vecCheckSumRange.begin() + cur_row, vec_tmp);
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
				m_vecCheckSumAddr.insert(m_vecCheckSumAddr.begin() + cur_row, vec_tmp);
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
			if (!IsHex(str))
				ui.m_CalSumRangeTable->item(row, col)->setBackgroundColor(Qt::red);
			else
				ui.m_CalSumRangeTable->item(row, col)->setBackgroundColor(Qt::green);
			
		}
	connect(ui.m_CalSumRangeTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_ChecksumRangeItemChanged(QTableWidgetItem*)));
}

bool BurnRule_UI::CommonConvert(Json::Value& out_json, const CSubBurnProp& prop, BurnMedium medium)
{
	Json::Value sub_json;
	if (prop.m_strSplit == "ASCII")
	{
		int index_count = 0;
		for (int i = prop.address; i < prop.address + prop.length; i++)
		{
			sub_json["data"] = (boost::format("%s:[%d,1,ascii]") % prop.m_strData %index_count++).str();
			sub_json["length"] = 1;
			sub_json["outconvert"] = "L";
			char buff[64];
			sprintf_s(buff, "0x%x", i);
			if (medium == EEPROM_Medium)
			{
				sub_json["start"] = buff;
			}
			else
			{
				sub_json["start"] = i;
			}
			out_json.append(sub_json);
		}
	}
	else
	{
		sub_json["data"] = prop.m_strData;
		sub_json["length"] = prop.length;
		sub_json["outconvert"] = prop.m_strSplit;
		char buff[64];
		sprintf_s(buff, "0x%x", prop.address);
		if (medium == EEPROM_Medium)
		{
			sub_json["start"] = buff;
		}
		else
		{
			sub_json["start"] = prop.address;
		}
		out_json.append(sub_json);
	}
	return true;
}

bool BurnRule_UI::GetRuleJson(Json::Value& out_json)
{
	//数据有效性检查
	for (auto& rules : m_vecBurnRule)
	{
		if (!IsHex(rules[0]))
		{
			string str_log = (boost::format("%s [ERROR]: 地址栏位不是16进制字符串") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return false;
		}
		if (!IsInteger(rules[1]))
		{
			string str_log = (boost::format("%s [ERROR]:烧录长度不是10进制") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return false;
		}

		if (rules[2].empty() || rules[3].empty())
		{
			string str_log = (boost::format("%s [ERROR]:烧录数据和拆分类型不能是空") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return false;
		}
	}
	for (auto& rang : m_vecCheckSumRange)
	{
		if (!IsHex(rang[1]) || !IsHex(rang[0]))
		{
			string str_log = (boost::format("%s [ERROR]: 计算checksum的地址必须得是16进制") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return false;
		}
	}
	for (auto& sums : m_vecCheckSumAddr)
	{
		if (!IsHex(sums[0]))
		{
			string str_log = (boost::format("%s [ERROR]:checksum填充地址必须是16进制") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return false;
		}
		if (!IsInteger(sums[1]))
		{
			string str_log = (boost::format("%s [ERROR]:checksum填充地址长度必须是整数") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return false;
		}
		if (sums[2].empty() || sums[3].empty())
		{
			string str_log = (boost::format("%s [ERROR]:地址排序和类型不能为空") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return false;
		}
	}

	//拼接成json数据
	Json::Value Burn_Data;
	for (int i = 0; i < m_vecBurnRule.size(); i++)
	{
		try
		{
			CSubBurnProp prop;
			sscanf(m_vecBurnRule[i][0].c_str(), "%x", &prop.address);
			sscanf(m_vecBurnRule[i][1].c_str(), "%d", &prop.length);
			if (prop.length <= 0)
			{
				string str_log = (boost::format("%s [ERROR]:烧录长度不能<=0") % __FUNCTION__).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return false;
			}
			prop.m_strData = m_vecBurnRule[i][2];
			prop.m_strSplit = m_vecBurnRule[i][3];
			CommonConvert(Burn_Data, prop,EEPROM_Medium);
		}
		catch (const exception& ex)
		{
			string str_log = (boost::format("%s [ERROR]: 在填写烧录数据时的第%d行的数据长度有错误,不是有效的10进制数,错误信息:%s") % __FUNCTION__%i %ex.what()).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return false;
		}
	}

	Json::Value CheckSum_Object;
	Json::Value checksum_UsedRange;
	Json::Value checksum_Addr;
	for (int i = 0; i < m_vecCheckSumRange.size(); i++)
	{
		Json::Value range;
		range["start"] = m_vecCheckSumRange[i][0];
		range["end"] = m_vecCheckSumRange[i][1];
		checksum_UsedRange.append(range);
	}
	CheckSum_Object["UsedRange"] = checksum_UsedRange;		//checksum使用到的区间

	try
	{
		if (!m_vecCheckSumAddr.empty())
		{
			//存在checksum地址
			if (m_vecCheckSumRange.empty())
			{
				string str_log = (boost::format("%s [ERROR]: 要配置checksum的地址,必须配置checksum所需要用到的空间") % __FUNCTION__).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return false;
			}

			int i_checksum_addr = 0;
			sscanf(m_vecCheckSumAddr[0][0].c_str(), "%x", &i_checksum_addr);

			int i_checksum_length = 0;
			sscanf(m_vecCheckSumAddr[0][1].c_str(), "%d", &i_checksum_length);

			int order_type = -1;
			if (m_vecCheckSumAddr[0][2] == "L")
				order_type = 0;
			else if (m_vecCheckSumAddr[0][2] == "H_L")
				order_type = 1;
			else if (m_vecCheckSumAddr[0][2] == "L_H")
				order_type = 2;

			char buff[256];
			switch (order_type)
			{
			case 0:
				if (i_checksum_length != 1)
				{
					string str_log = (boost::format("%s [ERROR]: L的拆分类型对应的checksum长度只能是1") % __FUNCTION__).str();
					QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
					return false;
				}
				sprintf_s(buff, "0x%x", i_checksum_addr);
				checksum_Addr.append(buff);
				break;
			case 1:
				for (int i = i_checksum_length - 1; i >= 0; i--)
				{
					sprintf_s(buff, "0x%x", i_checksum_addr + i);
					checksum_Addr.append(buff);
				}
				break;
			case 2:
				for (int i = 0; i < i_checksum_length; i++)
				{
					sprintf_s(buff, "0x%x", i_checksum_addr + i);
					checksum_Addr.append(buff);
				}
				break;
			default:
				//AfxMessageBox(_T("未定义的拆分类型,导致无法计算checksum类型"));
				return false;
				break;
			}
			CheckSum_Object["CheckSum_Addr"] = checksum_Addr;	//记录checksum用到了哪些地址
			string str_cal_type = m_vecCheckSumAddr[0][3];
			str_cal_type = str_cal_type.substr(0, 1);
			CheckSum_Object["CheckSumType"] = boost::lexical_cast<int>(str_cal_type);
		}
		else
		{
			//不存在checksum地址
			if (!m_vecCheckSumRange.empty())
			{
				//存在checksum计算区间 - 不能成立
				string str_log = (boost::format("%s [ERROR]: 用户配置了checksum的区间却没有配置checksum的地址,如果不需要配置chekcsum请不要填写checksum的区间") % __FUNCTION__).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return false;
			}
		}
	}
	catch (const exception& ex)
	{
		string str_log = (boost::format("%s [ERROR]: 生成json时出现异常,异常信息:%s") % __FUNCTION__ %ex.what()).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return false;
	}
	out_json["CheckSum"] = CheckSum_Object;
	out_json["BurnData"] = Burn_Data;
	return true;
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
		/*if (cur->column() == 2)
		{
			MyCombox* com = new MyCombox(m_QlistChecksumDataSource, cur->text(), m_vecCheckSumRange[m_iLastRow][2], this, CheckSumRange);
			ui.m_CalSumRangeTable->setCellWidget(cur->row(), 2, com);
		}*/
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
	if (m_iLastRow == -1)
		return;
	m_vecBurnRule[m_iLastRow][2] = qstr.toLocal8Bit().data();
	ShowBurnRuleExcel();
}