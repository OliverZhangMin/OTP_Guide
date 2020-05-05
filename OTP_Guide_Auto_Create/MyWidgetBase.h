#pragma once
#include <QWidget>

class CMyWidgetBase :public QWidget
{
public:
	CMyWidgetBase(QWidget* parent) :QWidget(parent) 
	{
	}
	virtual void UpdataWidget() = 0;  //更新UI界面
	virtual void ShowExcel() {};
};