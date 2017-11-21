#include <stdlib.h>
#include <string.h>
#include <vector>

template<class T>
class CycleBuffer
{
public:
	CycleBuffer(UINT size, bool autoGrow = false)
	{
		m_bAutoGrow = autoGrow;
		reset();
		m_pBuf.resize(size);
		m_nBufSize = size;
	}

	virtual ~CycleBuffer()
	{
		if(m_pBuf != NULL)
		{
			FREE(m_pBuf);
		}
	}

	int grow(int nSize)
	{
		if (nSize <= 0)
		{
			return 0;
		}

		m_pBuf.resize(nSize + m_nBufSize);
		int bufSize = nSize + m_nBufSize;

		if (bufSize >= m_nAlertSize) {
			printf("CycleBuffer grow beyond alert size: " + bufSize);
		}

		if (m_nWriteIndex < m_nReadIndex) {
			for (int i = 0; i < m_nWriteIndex; i++) {
				m_pBuf.set((m_nBufSize + i) % bufSize, m_pBuf.get(i));
			}

			m_nWriteIndex = (m_nBufSize + m_nWriteIndex) % bufSize;
		}

		m_nBufSize = bufSize;

		return nSize;
	}

	//写到写列表的头部内存
	int writeToHead(T pBuf)
	{
		if (pBuf == null)
		{
			return 0;
		}

		if (space() == 0)
		{
			if (!m_bAutoGrow)
			{
				//return -1;
				readFromTail();
			}
			else if (grow(m_nGrowSize) <= 0)
			{
				return 0;
			}
		}

		if (m_nWriteIndex == m_nReadIndex)
		{
			return write(pBuf);
		}

		m_nReadIndex = (m_nReadIndex + m_nBufSize - 1) % m_nBufSize;
		m_pBuf.set(m_nReadIndex, pBuf);

		m_nDataSize++;

		return 1;
	}

	//写到写列表的尾部内存
	int write(T pBuf)
	{
		if (pBuf == null)
		{
			return 0;
		}

		//剩余空间不足
		if (space() == 0)
		{
			if (!m_bAutoGrow)
			{
				read();
			}
			else if (grow(m_nGrowSize) <= 0)
			{
				return 0;
			}
		}

		m_pBuf[m_nWriteIndex] = pBuf;
		m_nWriteIndex = (m_nWriteIndex + m_nBufSize + 1) % m_nBufSize;

		m_nDataSize++;

		return 1;
	}

	//读取读列表的尾部内存
	T readFromTail()
	{
		if (size() == 0) {
			return null;
		}

		m_nWriteIndex = (m_nWriteIndex + m_nBufSize - 1) % m_nBufSize;
		T data = m_pBuf.get(m_nWriteIndex);

		m_nDataSize--;

		return data;
	}

	//读取读列表的头部内存
	T read()
	{
		if (size() == 0) {
			return null;
		}

		T data = m_pBuf.get(m_nReadIndex);
		m_nReadIndex = (m_nReadIndex + 1) % m_nBufSize;

		m_nDataSize--;

		return data;
	}

	//读取数据但是不修改读索引
	T peek()
	{
		if (size() == 0) {
			return null;
		}

		T data = m_pBuf.get(m_nReadIndex);

		return data;
	}

	void reset()
	{
		m_nReadIndex = 0;
		m_nWriteIndex = 0;
		m_nDataSize = 0;
	}

	UINT size()
	{
		return m_nDataSize;
	}

	UINT capatity()
	{
		return m_nBufSize;
	}

	UINT space()
	{
		return m_nBufSize - m_nDataSize;
	}

protected:
	bool m_autoGrow;
	UINT m_nReadIndex;
	UINT m_nWriteIndex;
	UINT m_nDataSize;
	UINT m_nBufSize;
	std::vector<T> m_pBuf;
};
