#include "stdafx.h"
#include "TestItemContainer_UI.h"
#include "Burn_TestItem_UI.h"
#include "BurnRule_UI.h"
#include <QMenu>
#include <QInputDialog>
#include <qtabwidget.h>

TestItemContainer_UI::TestItemContainer_UI(OTPGuideInfo& guide_info,QWidget *parent)
	: CMyWidgetBase(parent), m_guideInfo(guide_info)
{
	ui.setupUi(this);
	m_pTestItemContainertMenu = new QMenu(ui.m_tabWidget);
	QAction* p_InsertTestItem = new QAction(QString::fromLocal8Bit("新增一个烧录测试项信息"), this);
	m_pTestItemContainertMenu->addAction(p_InsertTestItem);
	connect(p_InsertTestItem, &QAction::triggered, this, &TestItemContainer_UI::TestItemInsert);
}

TestItemContainer_UI::~TestItemContainer_UI()
{

}

Json::Value TestItemContainer_UI::GetJsons()
{
	Json::Value all_json;
	for (int i = 0; i < ui.m_tabWidget->count(); i++)
	{
		QTabWidget* p_tab = (QTabWidget*)ui.m_tabWidget->widget(i);

		string algo_name = ui.m_tabWidget->tabText(i).toLocal8Bit().data();
		BurnRule_UI* p_burn_rule = (BurnRule_UI*)p_tab->widget(1);
		Json::Value json_tmp;
		if (!p_burn_rule->GetRuleJson(json_tmp))
		{
			string str_log = (boost::format("%s[ERROR]:模块%s获取json数据失败")%__FUNCTION__ %algo_name).str();
			QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
			return Json::Value();
		}
		all_json[algo_name] = json_tmp;
	}
	ostringstream os;
	Json::StreamWriterBuilder swb_cloud_burn;
	unique_ptr<Json::StreamWriter> writer_cloud(swb_cloud_burn.newStreamWriter());
	writer_cloud->write(all_json, &os);
	ofstream ofile("out.json", ios::binary | ios::out);
	ofile << os.str();
	ofile.close();
	return all_json;
}

void TestItemContainer_UI::callback_customContextMenuRequested(QPoint pt)
{
	m_pTestItemContainertMenu->popup(ui.m_tabWidget->mapToGlobal(pt));
}

void TestItemContainer_UI::TestItemInsert()
{
	bool ok;
	QString text = QInputDialog::getText(this, QString::fromLocal8Bit("Guide中烧录项目名称"), QString::fromLocal8Bit("请输入烧录项名称"), QLineEdit::Normal, 0, &ok);
	if (ok && !text.isEmpty())
	{
		m_guideInfo.m_vecBurnItems.push_back(BurnItem());
		auto& item = *(m_guideInfo.m_vecBurnItems.rbegin());
		item.title = text.toLocal8Bit().data();
		Burn_TestItem_UI* p_desc = new Burn_TestItem_UI(item);
		BurnRule_UI* p_rule = new BurnRule_UI(item);
		QTabWidget* p = new QTabWidget;
		p->addTab(p_desc, QString::fromLocal8Bit("描述细节"));
		p->addTab(p_rule, QString::fromLocal8Bit("烧录规则"));
		ui.m_tabWidget->addTab(p, text);
	}
	else
	{
		string str_log = (boost::format("[ERROR]插入烧录测项名时,不能填写空的字段")).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
}

void TestItemContainer_UI::UpdataWidget()
{

}