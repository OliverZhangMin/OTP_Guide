#pragma once

#include <QWidget>
#include "ui_ChecksumConfigurate_ui.h"
#include <vector>
#include <string>

struct BurnItem;
class ChecksumConfigurate_ui : public QWidget
{
	Q_OBJECT

public:
	ChecksumConfigurate_ui(BurnItem& item,QWidget *parent = Q_NULLPTR);
	~ChecksumConfigurate_ui();

private:
	Ui::ChecksumConfigurate_ui ui;
	std::vector<std::vector<std::string>>& m_vecCheckSumRange;
};
