#include "stdafx.h"
#include "Demo.h"
#include <QtWidgets/QApplication>
#include "Scanner.h"

#include "logFile.h"

//Scanner scan;
int main(int argc, char *argv[])
{
	ELOGI("Init Scanner");
	//Scanner scan;
	//scan.InitScanner();
	QApplication a(argc, argv);
	Demo w;
	w.show();
	return a.exec();
}
