#include "CSDKExample.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    CSDKExample w;
	w.show();
	return a.exec();
}
