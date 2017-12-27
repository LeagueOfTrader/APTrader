#pragma once
#include <stdio.h>
#include <mutex>

template<class T>
class Singleton
{
public:
	static T* getInstance()
	{
		if (NULL == m_instance)
		{
			m_mutex.lock();
			if (NULL == m_instance) {
				m_instance = new T();
			}
			m_mutex.unlock();
		}

		return m_instance;
	}

protected:
	Singleton() {}
	Singleton(const Singleton&){}
	Singleton& operator=(const Singleton&){}

	virtual ~Singleton()
	{
	}

	static void destory()
	{
		if (m_instance)
		{
			delete m_instance;
			m_pInstance = NULL;
		}
	}

protected:
	static T* m_instance;
	static std::mutex m_mutex;
};

template<class T> T* Singleton<T>::m_instance = NULL;
template<class T> std::mutex Singleton<T>::m_mutex;