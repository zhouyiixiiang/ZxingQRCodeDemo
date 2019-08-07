#pragma once
#include <mutex>
#include <queue>
#include<iostream>
template <typename T>

class SafeQueue {
private:
	std::queque<T> Q;
	std::mutex mtx;
public:
	SafeQueue() {//���캯��

	}
	SafeQueue(SafeQueue& other) {//�������캯��

	}
	~SafeQueue() {//��������

	}
	bool empty() {//�����Ƿ�Ϊ��
		std::unique_lock<std::mutex>lock(mtx);//�����źű�����������ֹQ������
		return Q.empty();//���ȼ����ٲ�ѯ�����Ƿ�Ϊ��
	}
	int size() {//�����ж೤
		std::unique_lock<std::mutex>lock(mtx);//�����źű�����������ֹQ������
		return Q.size();
	}
	//�������Ԫ��
	void enqueue(T& t) {
		std::unique_lock<std::mutex>lock(mtx);//���м���
		Q.push(t);
	}
	//����ȡ��Ԫ��
	bool dequeue(T& t) {
		std::unique_lock<std::mutex>lock(mtx);//���м���
		if (Q.empty()) {
			return false;
		}
		t = std::move(m_queue.front());//ȡ������Ԫ�أ����ض���Ԫ��ֵ����������ֵ����
		Q.pop();
		return true;
	}
};