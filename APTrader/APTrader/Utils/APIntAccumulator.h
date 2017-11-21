#pragma once
#include "../APTypes.h"

class APIntAccumulator
{
public:
	APIntAccumulator();
	~APIntAccumulator();

	UINT generateID();

private:
	UINT m_id;
};

