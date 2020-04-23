#include "stdafx.h"
#include "BurnAddrsAndChecksum.h"
#include <QKeyEvent>

BurnAddrsAndChecksum::BurnAddrsAndChecksum(OTPGuideInfo& guide_info,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

BurnAddrsAndChecksum::~BurnAddrsAndChecksum()
{

}

void BurnAddrsAndChecksum::ShowExcel()
{
	disconnect(ui.m_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_BurnAddrWidgetItemChanged(QTableWidgetItem*)));
	ui.m_tableWidget->clearSpans();		//清空所有的合并
	ui.m_tableWidget->clearContents();
	ui.m_tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//表头字体居中
	ui.m_tableWidget->setRowCount(m_vecBurnAddrs.size());
	ui.m_tableWidget->setColumnCount(2);

	/*QStringList list_headerLabels;
	for (const auto& label : m_vecBurnRuleHeaderLabels)
		list_headerLabels.push_back(QString::fromLocal8Bit(label.c_str()));
	ui.m_BurnRuleTable->setHorizontalHeaderLabels(list_headerLabels);*/

	for (int row = 0; row < m_vecBurnAddrs.size(); row++)
		for (int col = 0; col < 2; col++)
		{
			ui.m_tableWidget->setItem(row, col, new QTableWidgetItem(QString::fromLocal8Bit(m_vecBurnAddrs[row][col].c_str())));
			if (!IsHex(m_vecBurnAddrs[row][col]))
				ui.m_tableWidget->item(row, col)->setBackgroundColor(Qt::red);
			else
				ui.m_tableWidget->item(row, col)->setBackgroundColor(Qt::green);
		}
	connect(ui.m_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_BurnAddrWidgetItemChanged(QTableWidgetItem*)));
}

void BurnAddrsAndChecksum::keyPressEvent(QKeyEvent * k)
{
	if ((k->key() == Qt::Key_Insert))
	{
		if (ui.m_tableWidget->hasFocus())		//如果烧录地址窗口有焦点,则往烧录规则表格中插入
		{
			int cur_row = ui.m_tableWidget->currentRow();
			if (cur_row == -1)
			{
				vector<string> vec_tmp;
				boost::assign::push_back(vec_tmp)("0x")("0x");
				m_vecBurnAddrs.push_back(vec_tmp);
			}
			else
			{
				vector<string> vec_tmp;
				boost::assign::push_back(vec_tmp)("0x")("0x");
				m_vecBurnAddrs.insert(m_vecBurnAddrs.begin() + cur_row, vec_tmp);
			}
			ShowExcel();
		}
	}
}

void BurnAddrsAndChecksum::callback_BurnAddrWidgetItemChanged(QTableWidgetItem* item)
{
	int row = item->row();
	int col = item->column();
	string m_str = item->text().toLocal8Bit().data();
	m_vecBurnAddrs[row][col] = m_str;
	ShowExcel();
}