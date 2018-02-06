#pragma once
#include "APFloatingGridStrategy.h"

class APFloatingGridOrderedStrategy : public APFloatingGridStrategy {
public:
	APFloatingGridOrderedStrategy();

	static APStrategy* create();

	virtual void init(std::string strategyInfo);

	void setOrdered(bool ordered);
	virtual void exit();

protected:
	virtual void locateGrids(double valueRef);
	virtual void enterGridInOpenWay(int gridIndex, APGridSectionType section);
	virtual void enterGridInCloseWay(int gridIndex, APGridSectionType section);

private:
	void openIfNotOverlapped(int index);
	void closeIfNotOverlapped(int index);
	bool closeIfAvailable(int index);

private:
	bool m_valid;
	bool m_ordered;

	std::vector<bool> m_gridsOverlapped;
};