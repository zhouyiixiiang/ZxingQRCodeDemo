#include "stdafx.h"
#include <string>
#include <fstream>
#include <zxing/LuminanceSource.h>
#include <zxing/common/Counted.h>
#include <zxing/Reader.h>
#include <zxing/aztec/AztecReader.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/DecodeHints.h>
#include <zxing/datamatrix/DataMatrixReader.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/pdf417/PDF417Reader.h>
#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/oned/CodaBarReader.h>
#include <zxing/oned/Code39Reader.h>
#include <zxing/oned/Code93Reader.h>
#include <zxing/oned/Code128Reader.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include "MatSource.h"
#include "zxing-QRcode.h"
#include <qmessagebox.h>
#include <zxing/NotFoundException.h>
#include "Scanner.h"
#include "Demo.h"

#include "logFile.h"

std::string Final_text;

//��LPCWSTRת��ΪLPCSTR�������һ���ַ�����ʽת���ĺ�������
char* wtoc1(wchar_t* wText)
{
	DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, wText, -1, NULL, 0, NULL, FALSE);//�ѵ�����������NULL�ĵ����ַ����ĳ��Ȱ�����β��
	char* psText = NULL;
	psText = new char[dwNum];
	if (!psText)
	{
		delete[]psText;
		psText = NULL;
	}
	WideCharToMultiByte(CP_ACP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	return psText;
}


int ZxingQRcode::recognize(CString image_name)//ʶ���ά��Ĺ��ܴ���
{
	//DefQueue queue;
	//image_name =queue.popImage();
	//std::string image_name = "./CRCode/Image_R1.bmp";
	//image_name = popImage();
	ELOGI("start recognizing");
	LPCSTR s2;
	LPWSTR s1 = image_name.GetBuffer(0);
	s2 = wtoc1(s1);
	//��ȡs2�ַ�����6λ
	std::string s3 = s2; //������Ϊ�˽���������ӡ�����ڶ�ȡ6λ
	int length = strlen(s2);
	std::string s4,s5;
	s4 = s3.substr(length - 12, length); //��ȡͼ�����Ƶĺ�12λ�����ڽ����ӡ��ʶ��ͼ������
	s5 = s3.substr(length - 12, 8); //��ȡͼ�����Ƶĵ���12λ��������4λ�����ڱ���ʶ������ͼ������

	//
	ELOGI("reading pic, pic name"<<s5);
	cv::Mat matSrc = cv::imread(s2, 1); //��ȡɨ���ͼ����ͼ��Ϊ��ͼ���ж����ά��
	ELOGI("read pic, pic name" << s5<< " over");
	if (!matSrc.data) {
		ELOGI("read pic, pic name" << s5 << " error");
		//QMessageBox::warning(NULL, "warning", "read image error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		ELOGI("read pic, pic name" << s5 << " error end");
		return -1;
	}
	ELOGI("read pic successfull, pic name" << s5);
	//��ȡ�ֲ���ά��
	int p_height = matSrc.rows;
	int p_width = matSrc.cols;
	int each_height = p_height / 7;  //��þֲ���ά�����ڷ���ĸ߶�
	int each_width = p_width / 5;   //��þֲ���ά�����ڷ���Ŀ��
	cv::Mat m_matSrc[35];            //����һ���ֲ�ͼ����������žֲ�ͼ��
	int k = 0;
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 5; j++) {
			ELOGI("getting small pic, no. "<<to_string(i)<<"-"<<to_string(j));
			m_matSrc[k] = matSrc(cv::Rect(each_width * j, each_height * i, each_width, each_height));
			//��һ��ѭ��������ͼ��ȡÿ����ά�����ڵľֲ�ͼ��ʹ��opencv::Rect�������ʼ���ص㣬�߶���ʼ���ص㣬��ȣ��߶ȣ�����ȡ�ֲ�ͼ��
			k += 1;
		}
	}
	//�鿴�ֲ���ȡЧ��
	//for (int k = 0; k < 35; k++) {
	//	cv::imshow("picture",m_matSrc[k]);
	//	cv::waitKey(0);
	//}
	Final_text += "\n--------------------------------------------------\n";
	Final_text += "picture name: ";
	Final_text += s4;
	Final_text += "\n--------------------------------------------------\n";

	for (int k = 0; k < 35; k++) {
		ELOGI("Recognizing picture name: "<<s4<<" , number:"<<to_string(k));
		cv::Mat matGray;
		cv::cvtColor(m_matSrc[k], matGray, CV_BGR2GRAY); //�������������󣬽���ͼ��ҶȻ�����
		//��ͼ������˲���������4���˲������Ա�Ч�������ʹ����ֵ�˲�
		int ii = 1;
		cv::Mat matBlur;
		//cv::blur(matGray, matBlur, cv::Size(ii, ii), cv::Point(-1, -1));
		//��˹�˲�
		//cv::Mat matGaussian;
		//cv::GaussianBlur(matGray, matGaussian, cv::Size(ii, ii), 0.0, 0.0, 4);
		//��ֵ�˲�
		cv::Mat matMedian;
		cv::medianBlur(matGray, matMedian, ii + 2);
		//��ֵ���˲�
		//cv::Mat matBilateral;
		//cv::bilateralFilter(matGray, matBilateral, ii, ii * 2, ii / 2);
		//�����˲�������Ա��˲�Ч��
		//cv::imwrite("E:/documents/casitworkspace/zxing-QRcode1/QRCode/gray.jpg", matGray);
		//cv::imwrite("E:/documents/casitworkspace/zxing-QRcode1/QRCode/matBlur.jpg", matBlur);
		//cv::imwrite("E:/documents/casitworkspace/zxing-QRcode1/QRCode/matGaussian.jpg", matGaussian);
		cv::imwrite("E:/documents/casitworkspace/zxing-QRcode1/QRCode/median.jpg", matMedian);
		//cv::imwrite("E:/documents/casitworkspace/zxing-QRcode1/QRCode/matBilateral.jpg", matBilateral);
		//����ͬbitmap��ȡ��һ����׼�ӿ��У��Ա�����һ�����greyscale luminance value
		zxing::Ref<zxing::LuminanceSource> source = MatSource::create(matMedian);
		int width = source->getWidth();
		int height = source->getHeight();
		fprintf(stderr, "image width: %d, height: %d\n", width, height);
		zxing::Ref<zxing::Reader> reader; //��ͼ��Ķ�ά�����Ϊ�������ַ���
		reader.reset(new zxing::qrcode::QRCodeReader);
		try
		{
			//��Luminance dataת��Ϊ1 bit ����
//zxing::Ref<zxing::Binarizer> binarizer(new zxing::GlobalHistogramBinarizer(source));
			//��Luminance dataת��Ϊ1 bit ����
			zxing::Ref<zxing::Binarizer> binarizer(new zxing::HybridBinarizer(source));
			//������ʾ1 bit data�� core bitmap
			zxing::Ref<zxing::BinaryBitmap> bitmap(new zxing::BinaryBitmap(binarizer));
			//��barcode�Ľ�����������һ��ͼ����
			zxing::Ref<zxing::Result> result(reader->decode(bitmap, zxing::DecodeHints(zxing::DecodeHints::QR_CODE_HINT)));
			std::string str2 = result->getText()->getText();
			ELOGI("recognizing pic success,pic number is"<<to_string(k));
			//��ӡ���
			Final_text += str2;
			Final_text += "     ";
			if ((k+1) % 5==0 ) {
				Final_text += "\n";//���ѭ������k+1���ñ�5�������ͻ��д�ӡ���
			}
		}
		catch (zxing::ReaderException e) {
			//�����ʶ���ά������г����쳣�����򱣴汨��ʶ���ͼƬ��badImagePath�ļ���
			ELOGI("recognizing pic faild,pic number is" << to_string(k));
			CString badImagePath;
			//CString strTemp;
			badImagePath = m_FilePath_i;
			badImagePath += "Bad\\";
			//CString s5 = s4;
			badImagePath += s5.c_str();
			badImagePath += "_";
			badImagePath += to_string(k).c_str();
			badImagePath += ".png";
			//strTemp.Format(L"Bad\\%s", s4);
			//badImagePath += strTemp;
			LPWSTR ss = badImagePath.GetBuffer(0);
			LPSTR ss1 = wtoc1(ss); //�ַ����ļ���ʽת��
			cv::imwrite(ss1, m_matSrc[k]);
			Final_text += "Fault"; //ʶ����󣬴�ӡ���ΪFault
			Final_text += "   ";
			if ((k + 1) % 5 == 0) {
				Final_text += "\n";//���ѭ������k+1���ñ�5�������ͻ��д�ӡ���
			}
		}
	}

	return 0;
}