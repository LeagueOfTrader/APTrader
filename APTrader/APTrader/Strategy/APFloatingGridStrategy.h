#pragma once

#include "APStandardGridStrategy.h"

class APFloatingGridStrategy : public APStandardGridStrategy
{
public:
	static APStrategy* create();

protected:
	virtual void enterGridInOpenWay(int gridIndex, APGridSectionType section);
	virtual void enterGridInCloseWay(int gridIndex, APGridSectionType section);
};