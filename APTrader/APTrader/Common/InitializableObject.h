#pragma once

class InitializableObject {
public:
	InitializableObject() {
		m_inited = false;
	}

	bool inited() {
		return m_inited;
	}

protected:
	void setInited() {
		m_inited = true;
	}

protected:
	bool m_inited;
};