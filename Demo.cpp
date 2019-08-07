#include "stdafx.h"
#include "Demo.h"
#include "zxing-QRcode.h"
#include "Scanner.h"

#include "logFile.h"
//INITIALIZE_EASYLOGGINGPP

int FlagFinished;
int RecognizeFinished;

//为了二维码识别单独开的线程
//线程函数
//std::string Final_text;
DWORD WINAPI ThreadZxingQRcode(LPVOID lpParameter)
{
	int EndLoop = 0; //线程开始时，先标注这个线程的结束循环为0，即还未结束该线程的循环
	while (EndLoop == 0) {  //如果循环结束标志为0，即该线程还未结束循环，则执行循环去识别二维码
		//以下这个while是用来判定当识别图像队列为空的时候，该不该退出识别二维码循环
		while (IsEmptyQueue()) //当要识别二维码的队列为空
		{
			if (FlagFinished == 0) {
				ELOGI( "queue is empty, FlagFinished=0,wait a moment");
				_sleep(0.1 * 1000); //如果扫描机结束扫描标志FlagFinished为0，则等待0.1秒，再看IsEmptyQueue是否还是队列为空
			}
			else {
				ELOGI("FlagFinished=1,break recognizing loop");
				EndLoop = 1;       //如果扫描机结束扫描标志FlagFinished为1，则不等待，退出识别循环
				break;
			}
		}
		//如果识别二维码的队列为非空了，则判定是否退出了识别循环，如果退出识别循环，则结束识别，否则，则开始识别
		if (EndLoop == 1) {
			ELOGI("pic is not empty and Endloop=1");
			RecognizeFinished = 1;
			break;
		}
		ELOGI("recognize begin, pic queque is not empty");
		CString ImageName;
		ImageName = popImage();
		ZxingQRcode::recognize(ImageName);
		ELOGI("recognizing pic "<<ImageName<<" finished");
	}
	RecognizeFinished = 1;
	ELOGI("thread Zxing over");
	return 0L;
}

Demo::Demo(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void Demo::on_lineEdit_textEdited() {

}
void Demo::on_pushButton_clicked() {
}
void Demo::on_pushButton2_clicked() {
	//点击按钮清空要识别二维码的队列和输入参数
	//Final_text = "";
	ui.pushButton_2->setEnabled(false);
	ELOGI("click buttom start scan");
	RecognizeFinished = 0;
	FlagFinished = 0;
	//
	Scanner scan;
	scan.InitScanner();
	CString str;
	str=scan.m_FilePath;
	scan.StartScan();
	//创建一个线程
	HANDLE thread = CreateThread(NULL, 0, ThreadZxingQRcode, NULL, 0, NULL);
	//CloseHandle(thread);
	//ui.pushButton_3->setText("eee");
	//ui.lineEdit->setText("eee");
	//ui.textEdit->setPlainText(Final_text.c_str());
	//扫描图像的主线程扫描完成后，在将识别结果打印到Text文本框之前，先让主线程等待识别二维码线程结束
	while (RecognizeFinished == 0) {
		_sleep(0.1 * 1000);
		ELOGI("Recognize Finished=0,wait a moment");
		ELOGI("size of queue is "<<to_string(QueueOfImages.size()));
	}
	//ui.textEdit->setText("eee");
	ELOGI("Recognize Finished=1,recognizing finished");
	ui.textEdit->setPlainText(Final_text.c_str());
	ui.pushButton_2->setEnabled(true);
}
void Demo::on_pushButton3_clicked() {
	int a = 1;
	FlagFinished = 1;
	//ZxingQRcode::recognize();
}

