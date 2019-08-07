#include "stdafx.h"
#include "defqueue.h"
std::mutex mtx; //定义一个互斥锁来保证线程对队列进行入队、出队操作加锁
queue<CString> QueueOfImages; //定义一个队列来放Image的name
BOOL IsEmptyQueue() {
	unique_lock<mutex> lock(mtx);
	return QueueOfImages.empty();
}
void pushImage(CString ImageName)
{
	mtx.lock();
	QueueOfImages.push(ImageName);
	mtx.unlock();
}
CString popImage()
{
	mtx.lock();
	CString ImageName;
	ImageName = QueueOfImages.front();
	QueueOfImages.pop();
	mtx.unlock();
	return ImageName;
}