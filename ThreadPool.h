#pragma once
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include "SafeQueue.h"
class ThreadPool {
private:
	class ThreadWorker {//�����̹߳�����
	private:
		int working_id;//����id
		ThreadPool* working_pool;//�����̳߳�
	public:
		//���캯��
		ThreadWorker(ThreadPool* pool, const int id) :working_pool(pool), working_id(id) {}
		//���ز���
		void operator()() {
			std::function<void()> func;//�������������func
			bool dequeued;//�Ƿ�����ȡ��������Ԫ��
			//�ж��̳߳��Ƿ�رգ�û�йرգ�ѭ����ȡ
			while (!working_pool->working_shutdown) {
				{
					//Ϊ�̻߳��������������ʹ����̵߳����ߺͻ���
					std::unique_lock<std::mutex>lock(working_pool->working_conditional_mutex);
					//�������Ϊ�գ�������ǰ�߳�
					if (working_pool->working_queue.empty()) {
						working_pool->working_conditional_lock.wait(lock);//�ȴ���������֪ͨ�����߳�
					}
					//ȡ���������Ԫ��
					dequeued = working_pool->working_queue.dequeue(func);
				}
				//���ȡ���ɹ���ִ�й�������
				if (dequeued) {
					func();
				}
			}
		};
	};
	//������threadworker�������Ǽ���pool
	bool working_shutdown;//�߳��Ƿ�ر�
	SafeQueue<std::function<void()>>working_queue;//ִ�к�����ȫ���У����������
	std::vector<std::thread>working_threads;//�����̶߳���
	std::mutex working_conditional_mutex; //�߳��������������
	std::condition_variable working_conditional_lock;//�̻߳����������߳̿��Դ������߻��߻���״̬
public:
	//�̳߳ع��캯��
	ThreadPool(const int n_threads) :working_threads(std::vector<std::thread>(n_threads)), working_shutdown(false) {}
	ThreadPool(const ThreadPool&) = delete; //�������캯��������ȡ��Ĭ�ϸ��๹�캯��
	ThreadPool(ThreadPool&&) = delete; // �������캯����������ֵ���� 
	ThreadPool& operator=(const ThreadPool&) = delete; // ��ֵ���� 
	ThreadPool& operator=(ThreadPool&&) = delete; //��ֵ���� 
	// Inits thread pool 
	void init() {
		for (int i = 0; i < working_threads.size(); ++i) {
			working_threads[i] = std::thread(ThreadWorker(this, i));//���乤���߳� 
		}
	}

	// Waits until threads finish their current task and shutdowns the pool 
	void shutdown() {
		working_shutdown = true;
		working_conditional_lock.notify_all(); //֪ͨ �������й����߳�     
		for (int i = 0; i < working_threads.size(); ++i) {
			if (working_threads[i].joinable()) { //�ж��߳��Ƿ����ڵȴ� 
				working_threads[i].join();  //���̼߳���ȴ����� 
			}
		}
	}

	// Submit a function to be executed asynchronously by the pool   //�̵߳���Ҫ����������ʹ���˺��÷������ͣ��Զ��жϺ�������ֵ 
	template<typename F, typename...Args>
	auto submit(F&& f, Args&& ... args) -> std::future<decltype(f(args...))> {
		// Create a function with bounded parameters ready to execute     //  
		std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);//���Ӻ����Ͳ������壬���⺯������,��������ֵ���� 
		// Encapsulate it into a shared ptr in order to be able to copy construct // assign      //��װ��ȡ������󣬷�������һ���̲߳鿴��� 
		auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

		// Wrap packaged task into void function     //����������ʽ������һ���������� 
		std::function<void()> wrapper_func = [task_ptr]() {
			(*task_ptr)();
		};

		// ����ͨ�ð�ȫ�����������ѹ�밲ȫ���� 
		m_queue.enqueue(wrapper_func);

		// ����һ���ȴ��е��߳� 
		m_conditional_lock.notify_one();

		// ������ǰע�������ָ�� 
		return task_ptr->get_future();
	}
};