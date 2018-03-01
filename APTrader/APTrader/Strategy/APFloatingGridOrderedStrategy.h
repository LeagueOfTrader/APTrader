#pragma once
#include "APFloatingGridStrategy.h"
#include "../APPositionObserver.h"

class APFloatingGridOrderedStrategy : public APFloatingGridStrategy, public APPositionObserver
{
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

	// Í¨¹ý APPositionObserver ¼Ì³Ð
	virtual void onTradeOrdered(APASSETID instrumentID, APTradeType type, APTradeDirection direction) override;
	virtual void onTradeCanceled(APASSETID instrumentID, APTradeType type, APTradeDirection direction) override;
	virtual void onTradeFinished(APASSETID instrumentID, APTradeType type, APTradeDirection direction) override;
	virtual void onTradeFailed(APASSETID instrumentID, APTradeType type, APTradeDirection direction) override;
};