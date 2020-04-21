#include "stdafx.h"
#include "TestItemContainer_UI.h"
#include "Burn_TestItem_UI.h"
#include <QMenu>
#include <QInputDialog>

TestItemContainer_UI::TestItemContainer_UI(OTPGuideInfo& guide_info,QWidget *parent)
	: QWidget(parent), m_guideInfo(guide_info)
{
	ui.setupUi(this);
	m_pTestItemContainertMenu = new QMenu(ui.m_tabWidget);
	QAction* p_InsertTestItem = new QAction(QString::fromLocal8Bit("����һ����¼��������Ϣ"), this);
	m_pTestItemContainertMenu->addAction(p_InsertTestItem);
	connect(p_InsertTestItem, &QAction::triggered, this, &TestItemContainer_UI::TestItemInsert);
}

TestItemContainer_UI::~TestItemContainer_UI()
{

}

void TestItemContainer_UI::callback_customContextMenuRequested(QPoint pt)
{
	m_pTestItemContainertMenu->popup(ui.m_tabWidget->mapToGlobal(pt));
}

void TestItemContainer_UI::TestItemInsert()
{
	bool ok;
	QString text = QInputDialog::getText(this, QString::fromLocal8Bit("Guide����¼��Ŀ����"), QString::fromLocal8Bit("��������¼������"), QLineEdit::Normal, 0, &ok);
	if (ok && !text.isEmpty())
	{
		m_guideInfo.m_vecBurnItems.push_back(BurnItem());
		auto& item = *(m_guideInfo.m_vecBurnItems.rbegin());
		item.title = text.toLocal8Bit().data();
		Burn_TestItem_UI* p = new Burn_TestItem_UI(item);
		ui.m_tabWidget->addTab(p, text);
	}
	else
	{
		string str_log = (boost::format("[ERROR]������¼������ʱ,������д�յ��ֶ�")).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
}