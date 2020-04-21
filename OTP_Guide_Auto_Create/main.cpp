#include "stdafx.h"
#include "OTP_Guide_Auto_Create.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	OTP_Guide_Auto_Create w;
	w.show();
	return a.exec();
}
