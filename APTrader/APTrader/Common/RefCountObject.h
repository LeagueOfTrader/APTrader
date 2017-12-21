#pragma once

class RefCountObject {
public:
	RefCountObject() {
		m_refCount = 1;
	};

	void incRef() {
		m_refCount++;
	}

	void decRef() {
		m_refCount--;
	}

	int getRefCount() {
		return m_refCount;
	}

protected:
	int m_refCount;
};