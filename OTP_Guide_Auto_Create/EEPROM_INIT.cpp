#include "stdafx.h"
#include "EEPROM_INIT.h"
#include "Burn_Station_ui.h"
#include "BurnAddrsAndChecksum.h"

EEPROM_INIT::EEPROM_INIT(OTPGuideInfo& guide_info,QWidget *parent)
	: CMyWidgetBase(parent), m_GuideInfo(guide_info)
{
	ui.setupUi(this);
	
	//string,ExcelProp
	for (auto& excel:guide_info.m_VecBurnStationAddr)
	{
		Qt_Excel* p_tmp = new Burn_Station_ui(excel.second);
		ui.m_tabWidget_BurnStation->addTab(p_tmp, QString::fromLocal8Bit(excel.first.c_str()));
	}
	
	m_pBurnStationWidgetMenu = new QMenu(ui.m_tabWidget_BurnStation);
	QAction* p_InsertBurnStation = new QAction(QString::fromLocal8Bit("����һ����¼վ"), this);
	m_pBurnStationWidgetMenu->addAction(p_InsertBurnStation);
	connect(p_InsertBurnStation, &QAction::triggered, this, &EEPROM_INIT::BurnStationInsert);
	//
}

EEPROM_INIT::~EEPROM_INIT()
{

}

void EEPROM_INIT::keyPressEvent(QKeyEvent * k)
{
	if ((k->key() == Qt::Key_Insert))
	{
	
	}
}

void EEPROM_INIT::BurnStationInsert()
{
	bool ok;
	QString text = QInputDialog::getText(this, QString::fromLocal8Bit("��¼վ������"), QString::fromLocal8Bit("��������¼վ������,����TO1,T02"), QLineEdit::Normal, 0, &ok);
	if (!ok && text.isEmpty())
	{
		string str_log = (boost::format("%s[ERROR]:���������ʱ,������д�յ��ֶ�") % __FUNCTION__).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	BurnAddrsAndChecksum* p_burnaddr = new BurnAddrsAndChecksum(m_GuideInfo,this);
	ui.m_tabWidget_BurnStation->addTab(p_burnaddr, text);

	/*m_GuideInfo.m_VecBurnStationAddr.push_back(make_pair(text.toLocal8Bit().data(), ExcelProp()));
	Qt_Excel* p_tmp = new Burn_Station_ui(m_GuideInfo.m_VecBurnStationAddr.rbegin()->second);
	ui.m_tabWidget_BurnStation->addTab(p_tmp, QString::fromLocal8Bit(m_GuideInfo.m_VecBurnStationAddr.rbegin()->first.c_str()));*/
}

void EEPROM_INIT::callback_BurnStationAddrWidget_customContextMenuRequested(QPoint pt)
{
	m_pBurnStationWidgetMenu->popup(ui.m_tabWidget_BurnStation->mapToGlobal(pt));
}

void EEPROM_INIT::callback_textChanged(QString qstr)
{
	auto iter = std::find_if(m_GuideInfo.m_vecProjectInfo.m_vecData.begin(), m_GuideInfo.m_vecProjectInfo.m_vecData.end(), [](vector<string> vec_str)
	{
		return vec_str[0] == "��������";
	});
	(*iter)[1] = qstr.toLocal8Bit().data();
}

void EEPROM_INIT::callback_textChangedBurnDefaultVal(QString qstr)
{
	string str_buff = qstr.toLocal8Bit().data();
	QPalette pal = ui.m_lineEdit_DefaultBurn->palette();
	if (!IsHex(str_buff))
	{
		pal.setColor(ui.m_lineEdit_DefaultBurn->backgroundRole(), Qt::red);
	}
	else
	{
		pal.setColor(ui.m_lineEdit_DefaultBurn->backgroundRole(), Qt::green);
	}
	ui.m_lineEdit_DefaultBurn->setPalette(pal);
}

void EEPROM_INIT::UpdataWidget()
{
	ui.m_UsedChecksumlistWidget->clear();		//����δ����������Դ��checksumģ��

	vector<BurnItem> vec_burnItem;
	for (auto it : m_GuideInfo.m_vecBurnItems)
	{
		string algo_name = it.title;
		if (!it.m_vecCheckSumRange.empty())		//����Ǵ���checksum ��ַ�����,����Ҫ����checksum��������Դ
		{
			//���û���ڱ�ռ�õ�checksumģ��������,����ʾ
			if (std::find_if(m_GuideInfo.m_vecSpaceUsageCheckSum.begin(), m_GuideInfo.m_vecSpaceUsageCheckSum.end(), [algo_name](string& str)
			{
				return str == algo_name;
			}) == m_GuideInfo.m_vecSpaceUsageCheckSum.end())
			{
				ui.m_UsedChecksumlistWidget->addItem(QString::fromLocal8Bit(algo_name.c_str()));
			}
		}
	}
}

void EEPROM_INIT::callback_doubleClicked(QModelIndex index)
{
	string str_cur_text = ui.m_UsedChecksumlistWidget->currentItem()->text().toLocal8Bit().data();
	int burn_station_index = ui.m_tabWidget_BurnStation->currentIndex();	//��ǰѡ������һ����¼վ��
	if(burn_station_index == -1)
	{
		string str_log = (boost::format("%s[ERROR]:��ǰû��ѡ����¼վ��,��������checksum��ʹ������") % __FUNCTION__).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	m_GuideInfo.m_vecSpaceUsageCheckSum.push_back(str_cur_text);
	BurnAddrsAndChecksum* p_cur_burn_check = (BurnAddrsAndChecksum*)ui.m_tabWidget_BurnStation->widget(burn_station_index);
	//�ҵ�����㷨��,��Ӧ��BurnItem��
	auto iter = std::find_if(m_GuideInfo.m_vecBurnItems.begin(), m_GuideInfo.m_vecBurnItems.end(), [str_cur_text](BurnItem& item)
	{
		return item.title == str_cur_text;
	});
	p_cur_burn_check->AddCheckSumConfigInfo(*iter);
	UpdataWidget();	//�����б�
}