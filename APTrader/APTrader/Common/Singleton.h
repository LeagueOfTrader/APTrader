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
			m_instanceMutex.lock();
			if (NULL == m_instance) {
				m_instance = new T();
			}
			m_instanceMutex.unlock();
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
	static std::mutex m_instanceMutex;
};

template<class T> T* Singleton<T>::m_instance = NULL;
template<class T> std::mutex Singleton<T>::m_instanceMutex;