#pragma once
#include "APFloatingGridStrategy.h"

class APFloatingGridOrderedStrategy : public APFloatingGridStrategy {
public:
	APFloatingGridOrderedStrategy();

	static APStrategy* create();

	virtual void init(std::string strategyInfo);

	//void setOrdered(bool ordered);
	virtual void exit();

protected:
	virtual void locateGrids(double valueRef);
	virtual void enterGridInOpenWay(int gridIndex, APGridSectionType section);
	virtual void enterGridInCloseWay(int gridIndex, APGridSectionType section);

	virtual void goGrids(double valueRef);

private:
	void openIfNotOverlapped(int index);
	void closeIfNotOverlapped(int index);
	bool closeIfAvailable(int index);

	void setAdjacentIndex(APTradeDirection direction);
	bool isIndexValid(int index);
	int getReferIndex(int index, APTradeType tradeType);

	void enterGrid(int gridIndex);
	void tryOrderAdjacentGrid();

private:
	bool m_valid;
	//bool m_ordered;
	int m_prevIndex;
	int m_nextIndex;

	std::vector<bool> m_gridsOverlapped;
};