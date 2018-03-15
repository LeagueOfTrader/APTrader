#pragma once
#include "../APTypes.h"

class IdentifiedObject {
public:
	UINT getID() {
		return m_id;
	}

	void setID(UINT id) {
		m_id = id;
	}

protected:
	UINT m_id;
};