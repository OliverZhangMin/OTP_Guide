#include"stdafx.h"
#include "ChecksumConfigurate_ui.h"

ChecksumConfigurate_ui::ChecksumConfigurate_ui(BurnItem& item, QWidget *parent)
	: CMyWidgetBase(parent),/* m_vecCheckSumRange(item.m_vecCheckSumRange),*/ m_Item(item)
{
	ui.setupUi(this);
	ui.m_tableWidgetChecksumDataSource->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	string str_text_edit = "";	//打印需要用到的checksum的地址信息
	for (const auto& range : m_Item.m_CheckSumRangeExcel.m_vecData)
	{
		str_text_edit += (boost::format("%s ~ %s\n") % range[0] % range[1]).str();
	}
	ui.m_textEditMustUsedChecksumAddrs->setText(QString::fromLocal8Bit(str_text_edit.c_str()));
	m_QlistEEPROM_DataSource.push_back(QString::fromLocal8Bit("EEPROM"));
	m_QlistEEPROM_DataSource.push_back(QString::fromLocal8Bit("共享内存"));
}

ChecksumConfigurate_ui::~ChecksumConfigurate_ui()
{

}

void ChecksumConfigurate_ui::UpdataWidget()
{
	ShowExcel();
}

void ChecksumConfigurate_ui::callback_customContextMenuRequested(QPoint pt)
{

}

void ChecksumConfigurate_ui::callback_itemChanged(QTableWidgetItem* qItem)
{
	int row = qItem->row();
	int col = qItem->column();
	m_Item.m_CheckSumDataSourceRnage.m_vecData[row][col] = qItem->text().toLocal8Bit().data();
	ShowExcel();
}

void ChecksumConfigurate_ui::keyPressEvent(QKeyEvent * k)
{
	if ((k->key() == Qt::Key_Insert))
	{
		int cur_row = ui.m_tableWidgetChecksumDataSource->currentRow();
		if (cur_row == -1)
		{
			vector<string> vec_tmp;
			boost::assign::push_back(vec_tmp)("0x")("0x")("");
			m_Item.m_CheckSumDataSourceRnage.m_vecData.push_back(vec_tmp);
		}
		else
		{
			vector<string> vec_tmp;
			boost::assign::push_back(vec_tmp)("0x")("0x")("");
			m_Item.m_CheckSumDataSourceRnage.m_vecData.insert(m_Item.m_CheckSumDataSourceRnage.m_vecData.begin() + cur_row, vec_tmp);
		}
		ShowExcel();
		return;
	}
	if (k->key() == Qt::Key_Delete)
	{
		return;
	}
	QWidget::keyPressEvent(k);
}

void ChecksumConfigurate_ui::callback_DataSourceCurrentItemChanged(QTableWidgetItem* cur, QTableWidgetItem* pre)
{
	if (pre)
	{
		if (pre->column() == 2)
		{
			int pre_row = pre->row();
			int pre_col = pre->column();
			ui.m_tableWidgetChecksumDataSource->removeCellWidget(pre_row, pre_col);
		}
	}
	if (cur)
	{
		/*if (cur->column() == 2)
		{
			ui.m_DataSourceSplitlist->setEnabled(true);
		}
		else
			ui.m_DataSourceSplitlist->setEnabled(false);*/

		m_iLastRow = cur->row();
		m_iLastCol = cur->column();
		if (cur->column() == 2)
		{
			MyEEPROMDataSourceComboBox* com = new MyEEPROMDataSourceComboBox(m_QlistEEPROM_DataSource, cur->text(), m_Item.m_CheckSumDataSourceRnage.m_vecData[m_iLastRow][m_iLastCol], this);
			ui.m_tableWidgetChecksumDataSource->setCellWidget(cur->row(), 2, com);
		}
	}
}

void ChecksumConfigurate_ui::ShowExcel()
{
	disconnect(ui.m_tableWidgetChecksumDataSource, SIGNAL(itemChanged(QTableWidgetItem*)), ui.m_tableWidgetChecksumDataSource, SLOT(resizeRowsToContents()));
	disconnect(ui.m_tableWidgetChecksumDataSource, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_itemChanged(QTableWidgetItem*)));
	ui.m_tableWidgetChecksumDataSource->clearSpans();		//清空所有的合并
	ui.m_tableWidgetChecksumDataSource->clear();
	ui.m_tableWidgetChecksumDataSource->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//表头字体居中
	int rows = m_Item.m_CheckSumDataSourceRnage.m_vecData.size();
	int cols = m_Item.m_CheckSumDataSourceRnage.m_vecHeaderLabels.size();
	ui.m_tableWidgetChecksumDataSource->setRowCount(rows);
	ui.m_tableWidgetChecksumDataSource->setColumnCount(cols);

	QStringList list_headerLabels;
	for (const auto& label : m_Item.m_CheckSumDataSourceRnage.m_vecHeaderLabels)
		list_headerLabels.push_back(QString::fromLocal8Bit(label.c_str()));
	ui.m_tableWidgetChecksumDataSource->setHorizontalHeaderLabels(list_headerLabels);

	for (int row = 0; row < rows; row++)
		for (int col = 0; col < cols; col++)
		{
			ui.m_tableWidgetChecksumDataSource->setItem(row, col, new QTableWidgetItem(QString::fromLocal8Bit(m_Item.m_CheckSumDataSourceRnage.m_vecData[row][col].c_str())));
			ui.m_tableWidgetChecksumDataSource->item(row,col)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
	
	connect(ui.m_tableWidgetChecksumDataSource, SIGNAL(itemChanged(QTableWidgetItem*)), ui.m_tableWidgetChecksumDataSource, SLOT(resizeRowsToContents()));
	connect(ui.m_tableWidgetChecksumDataSource, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_itemChanged(QTableWidgetItem*)));
}

MyEEPROMDataSourceComboBox::MyEEPROMDataSourceComboBox(QList<QString> qlist, QString qstr, string& str, ChecksumConfigurate_ui* p):m_str(str), m_pBurnRule(p)
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

void MyEEPROMDataSourceComboBox::callback_currentTextChanged(QString qstr)
{
	m_str = qstr.toLocal8Bit().data();
	m_pBurnRule->ShowExcel();
}