#pragma once
#include <string>
#include <fstream>
# include <afx.h>
#include <queue>
#include <mutex>
using namespace std;
extern queue<CString> QueueOfImages; //����һ����������Image��name
//extern std::mutex mtx; //����һ������������֤�̶߳Զ��н�����ӡ����Ӳ�������
//��ӡ����Ӻ��ж϶����Ƿ�Ϊ�գ���Ҫ�ڳ���ĸ������ֵ��ã������extern����Ϊȫ������
extern BOOL IsEmptyQueue();
extern void pushImage(CString ImageName);
extern CString popImage();


//�������-----------------------------------------------------------

//extern queue<CString> QueueOfImages; //����һ����������Image��name
//extern std::mutex mtx; //����һ������������֤�̶߳Զ��н�����ӡ����Ӳ�������
//
//extern BOOL IsEmptyQueue() {
//	unique_lock<mutex> lock(mtx);
//	return QueueOfImages.empty();
//}
//extern void pushImage(CString ImageName)
//{
//	mtx.lock();
//	QueueOfImages.push(ImageName);
//	mtx.unlock();
//}
//extern CString popImage()
//{
//	while (IsEmptyQueue())
//	{
//		_sleep(1 * 1000);
//	}
//	mtx.lock();
//	CString ImageName;
//	ImageName = QueueOfImages.front();
//	QueueOfImages.pop();
//	mtx.unlock();
//	return ImageName;
//}

//class DefQueue {
//private:
//	mutex mtx; //����һ������������֤�̶߳Զ��н�����ӡ����Ӳ�������
//public:
//	DefQueue(){}
//	~DefQueue(){}
//	BOOL IsEmptyQueue() {
//		unique_lock<mutex> lock(mtx);
//		return QueueOfImages.empty();
//	}
//	void pushImage(CString ImageName)
//	{
//		mtx.lock();
//		QueueOfImages.push(ImageName);
//		mtx.unlock();
//	}
//	CString popImage()
//	{
//		while (IsEmptyQueue())
//		{
//			_sleep(1 * 1000);
//		}
//		mtx.lock();
//		CString ImageName;
//		ImageName = QueueOfImages.front();
//		QueueOfImages.pop();
//		mtx.unlock();
//		return ImageName;
//	}
//};