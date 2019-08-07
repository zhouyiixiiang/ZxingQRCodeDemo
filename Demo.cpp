#include "stdafx.h"
#include "Demo.h"
#include "zxing-QRcode.h"
#include "Scanner.h"

#include "logFile.h"
//INITIALIZE_EASYLOGGINGPP

int FlagFinished;
int RecognizeFinished;

//Ϊ�˶�ά��ʶ�𵥶������߳�
//�̺߳���
//std::string Final_text;
DWORD WINAPI ThreadZxingQRcode(LPVOID lpParameter)
{
	int EndLoop = 0; //�߳̿�ʼʱ���ȱ�ע����̵߳Ľ���ѭ��Ϊ0������δ�������̵߳�ѭ��
	while (EndLoop == 0) {  //���ѭ��������־Ϊ0�������̻߳�δ����ѭ������ִ��ѭ��ȥʶ���ά��
		//�������while�������ж���ʶ��ͼ�����Ϊ�յ�ʱ�򣬸ò����˳�ʶ���ά��ѭ��
		while (IsEmptyQueue()) //��Ҫʶ���ά��Ķ���Ϊ��
		{
			if (FlagFinished == 0) {
				ELOGI( "queue is empty, FlagFinished=0,wait a moment");
				_sleep(0.1 * 1000); //���ɨ�������ɨ���־FlagFinishedΪ0����ȴ�0.1�룬�ٿ�IsEmptyQueue�Ƿ��Ƕ���Ϊ��
			}
			else {
				ELOGI("FlagFinished=1,break recognizing loop");
				EndLoop = 1;       //���ɨ�������ɨ���־FlagFinishedΪ1���򲻵ȴ����˳�ʶ��ѭ��
				break;
			}
		}
		//���ʶ���ά��Ķ���Ϊ�ǿ��ˣ����ж��Ƿ��˳���ʶ��ѭ��������˳�ʶ��ѭ���������ʶ�𣬷�����ʼʶ��
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
	//�����ť���Ҫʶ���ά��Ķ��к��������
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
	//����һ���߳�
	HANDLE thread = CreateThread(NULL, 0, ThreadZxingQRcode, NULL, 0, NULL);
	//CloseHandle(thread);
	//ui.pushButton_3->setText("eee");
	//ui.lineEdit->setText("eee");
	//ui.textEdit->setPlainText(Final_text.c_str());
	//ɨ��ͼ������߳�ɨ����ɺ��ڽ�ʶ������ӡ��Text�ı���֮ǰ���������̵߳ȴ�ʶ���ά���߳̽���
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

