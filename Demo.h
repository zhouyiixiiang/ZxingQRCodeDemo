#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Demo.h"
extern int FlagFinished;
extern std::string Final_text;

class Demo : public QMainWindow
{
	Q_OBJECT

public:
	Demo(QWidget *parent = Q_NULLPTR);

private:
	Ui::DemoClass ui;
private slots:
	void on_pushButton_clicked();
	void on_pushButton2_clicked();
	void on_pushButton3_clicked();
	void on_lineEdit_textEdited();
	//void 
	//void on_textEdit_textEdited();
};
