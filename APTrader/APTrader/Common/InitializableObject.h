#pragma once

class InitializableObject {
public:
	InitializableObject() {
		m_inited = false;
		m_exited = false;
	}

	virtual bool inited() {
		return m_inited;
	}
	
	bool exited() {
		return m_exited;
	}

	virtual void init() = 0;
	
	virtual void exit(){
		m_exited = true;
	}

protected:
	void setInited() {
		m_inited = true;
	}

protected:
	bool m_inited;
	bool m_exited;
};