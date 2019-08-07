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

//将LPCWSTR转换为LPCSTR，这个是一个字符串格式转换的函数定义
char* wtoc1(wchar_t* wText)
{
	DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, wText, -1, NULL, 0, NULL, FALSE);//把第五个参数设成NULL的到宽字符串的长度包括结尾符
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


int ZxingQRcode::recognize(CString image_name)//识别二维码的功能代码
{
	//DefQueue queue;
	//image_name =queue.popImage();
	//std::string image_name = "./CRCode/Image_R1.bmp";
	//image_name = popImage();
	ELOGI("start recognizing");
	LPCSTR s2;
	LPWSTR s1 = image_name.GetBuffer(0);
	s2 = wtoc1(s1);
	//截取s2字符串后6位
	std::string s3 = s2; //这里是为了将结果后面打印到窗口而取6位
	int length = strlen(s2);
	std::string s4,s5;
	s4 = s3.substr(length - 12, length); //截取图像名称的后12位，用于结果打印的识别图像名称
	s5 = s3.substr(length - 12, 8); //截取图像名称的倒数12位到倒数第4位，用于保存识别错误的图像名称

	//
	ELOGI("reading pic, pic name"<<s5);
	cv::Mat matSrc = cv::imread(s2, 1); //读取扫描的图像结果图，为整图，有多个二维码
	ELOGI("read pic, pic name" << s5<< " over");
	if (!matSrc.data) {
		ELOGI("read pic, pic name" << s5 << " error");
		//QMessageBox::warning(NULL, "warning", "read image error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		ELOGI("read pic, pic name" << s5 << " error end");
		return -1;
	}
	ELOGI("read pic successfull, pic name" << s5);
	//提取局部二维码
	int p_height = matSrc.rows;
	int p_width = matSrc.cols;
	int each_height = p_height / 7;  //获得局部二维码所在方块的高度
	int each_width = p_width / 5;   //获得局部二维码所在方块的宽度
	cv::Mat m_matSrc[35];            //定义一个局部图像数组来存放局部图像
	int k = 0;
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 5; j++) {
			ELOGI("getting small pic, no. "<<to_string(i)<<"-"<<to_string(j));
			m_matSrc[k] = matSrc(cv::Rect(each_width * j, each_height * i, each_width, each_height));
			//做一个循环，从整图中取每个二维码所在的局部图像，使用opencv::Rect（宽度起始像素点，高度起始像素点，宽度，高度）来获取局部图像
			k += 1;
		}
	}
	//查看局部提取效果
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
		cv::cvtColor(m_matSrc[k], matGray, CV_BGR2GRAY); //输入矩阵，输出矩阵，进行图像灰度化处理
		//对图像进行滤波处理，用了4种滤波方法对比效果，最后使用中值滤波
		int ii = 1;
		cv::Mat matBlur;
		//cv::blur(matGray, matBlur, cv::Size(ii, ii), cv::Point(-1, -1));
		//高斯滤波
		//cv::Mat matGaussian;
		//cv::GaussianBlur(matGray, matGaussian, cv::Size(ii, ii), 0.0, 0.0, 4);
		//中值滤波
		cv::Mat matMedian;
		cv::medianBlur(matGray, matMedian, ii + 2);
		//二值化滤波
		//cv::Mat matBilateral;
		//cv::bilateralFilter(matGray, matBilateral, ii, ii * 2, ii / 2);
		//保存滤波结果，对比滤波效果
		//cv::imwrite("E:/documents/casitworkspace/zxing-QRcode1/QRCode/gray.jpg", matGray);
		//cv::imwrite("E:/documents/casitworkspace/zxing-QRcode1/QRCode/matBlur.jpg", matBlur);
		//cv::imwrite("E:/documents/casitworkspace/zxing-QRcode1/QRCode/matGaussian.jpg", matGaussian);
		cv::imwrite("E:/documents/casitworkspace/zxing-QRcode1/QRCode/median.jpg", matMedian);
		//cv::imwrite("E:/documents/casitworkspace/zxing-QRcode1/QRCode/matBilateral.jpg", matBilateral);
		//将不同bitmap提取到一个标准接口中，以便于下一步获得greyscale luminance value
		zxing::Ref<zxing::LuminanceSource> source = MatSource::create(matMedian);
		int width = source->getWidth();
		int height = source->getHeight();
		fprintf(stderr, "image width: %d, height: %d\n", width, height);
		zxing::Ref<zxing::Reader> reader; //将图像的二维码解码为其编码的字符串
		reader.reset(new zxing::qrcode::QRCodeReader);
		try
		{
			//将Luminance data转化为1 bit 数据
//zxing::Ref<zxing::Binarizer> binarizer(new zxing::GlobalHistogramBinarizer(source));
			//将Luminance data转化为1 bit 数据
			zxing::Ref<zxing::Binarizer> binarizer(new zxing::HybridBinarizer(source));
			//用来表示1 bit data的 core bitmap
			zxing::Ref<zxing::BinaryBitmap> bitmap(new zxing::BinaryBitmap(binarizer));
			//将barcode的解码结果概括到一个图像中
			zxing::Ref<zxing::Result> result(reader->decode(bitmap, zxing::DecodeHints(zxing::DecodeHints::QR_CODE_HINT)));
			std::string str2 = result->getText()->getText();
			ELOGI("recognizing pic success,pic number is"<<to_string(k));
			//打印结果
			Final_text += str2;
			Final_text += "     ";
			if ((k+1) % 5==0 ) {
				Final_text += "\n";//如果循环参数k+1正好被5整除，就换行打印结果
			}
		}
		catch (zxing::ReaderException e) {
			//如果在识别二维码过程中出现异常报错，则保存报错识别的图片到badImagePath文件夹
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
			LPSTR ss1 = wtoc1(ss); //字符串文件格式转换
			cv::imwrite(ss1, m_matSrc[k]);
			Final_text += "Fault"; //识别错误，打印结果为Fault
			Final_text += "   ";
			if ((k + 1) % 5 == 0) {
				Final_text += "\n";//如果循环参数k+1正好被5整除，就换行打印结果
			}
		}
	}

	return 0;
}