#pragma once
#include "../APTypes.h"

class APIntAccumulator
{
public:
	APIntAccumulator();
	~APIntAccumulator();

	UINT generateID();

	void setBase(UINT base);

private:
	UINT m_id;
};

