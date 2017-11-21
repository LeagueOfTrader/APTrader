#pragma once
#include <vector>

template<class T>
class ParentChildrenHierarchy
{	
public:
	ParentChildrenHierarchy<T>::ParentChildrenHierarchy() {
		m_parent = NULL;
		m_children.empty();
	}

	T* getParent() {
		return m_parent;
	}

	void attach(T* child) {
		if (child == NULL) {
			return;
		}

		if (child->getParent() != NULL) {
			child->detachFromParent();
		}

		m_children.push_back(child);
		child->m_parent = (T*)this;
	}

	void detach(T* child) {
		if (child == NULL) {
			return;
		}

		if (child->getParent() == NULL || !equals(child->getParent())) {
			return;
		}

		std::vector<T*>::iterator it;
		for (it = m_children.begin(); it != m_children.end(); it++) {
			if(child->equals(*it)){
				it = m_children.erase(it);
				return;
			}
		}
	}

	inline void detachFromParent() {
		if (m_parent == NULL) {
			return;
		}

		m_parent->detach((T*)this);
	}

	virtual bool equals(T* obj) = 0;

protected:
	T* m_parent;
	std::vector<T*> m_children;
};

