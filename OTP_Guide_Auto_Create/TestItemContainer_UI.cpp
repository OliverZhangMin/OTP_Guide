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

	QAction* p_DeleteTestItem = new QAction(QString::fromLocal8Bit("删除当前烧录测试项信息"), this);
	m_pTestItemContainertMenu->addAction(p_DeleteTestItem);
	connect(p_DeleteTestItem, &QAction::triggered, this, &TestItemContainer_UI::DeleteTestItem);
	
	UpdataWidget();
}

TestItemContainer_UI::~TestItemContainer_UI()
{

}

Json::Value TestItemContainer_UI::GetJsonByAlgoName(const std::string& str_algo)
{
	for (int i = 0; i < ui.m_tabWidget->count(); i++)
	{
		string algo_name = ui.m_tabWidget->tabText(i).toLocal8Bit().data();
		if (str_algo == algo_name)
		{
			QTabWidget* p_tab = (QTabWidget*)ui.m_tabWidget->widget(i);
			BurnRule_UI* p_burn_rule = (BurnRule_UI*)p_tab->widget(1);
			Json::Value json_tmp;
			if (!p_burn_rule->GetRuleJson(json_tmp))
			{
				string str_log = (boost::format("%s[ERROR]:模块%s获取json数据失败") % __FUNCTION__ %algo_name).str();
				QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
				return Json::Value();
			}
			return json_tmp;
		}
	}
	return Json::Value();
	/*ostringstream os;
	Json::StreamWriterBuilder swb_cloud_burn;
	unique_ptr<Json::StreamWriter> writer_cloud(swb_cloud_burn.newStreamWriter());
	writer_cloud->write(all_json, &os);
	ofstream ofile("out.json", ios::binary | ios::out);
	ofile << os.str();
	ofile.close();*/
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
		std::shared_ptr<BurnItem> m_pBurnItem(new BurnItem);
		m_pBurnItem->title = text.toLocal8Bit().data();
		m_guideInfo.m_vecBurnItems.push_back(m_pBurnItem);
		//m_guideInfo.m_vecBurnItems.rbegin()->title = text.toLocal8Bit().data();
		//auto& item = *();
		//item.title = text.toLocal8Bit().data();

		Burn_TestItem_UI* p_desc = new Burn_TestItem_UI(m_pBurnItem);
		BurnRule_UI* p_rule = new BurnRule_UI( m_pBurnItem);
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

void TestItemContainer_UI::DeleteTestItem()
{
	int cur_index = ui.m_tabWidget->currentIndex();
	if (cur_index == -1)
	{
		string str_log = (boost::format("%s[ERROR]:当前没有测项名模块被选中,删除失败") % __FUNCTION__).str();
		QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit(str_log.c_str()), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	//获取当前的测试名
	string str_lab_name = ui.m_tabWidget->tabText(cur_index).toLocal8Bit().data();
	auto iter = std::find_if(m_guideInfo.m_vecBurnItems.begin(), m_guideInfo.m_vecBurnItems.end(), [str_lab_name](std::shared_ptr<BurnItem> p)
	{
		return p->title == str_lab_name;
	});
	m_guideInfo.m_vecBurnItems.erase(iter);
	ui.m_tabWidget->removeTab(cur_index);
}

void TestItemContainer_UI::UpdataWidget()
{
	ui.m_tabWidget->clear();
	for (auto& p_BurnItem : m_guideInfo.m_vecBurnItems)
	{
		Burn_TestItem_UI* p_desc = new Burn_TestItem_UI(p_BurnItem);
		BurnRule_UI* p_rule = new BurnRule_UI(p_BurnItem);
		QTabWidget* p = new QTabWidget;
		p->addTab(p_desc, QString::fromLocal8Bit("描述细节"));
		p->addTab(p_rule, QString::fromLocal8Bit("烧录规则"));
		ui.m_tabWidget->addTab(p, QString::fromLocal8Bit(p_BurnItem->title.c_str()));
	}
}