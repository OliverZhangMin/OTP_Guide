#include "stdafx.h"
#include "BurnAddrsAndChecksum.h"
#include "ChecksumConfigurate_ui.h"
#include "EEPROM_INIT.h"
#include <QKeyEvent>
#include <QMenu>

BurnAddrsAndChecksum::BurnAddrsAndChecksum(OTPGuideInfo& guide_info,EEPROM_INIT* p_init, string station_name,QWidget *parent)
	: QWidget(parent), 
	m_GuideInfo(guide_info),	//�ܵ�OTP Guide������
	m_pInitWidget(p_init),		//��ʼ�����ڵ�ָ��
	m_BurnExcel(guide_info.m_mapBurnAddrs[station_name])	//��¼��ַ��excel
{
	ui.setupUi(this);
	ui.m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	if (m_BurnExcel.m_vecHeaderLabels.empty())
		boost::assign::push_back(m_BurnExcel.m_vecHeaderLabels)("��¼��ʼ��ַ")("��¼������ַ");
	
	m_pMenuCheckSumConfig = new QMenu(ui.m_ChecksumConfigtabWidget);
	QAction* p_DeleteCheckSumConfig = new QAction(QString::fromLocal8Bit("ɾ����ǰchecksum������Դ"), this);
	m_pMenuCheckSumConfig->addAction(p_DeleteCheckSumConfig);
	connect(p_DeleteCheckSumConfig, &QAction::triggered, this, &BurnAddrsAndChecksum::callback_DeleteCurrentCheckSumConfig);
}

BurnAddrsAndChecksum::~BurnAddrsAndChecksum()
{

}

void BurnAddrsAndChecksum::AddCheckSumConfigInfo(BurnItem& item)
{
	ChecksumConfigurate_ui* p_checksumConfig = new ChecksumConfigurate_ui(item);
	string algo_name = item.title;
	ui.m_ChecksumConfigtabWidget->addTab(p_checksumConfig , QString::fromLocal8Bit(algo_name.c_str()));
}

void BurnAddrsAndChecksum::ShowExcel()
{
	disconnect(ui.m_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_BurnAddrWidgetItemChanged(QTableWidgetItem*)));
	ui.m_tableWidget->clearSpans();		//������еĺϲ�
	ui.m_tableWidget->clearContents();
	ui.m_tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//��ͷ�������
	ui.m_tableWidget->setRowCount(m_BurnExcel.m_vecData.size());
	ui.m_tableWidget->setColumnCount(2);

	/*QStringList list_headerLabels;
	for (const auto& label : m_vecBurnRuleHeaderLabels)
		list_headerLabels.push_back(QString::fromLocal8Bit(label.c_str()));
	ui.m_BurnRuleTable->setHorizontalHeaderLabels(list_headerLabels);*/

	for (int row = 0; row < m_BurnExcel.m_vecData.size(); row++)
		for (int col = 0; col < 2; col++)
		{
			ui.m_tableWidget->setItem(row, col, new QTableWidgetItem(QString::fromLocal8Bit(m_BurnExcel.m_vecData[row][col].c_str())));
			if (!IsHex(m_BurnExcel.m_vecData[row][col]))
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
		if (ui.m_tableWidget->hasFocus())		//�����¼��ַ�����н���,������¼�������в���
		{
			InsertOneRow();
			ShowExcel();
		}
	}
	if (k->key() == Qt::Key_Delete)
	{
		if (m_iLastSelectRow == -1)
		{
			string str_log = (boost::format("%s[ERROR]:δѡ��ĳ��,����ɾ��") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		if (m_iLastSelectRow == 0 && m_BurnExcel.m_vecData.empty())
		{
			string str_log = (boost::format("%s[ERROR]:��ǰ�Ѿ�û�����ݿ���ɾ����") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		m_BurnExcel.m_vecData.erase(m_BurnExcel.m_vecData.begin() + m_iLastSelectRow);
		ShowExcel();
		return;
	}
}

bool BurnAddrsAndChecksum::InsertOneRow()
{
	auto selectItems = ui.m_tableWidget->selectedItems();
	vector<string> vec_tmp;
	boost::assign::push_back(vec_tmp)("0x")("0x");
	if (selectItems.size() == 0)
		m_BurnExcel.m_vecData.push_back(vec_tmp);
	else if (selectItems.size() == 1)
		m_BurnExcel.m_vecData.insert(m_BurnExcel.m_vecData.begin() + selectItems[0]->row(), vec_tmp);
	return true;
}

void BurnAddrsAndChecksum::callback_BurnAddrWidgetItemChanged(QTableWidgetItem* item)
{
	int row = item->row();
	int col = item->column();
	string m_str = item->text().toLocal8Bit().data();
	m_BurnExcel.m_vecData[row][col] = m_str;
	ShowExcel();
}

void BurnAddrsAndChecksum::callback_ChecksumConfigcustomContextMenuRequested(QPoint pt)
{
	m_pMenuCheckSumConfig->popup(ui.m_ChecksumConfigtabWidget->mapToGlobal(pt));
}

void BurnAddrsAndChecksum::callback_DeleteCurrentCheckSumConfig()
{
	int cur_select_index = ui.m_ChecksumConfigtabWidget->currentIndex();
	if (cur_select_index == -1)
	{
		string str_log = (boost::format("%s[ERROR]:��ǰû��ѡ���tab,�޷�ɾ��")%__FUNCTION__).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	string algo_name = ui.m_ChecksumConfigtabWidget->tabText(cur_select_index).toLocal8Bit().data();	//��ȡ��ǰѡ�е�tab������
	
	auto iter = std::find_if(m_GuideInfo.m_vecSpaceUsageCheckSum.begin(), m_GuideInfo.m_vecSpaceUsageCheckSum.end(), [algo_name](string& str)	// �ҵ���tab���ֶ�Ӧ��ռ��chekcsum�����еĵ�����λ��
	{
		return algo_name == str;
	}
	);
	m_GuideInfo.m_vecSpaceUsageCheckSum.erase(iter);	//ɾ���õ�����
	ui.m_ChecksumConfigtabWidget->removeTab(cur_select_index);
	m_pInitWidget->UpdataWidget();
}

void BurnAddrsAndChecksum::callback_ItemClicked(QTableWidgetItem* item)
{
	m_iLastSelectRow = item->row();
	m_iLastSelectCol = item->column();
}