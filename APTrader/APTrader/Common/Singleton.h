#pragma once
#include <stdio.h>

template<class T>
class Singleton
{
public:
	static T* getInstance()
	{
		if (NULL == m_pInstance)
		{
			m_pInstance = new T();
		}

		return m_pInstance;
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
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

private:
	static T* m_pInstance;
};

template<class T> T* Singleton<T>::m_pInstance = NULL;