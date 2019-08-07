#pragma once
#include <string>
#include <fstream>
# include <afx.h>
#include <queue>
#include <mutex>
using namespace std;
extern queue<CString> QueueOfImages; //定义一个队列来放Image的name
//extern std::mutex mtx; //定义一个互斥锁来保证线程对队列进行入队、出队操作加锁
//入队、出队和判断队列是否为空，需要在程序的各个部分调用，因此用extern声明为全局向量
extern BOOL IsEmptyQueue();
extern void pushImage(CString ImageName);
extern CString popImage();


//定义队列-----------------------------------------------------------

//extern queue<CString> QueueOfImages; //定义一个队列来放Image的name
//extern std::mutex mtx; //定义一个互斥锁来保证线程对队列进行入队、出队操作加锁
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
//	mutex mtx; //定义一个互斥锁来保证线程对队列进行入队、出队操作加锁
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