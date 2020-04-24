#include"stdafx.h"
#include "ChecksumConfigurate_ui.h"

ChecksumConfigurate_ui::ChecksumConfigurate_ui(BurnItem& item,QWidget *parent)
	: QWidget(parent), m_vecCheckSumRange(item.m_vecCheckSumRange)
{
	ui.setupUi(this);
	string str_text_edit = "";	//打印需要用到的checksum的地址信息
	for (const auto& range : m_vecCheckSumRange)
	{
		str_text_edit += (boost::format("%s ~ %s\n") % range[0] % range[1]).str();
	}
	ui.m_textEditMustUsedChecksumAddrs->setText(QString::fromLocal8Bit(str_text_edit.c_str()));
}

ChecksumConfigurate_ui::~ChecksumConfigurate_ui()
{

}

void ChecksumConfigurate_ui::callback_customContextMenuRequested(QPoint pt)
{

}