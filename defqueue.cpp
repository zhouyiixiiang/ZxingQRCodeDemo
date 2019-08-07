#include "stdafx.h"
#include "defqueue.h"
std::mutex mtx; //����һ������������֤�̶߳Զ��н�����ӡ����Ӳ�������
queue<CString> QueueOfImages; //����һ����������Image��name
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