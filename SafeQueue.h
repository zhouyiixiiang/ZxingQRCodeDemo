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
	SafeQueue() {//构造函数

	}
	SafeQueue(SafeQueue& other) {//拷贝构造函数

	}
	~SafeQueue() {//析构函数

	}
	bool empty() {//队列是否为空
		std::unique_lock<std::mutex>lock(mtx);//互斥信号变量加锁，防止Q被更改
		return Q.empty();//即先加锁再查询队列是否为空
	}
	int size() {//队列有多长
		std::unique_lock<std::mutex>lock(mtx);//互斥信号变量加锁，防止Q被更改
		return Q.size();
	}
	//队列添加元素
	void enqueue(T& t) {
		std::unique_lock<std::mutex>lock(mtx);//队列加锁
		Q.push(t);
	}
	//队列取出元素
	bool dequeue(T& t) {
		std::unique_lock<std::mutex>lock(mtx);//队列加锁
		if (Q.empty()) {
			return false;
		}
		t = std::move(m_queue.front());//取出队首元素，返回队首元素值，并进行右值引用
		Q.pop();
		return true;
	}
};