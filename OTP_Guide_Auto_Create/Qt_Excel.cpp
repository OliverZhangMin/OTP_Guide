#include "stdafx.h"
#include "Qt_Excel.h"
#include <QKeyEvent>
#include <boost/format.hpp>



Qt_Excel::Qt_Excel(ExcelProp& out_excel,QWidget *parent)
	:m_outExcel(out_excel), CMyWidgetBase(parent)
{
	ui.setupUi(this);
	QStringList headerLabels;
	for (const auto& label : m_outExcel.m_vecHeaderLabels)
		headerLabels << QString::fromLocal8Bit(label.c_str());
	ui.m_tableWidget->setColumnCount(out_excel.m_vecHeaderLabels.size());
	ui.m_tableWidget->setHorizontalHeaderLabels(headerLabels);
	ui.m_tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//ui.m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//���õ�Ԫ�񲻿ɱ༭
	/*setFocusPolicy(Qt::StrongFocus);
	this->grabKeyboard();*/

	ShowExcel();
}

Qt_Excel::~Qt_Excel()
{
}

bool Qt_Excel::InsertOneRow()
{
	auto selectItems = ui.m_tableWidget->selectedItems();

	vector<string> vec_str(m_outExcel.m_vecHeaderLabels.size());
	if (selectItems.size() == 0)
		m_outExcel.m_vecData.push_back(vec_str);
	else if (selectItems.size() == 1)
		m_outExcel.m_vecData.insert(m_outExcel.m_vecData.begin() + selectItems[0]->row(), vec_str);
	return true;
}

void Qt_Excel::keyPressEvent(QKeyEvent * k)
{
	if ((k->key() == Qt::Key_Insert))
	{
		auto selectItems = ui.m_tableWidget->selectedItems();
		if (selectItems.size() > 1)
		{
			string str_log = (boost::format("��ѡ��1��Ҫ�������,�ڰ���insert,���߲�Ҫѡ���κ���,ֱ�Ӱ���insert���뵽���һ��")).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		//����һ���յ�����
		if (!InsertOneRow())
		{
			string str_log = (boost::format("%s[ERROR]:����һ������ʧ��")%__FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		ShowExcel();
		return;
	}
	if (k->key() == Qt::Key_Delete)
	{
		if (m_iLastSelectRow == -1)
		{
			string str_log = (boost::format("%s[ERROR]:δѡ��ĳ��,����ɾ��") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		if (m_iLastSelectRow == 0 && m_outExcel.m_vecData.empty())
		{
			string str_log = (boost::format("%s[ERROR]:��ǰ�Ѿ�û�����ݿ���ɾ����") % __FUNCTION__).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		if (m_iLastSelectRow >= m_outExcel.m_vecData.size())	//���ѡ�е�����,�����˵�ǰ���ݽṹ���ܳ���,��ôֱ�����Ӽ���
		{
			return;
		}
		m_outExcel.m_vecData.erase(m_outExcel.m_vecData.begin() + m_iLastSelectRow);
		ShowExcel();
		return;
	}
	QWidget::keyPressEvent(k);
}

void Qt_Excel::UpdataWidget()
{
	ShowExcel();
}

void Qt_Excel::ShowExcel()
{
	disconnect(ui.m_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), ui.m_tableWidget, SLOT(resizeRowsToContents()));
	  
	//disconnect(ui.m_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_itemChanged(QTableWidgetItem*)));
	ui.m_tableWidget->clearSpans();		//������еĺϲ�
	ui.m_tableWidget->clear();
	ui.m_tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//��ͷ�������
	ui.m_tableWidget->setRowCount(m_outExcel.m_vecData.size());
	ui.m_tableWidget->setColumnCount(m_outExcel.m_vecHeaderLabels.size());

	QStringList list_headerLabels;
	for (const auto& label : m_outExcel.m_vecHeaderLabels)
		list_headerLabels.push_back(QString::fromLocal8Bit(label.c_str()));
	ui.m_tableWidget->setHorizontalHeaderLabels(list_headerLabels);

	for (int row = 0; row < m_outExcel.m_vecData.size(); row++)
		for (int col = 0; col < m_outExcel.m_vecData[row].size(); col++)
			ui.m_tableWidget->setItem(row, col, new QTableWidgetItem(QString::fromLocal8Bit(m_outExcel.m_vecData[row][col].c_str())));

	//������ʾ
	int rows = ui.m_tableWidget->rowCount(), cols = ui.m_tableWidget->columnCount();
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
		{
			ui.m_tableWidget->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//ui.m_tableWidget->item(i, j)->setFlags(ui.m_tableWidget->item(i, j)->flags() & (~Qt::ItemIsEditable));
		}
	connect(ui.m_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), ui.m_tableWidget, SLOT(resizeRowsToContents()));

	ui.m_tableWidget->resizeRowsToContents();
	//connect(ui.m_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(callback_itemChanged(QTableWidgetItem*)));
}

//void Qt_Excel::HideTextDesc()
//{
//	ui.m_textEdit_remark->setMaximumSize(QSize(0, 0));
//	ui.m_label_desc->setMaximumSize(QSize(0, 0));
//}

void Qt_Excel::callback_itemClicked(QTableWidgetItem* item) 
{
	m_iLastSelectRow = item->row();
	m_iLastSelectCol = item->column();
	//cout << "row-:" << m_iLastSelectRow << ",col- = " << m_iLastSelectCol << endl;
	/*ui.m_textEdit->setText(ui.m_tableWidget->item(m_iLastSelectRow, m_iLastSelectCol)->text());
	ui.m_textEdit->setFocus();
	QTextCursor cursor = ui.m_textEdit->textCursor();
	cursor.movePosition(QTextCursor::End);
	ui.m_textEdit->setTextCursor(cursor);*/
}

//void Qt_Excel::callback_textChanged()
//{
//	if (ui.m_tableWidget->rowCount() == 0)
//		return;
//	auto str = ui.m_textEdit->document()->toPlainText();
//	if (m_iLastSelectCol < 0 || m_iLastSelectCol >= m_outExcel.m_vecHeaderLabels.size())
//	{
//		string str_log = (boost::format("��ǰ�޸ĵĵ�%d��,��%d��,���ǵ�ǰ�����û��ά��������") % m_iLastSelectRow % m_iLastSelectCol).str();
//		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
//		return;
//	}
//	ui.m_tableWidget->item(m_iLastSelectRow, m_iLastSelectCol)->setText(str);
//	m_outExcel.m_vecData[m_iLastSelectRow][m_iLastSelectCol] = str.toLocal8Bit().data();
//}

void Qt_Excel::callback_itemDoubleClicked(QTableWidgetItem* item)
{
	int row = item->row();
	int col = item->column();
	string str_current_text = item->text().toLocal8Bit().data();
	CMyTextEdit* p_text = new CMyTextEdit(this, m_outExcel.m_vecData[row][col], str_current_text); //.m_vecData[m_iLastRow][m_iLastCol], this, BurnRule);
	ui.m_tableWidget->setCellWidget(row, col, p_text);
}

//void Qt_Excel::callback_itemSelectionChanged()
//{
//	//m_SelectItems = ui.m_tableWidget->selectedItems();
//	//if (m_SelectItems.size() == 0)
//	//	return;
//	//m_iLastSelectRow = m_SelectItems[0]->row();
//	//m_iLastSelectCol = m_SelectItems[0]->column();
//	//cout << "row--:" << m_iLastSelectRow << ",col-- = " << m_iLastSelectCol << endl;
//	//ui.m_textEdit->setText(ui.m_tableWidget->item(m_iLastSelectRow, m_iLastSelectCol)->text());
//	////ui.m_textEdit->setFocus();
//	//QTextCursor cursor = ui.m_textEdit->textCursor();
//	//cursor.movePosition(QTextCursor::End);
//	//ui.m_textEdit->setTextCursor(cursor);
//}

//void Qt_Excel::callback_cellClicked(int row, int col)
//{
//	m_iLastSelectRow = row;
//	m_iLastSelectCol = col;
//	cout << "row-:" << m_iLastSelectRow << ",col- = " << m_iLastSelectCol << endl;
//	ui.m_textEdit->setText(ui.m_tableWidget->item(m_iLastSelectRow, m_iLastSelectCol)->text());
//	ui.m_textEdit->setFocus();
//	QTextCursor cursor = ui.m_textEdit->textCursor();
//	cursor.movePosition(QTextCursor::End);
//	ui.m_textEdit->setTextCursor(cursor);
//}