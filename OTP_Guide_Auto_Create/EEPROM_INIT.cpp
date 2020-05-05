#include "stdafx.h"
#include "EEPROM_INIT.h"
#include "Burn_Station_ui.h"
#include "BurnAddrsAndChecksum.h"
#include "HexCheckExcel.h"

EEPROM_INIT::EEPROM_INIT(OTPGuideInfo& guide_info,QWidget *parent)
	: CMyWidgetBase(parent), m_GuideInfo(guide_info)
{
	ui.setupUi(this);
	ui.m_lineEdit_ProjectName->setEnabled(false);
	//string,ExcelProp
	//for (auto& excel:guide_info.m_VecBurnStationAddr)
	//{
	//	Qt_Excel* p_tmp = new Burn_Station_ui(excel.second);
	//	ui.m_tabWidget_BurnStation->addTab(p_tmp, QString::fromLocal8Bit(excel.first.c_str()));
	//}
	
	/*for (auto& excel : guide_info.m_mapBurnAddrs)
	{
		Qt_Excel* p_tmp = new Burn_Station_ui(excel.second);
		ui.m_tabWidget_BurnStation->addTab(p_tmp, QString::fromLocal8Bit(excel.first.c_str()));
	}*/

	if (m_GuideInfo.m_eepromInfo.m_EFlashExcel.m_vecHeaderLabels.empty())
	{
		m_GuideInfo.m_eepromInfo.m_EFlashExcel.m_vecHeaderLabels.push_back("�Ĵ���");
		m_GuideInfo.m_eepromInfo.m_EFlashExcel.m_vecHeaderLabels.push_back("��ֵ");
	}
	ui.m_ElashScrollArea->setWidget(new CHexCheckExcel(m_GuideInfo.m_eepromInfo.m_EFlashExcel));

	m_pBurnStationWidgetMenu = new QMenu(ui.m_tabWidget_BurnStation);
	QAction* p_InsertBurnStation = new QAction(QString::fromLocal8Bit("����һ����¼վ"), this);
	m_pBurnStationWidgetMenu->addAction(p_InsertBurnStation);
	connect(p_InsertBurnStation, &QAction::triggered, this, &EEPROM_INIT::BurnStationInsert);

	QAction* p_DeleteBurnStation = new QAction(QString::fromLocal8Bit("ɾ����ǰ��¼վ"), this);
	m_pBurnStationWidgetMenu->addAction(p_DeleteBurnStation);
	connect(p_DeleteBurnStation, &QAction::triggered, this, &EEPROM_INIT::DeleteBurnStation);

	for (const auto& stations : m_GuideInfo.m_mapStationInfo)
	{
		BurnAddrsAndChecksum* p_burnaddr = new BurnAddrsAndChecksum(m_GuideInfo,this,stations.first);
		ui.m_tabWidget_BurnStation->addTab(p_burnaddr, QString::fromLocal8Bit(stations.first.c_str()));
	}

	//ˢ�½���
	callback_ProtectcheckboxClick(m_GuideInfo.m_eepromInfo.m_bProtectEnable);
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
	string str_text = text.toLocal8Bit().data();
	if (m_GuideInfo.m_mapStationInfo.find(str_text) != m_GuideInfo.m_mapStationInfo.end())
	{
		string str_log = (boost::format("%s[ERROR]:�Ѿ��������¼վ%s��ģ��") % __FUNCTION__ %str_text).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}

	BurnAddrsAndChecksum* p_burnaddr = new BurnAddrsAndChecksum(m_GuideInfo,this,text.toLocal8Bit().data());
	ui.m_tabWidget_BurnStation->addTab(p_burnaddr, text);

	/*m_GuideInfo.m_VecBurnStationAddr.push_back(make_pair(text.toLocal8Bit().data(), ExcelProp()));
	Qt_Excel* p_tmp = new Burn_Station_ui(m_GuideInfo.m_VecBurnStationAddr.rbegin()->second);
	ui.m_tabWidget_BurnStation->addTab(p_tmp, QString::fromLocal8Bit(m_GuideInfo.m_VecBurnStationAddr.rbegin()->first.c_str()));*/
}

void EEPROM_INIT::DeleteBurnStation()
{
	int cur_burnstation_index = ui.m_tabWidget_BurnStation->currentIndex();
	if (cur_burnstation_index == -1)
	{
		string str_log = (boost::format("%s[ERROR]:û��ѡ���κ���¼վ,ɾ��ʧ��") % __FUNCTION__ ).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	string station_name =  ui.m_tabWidget_BurnStation->tabText(cur_burnstation_index).toLocal8Bit().data();
	ui.m_tabWidget_BurnStation->removeTab(cur_burnstation_index);
	m_GuideInfo.m_mapStationInfo.erase(station_name);
}

void EEPROM_INIT::callback_BurnStationAddrWidget_customContextMenuRequested(QPoint pt)
{
	m_pBurnStationWidgetMenu->popup(ui.m_tabWidget_BurnStation->mapToGlobal(pt));
}

void EEPROM_INIT::callback_textChanged(QString qstr)
{
	/*auto iter = std::find_if(m_GuideInfo.m_vecProjectInfo.m_vecData.begin(), m_GuideInfo.m_vecProjectInfo.m_vecData.end(), [](vector<string> vec_str)
	{
		return vec_str[0] == "��������";
	});
	(*iter)[1] = qstr.toLocal8Bit().data();*/
}

void EEPROM_INIT::UpdataWidget()
{
	disconnect(ui.m_lineEdit_SensorSlaveID, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	disconnect(ui.m_lineEdit_EEPROMSlaveID, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	disconnect(ui.m_lineEdit_DefaultBurn, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	disconnect(ui.m_lineEditProtectSlaveID, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	disconnect(ui.m_lineEditProtectAddr, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	disconnect(ui.m_lineEditProtectVal, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	disconnect(ui.m_lineEditIICMode, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));

	//ui.m_UsedChecksumlistWidget->clear();		//����δ����������Դ��checksumģ��
	ui.m_UsedConfigBurnModelWidget->clear();	//��Ҫ���õ���¼ģ���б�
	ui.m_lineEdit_ProjectName->setText(QString::fromLocal8Bit(m_GuideInfo.m_strProjectName.c_str()));
	ui.m_lineEdit_SensorSlaveID->setText(QString::fromLocal8Bit(m_GuideInfo.m_eepromInfo.m_str_sensor_slaveid.c_str()));
	ui.m_lineEdit_EEPROMSlaveID->setText(QString::fromLocal8Bit(m_GuideInfo.m_eepromInfo.m_str_eeprom_slaveid.c_str()));
	ui.m_lineEdit_DefaultBurn->setText(QString::fromLocal8Bit(m_GuideInfo.m_eepromInfo.m_str_DefaultVal.c_str()));
	ui.m_checkBoxProtect->setCheckState(m_GuideInfo.m_eepromInfo.m_bProtectEnable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
	ui.m_lineEditProtectSlaveID->setText(QString::fromLocal8Bit(m_GuideInfo.m_eepromInfo.m_str_protect_slaveid.c_str()));
	ui.m_lineEditProtectAddr->setText(QString::fromLocal8Bit(m_GuideInfo.m_eepromInfo.m_str_protectAddr.c_str()));
	ui.m_lineEditProtectVal->setText(QString::fromLocal8Bit(m_GuideInfo.m_eepromInfo.m_str_protectVal.c_str()));
	ui.m_lineEditIICMode->setText(QString::fromLocal8Bit(m_GuideInfo.m_eepromInfo.m_str_ProtectIICMode.c_str()));

	connect(ui.m_lineEdit_SensorSlaveID, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	connect(ui.m_lineEdit_EEPROMSlaveID, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	connect(ui.m_lineEdit_DefaultBurn, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	connect(ui.m_lineEditProtectSlaveID, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	connect(ui.m_lineEditProtectAddr, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	connect(ui.m_lineEditProtectVal, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));
	connect(ui.m_lineEditIICMode, SIGNAL(textChanged(QString)), this, SLOT(callback_ProtectTextChanged(QString)));

	//��ʾ��¼վ�����¼��ַ
	for (int i = 0; i < ui.m_tabWidget_BurnStation->count(); i++)
		((CMyWidgetBase*)ui.m_tabWidget_BurnStation->widget(i))->UpdataWidget();
	
	//ui.m_tabWidget_BurnStation->clear();
	//for (const auto& stations_name : m_GuideInfo.m_mapBurnAddrs)
	//{
	//	BurnAddrsAndChecksum* p_burnAddr = new BurnAddrsAndChecksum(m_GuideInfo, this, stations_name.first);
	//	ui.m_tabWidget_BurnStation->addTab(p_burnAddr, QString::fromLocal8Bit(stations_name.first.c_str()));
	//}

	//vector<string> vecChecksumList;
	//vector<string> vecBurnList;
	for (const auto items : m_GuideInfo.m_vecBurnItems)
	{
		ui.m_UsedConfigBurnModelWidget->addItem(QString::fromLocal8Bit(items->title.c_str()));
		/*vecChecksumList.push_back(items->title);
		vecBurnList.push_back(items->title);*/
	}

	/*for (const auto& stations : m_GuideInfo.m_mapStationInfo)
	{
		for (const auto& name : stations.second.m_VecNeedBurnName)
		{
			auto iter = std::find_if(vecBurnList.begin(), vecBurnList.end(), [name](string str)
			{
				return str == name;
			});
			if (iter != vecBurnList.end())
				vecBurnList.erase(iter);
		}

		for (const auto& name : stations.second.m_VecNeedChecksumName)
		{
			auto iter = std::find_if(vecChecksumList.begin(), vecChecksumList.end(), [name](string str)
			{
				return str == name;
			});
			if(iter != vecChecksumList.end())
				vecChecksumList.erase(iter);
		}
	}

	for (const auto& name : vecChecksumList)
		ui.m_UsedChecksumlistWidget->addItem(QString::fromLocal8Bit(name.c_str()));

	for (const auto& name : vecBurnList)
		ui.m_UsedConfigBurnModelWidget->addItem(QString::fromLocal8Bit(name.c_str()));
*/


	////��ʾ��Ҫ���õ�ģ��
	//vector<BurnItem> vec_burnItem;
	//for (auto it : m_GuideInfo.m_vecBurnItems)
	//{
	//	string algo_name = it->title;
	//	if (!it->m_CheckSumRangeExcel.m_vecData.empty())		//����Ǵ���checksum ��ַ�����,����Ҫ����checksum��������Դ
	//	{
	//		//���û���ڱ�ռ�õ�checksumģ��������,����ʾ
	//		if (std::find_if(m_GuideInfo.m_vecSpaceUsageCheckSum.begin(), m_GuideInfo.m_vecSpaceUsageCheckSum.end(), [algo_name](string& str)
	//		{
	//			return str == algo_name;
	//		}) == m_GuideInfo.m_vecSpaceUsageCheckSum.end())
	//		{
	//			ui.m_UsedChecksumlistWidget->addItem(QString::fromLocal8Bit(algo_name.c_str()));
	//		}
	//	}

	//	//����map�����е���¼ģ��,��������ʱ,������Ϊ������¼ģ������ʾ
	//	bool b_isExist = false;		//����������κ���¼վ�Ļ�����,��������б�����ʾ,���û�˫������
	//	for (const auto& it : m_GuideInfo.m_mapStationInfo) //m_mapVecNeedBurnName
	//	{
	//		string station_name = it.first;		//��ǰ����¼վ����

	//		if (std::find_if(it.second.m_VecNeedBurnName.begin(), it.second.m_VecNeedBurnName.end(), [algo_name](const string& str)
	//		{
	//			return str == algo_name;
	//		}
	//		) != it.second.m_VecNeedBurnName.end())
	//		{
	//			b_isExist = true;
	//		}
	//	}
	//	if (b_isExist == false)
	//	{
	//		ui.m_UsedConfigBurnModelWidget->addItem(QString::fromLocal8Bit(algo_name.c_str()));
	//	}
	//}
	callback_ProtectTextChanged("");	//�������й�16���Ƶ�����
}

//void EEPROM_INIT::callback_doubleClicked(QModelIndex index)
//{
//	string str_cur_text = ui.m_UsedChecksumlistWidget->currentItem()->text().toLocal8Bit().data();
//	int burn_station_index = ui.m_tabWidget_BurnStation->currentIndex();	//��ǰѡ������һ����¼վ��
//	if(burn_station_index == -1)
//	{
//		string str_log = (boost::format("%s[ERROR]:��ǰû��ѡ����¼վ��,��������checksum��ʹ������") % __FUNCTION__).str();
//		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
//		return;
//	}
//	string str_station_name = ui.m_tabWidget_BurnStation->tabText(burn_station_index).toLocal8Bit().data();	//��¼վ�������
//	m_GuideInfo.m_mapStationInfo[str_station_name].m_VecNeedChecksumName.push_back(str_cur_text);
//	//m_GuideInfo.m_vecSpaceUsageCheckSum.push_back(str_cur_text);
//
//	BurnAddrsAndChecksum* p_cur_burn_check = (BurnAddrsAndChecksum*)ui.m_tabWidget_BurnStation->widget(burn_station_index);
//	//�ҵ�����㷨��,��Ӧ��BurnItem��
//	auto iter = std::find_if(m_GuideInfo.m_vecBurnItems.begin(), m_GuideInfo.m_vecBurnItems.end(), [str_cur_text](std::shared_ptr<BurnItem> item)
//	{
//		return item->title == str_cur_text;
//	});
//	p_cur_burn_check->AddCheckSumConfigInfo(**iter);
//	UpdataWidget();	//�����б�
//}

void EEPROM_INIT::callback_ConfigBurnModelDoubleClicked(QModelIndex index)
{
	string str_cur_text = ui.m_UsedConfigBurnModelWidget->currentItem()->text().toLocal8Bit().data();
	int burn_station_index = ui.m_tabWidget_BurnStation->currentIndex();	//��ǰѡ������һ����¼վ��
	if (burn_station_index == -1)
	{
		string str_log = (boost::format("%s[ERROR]:��ǰû��ѡ����¼վ��,��������Ҫ��¼��ģ������") % __FUNCTION__).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	string str_station_name = ui.m_tabWidget_BurnStation->tabText(burn_station_index).toLocal8Bit().data();	//��¼վ�������

	auto iter = std::find_if(m_GuideInfo.m_mapStationInfo[str_station_name].m_VecNeedBurnName.begin(), m_GuideInfo.m_mapStationInfo[str_station_name].m_VecNeedBurnName.end(), [str_cur_text](const string& str)
	{
		return str_cur_text == str;
	});

	if (iter != m_GuideInfo.m_mapStationInfo[str_station_name].m_VecNeedBurnName.end())
	{
		string str_log = (boost::format("%s[ERROR]:��¼վ%s,�Ѿ���ӹ�%s����,�����ظ����") % __FUNCTION__ %str_station_name %(*iter)).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	else
	{
		m_GuideInfo.m_mapStationInfo[str_station_name].m_VecNeedBurnName.push_back(str_cur_text);
	}
	BurnAddrsAndChecksum* p_cur_burn_check = (BurnAddrsAndChecksum*)ui.m_tabWidget_BurnStation->widget(burn_station_index);
	p_cur_burn_check->ui.m_BurnModellistWidget->addItem(QString::fromLocal8Bit(str_cur_text.c_str()));	//���б�����Ӳ�����
	p_cur_burn_check->AddCheckSumTabByAlgoName(str_cur_text);

	UpdataWidget();	//�����б�
}

void EEPROM_INIT::callback_ProtectcheckboxClick(bool state)
{
	m_GuideInfo.m_eepromInfo.m_bProtectEnable = state;
	ui.m_lineEditProtectAddr->setEnabled(state);
	ui.m_lineEditProtectVal->setEnabled(state);
	ui.m_lineEditIICMode->setEnabled(state);
	ui.m_lineEditProtectSlaveID->setEnabled(state);
	UpdataWidget();
}

void EEPROM_INIT::callback_ProtectTextChanged(QString qstr)
{
	if (m_GuideInfo.m_eepromInfo.m_bProtectEnable)
	{
		//���������д��������,��Ҫ��������
		m_GuideInfo.m_eepromInfo.m_str_protect_slaveid = ui.m_lineEditProtectSlaveID->text().toLocal8Bit().data();
		m_GuideInfo.m_eepromInfo.m_str_protectAddr = ui.m_lineEditProtectAddr->text().toLocal8Bit().data();
		m_GuideInfo.m_eepromInfo.m_str_protectVal = ui.m_lineEditProtectVal->text().toLocal8Bit().data();
		m_GuideInfo.m_eepromInfo.m_str_ProtectIICMode = ui.m_lineEditIICMode->text().toLocal8Bit().data();
		QPalette pal = ui.m_lineEditProtectSlaveID->palette();
		if (!IsHex(m_GuideInfo.m_eepromInfo.m_str_protect_slaveid))
			pal.setColor(ui.m_lineEditProtectSlaveID->backgroundRole(), Qt::red);
		else
			pal.setColor(ui.m_lineEditProtectSlaveID->backgroundRole(), Qt::green);
		ui.m_lineEditProtectSlaveID->setPalette(pal);

		if (!IsHex(m_GuideInfo.m_eepromInfo.m_str_protectAddr))
			pal.setColor(ui.m_lineEditProtectAddr->backgroundRole(), Qt::red);
		else
			pal.setColor(ui.m_lineEditProtectAddr->backgroundRole(), Qt::green);
		ui.m_lineEditProtectAddr->setPalette(pal);

		pal = ui.m_lineEditProtectVal->palette();
		if (!IsHex(m_GuideInfo.m_eepromInfo.m_str_protectVal))
			pal.setColor(ui.m_lineEditProtectVal->backgroundRole(), Qt::red);
		else
			pal.setColor(ui.m_lineEditProtectVal->backgroundRole(), Qt::green);
		ui.m_lineEditProtectVal->setPalette(pal);

		pal = ui.m_lineEditIICMode->palette();
		if (!IsInteger(m_GuideInfo.m_eepromInfo.m_str_ProtectIICMode))
			pal.setColor(ui.m_lineEditIICMode->backgroundRole(), Qt::red);
		else
			pal.setColor(ui.m_lineEditIICMode->backgroundRole(), Qt::green);
		ui.m_lineEditIICMode->setPalette(pal);
	}
	
	//sensorSlaveid�Ļ�ȡ
	m_GuideInfo.m_eepromInfo.m_str_sensor_slaveid = ui.m_lineEdit_SensorSlaveID->text().toLocal8Bit().data();
	QPalette pal = ui.m_lineEdit_SensorSlaveID->palette();
	if (!IsHex(m_GuideInfo.m_eepromInfo.m_str_sensor_slaveid))
		pal.setColor(ui.m_lineEdit_SensorSlaveID->backgroundRole(), Qt::red);
	else
		pal.setColor(ui.m_lineEdit_SensorSlaveID->backgroundRole(), Qt::green);
	ui.m_lineEdit_SensorSlaveID->setPalette(pal);
	//eepromSlaveid�Ļ�ȡ
	m_GuideInfo.m_eepromInfo.m_str_eeprom_slaveid = ui.m_lineEdit_EEPROMSlaveID->text().toLocal8Bit().data();
	pal = ui.m_lineEdit_EEPROMSlaveID->palette();
	if (!IsHex(m_GuideInfo.m_eepromInfo.m_str_eeprom_slaveid))
		pal.setColor(ui.m_lineEdit_EEPROMSlaveID->backgroundRole(), Qt::red);
	else
		pal.setColor(ui.m_lineEdit_EEPROMSlaveID->backgroundRole(), Qt::green);
	ui.m_lineEdit_EEPROMSlaveID->setPalette(pal);
	//��¼Ĭ��ֵ
	m_GuideInfo.m_eepromInfo.m_str_DefaultVal = ui.m_lineEdit_DefaultBurn->text().toLocal8Bit().data();
	pal = ui.m_lineEdit_DefaultBurn->palette();
	if (!IsHex(m_GuideInfo.m_eepromInfo.m_str_DefaultVal))
		pal.setColor(ui.m_lineEdit_DefaultBurn->backgroundRole(), Qt::red);
	else
		pal.setColor(ui.m_lineEdit_DefaultBurn->backgroundRole(), Qt::green);
	ui.m_lineEdit_DefaultBurn->setPalette(pal);
}

void EEPROM_INIT::callback_StationTableCurrentChanged(int index)
{
	for (int i = 0; i < ui.m_tabWidget_BurnStation->count(); i++)
		((CMyWidgetBase*)ui.m_tabWidget_BurnStation->widget(i))->UpdataWidget();
}