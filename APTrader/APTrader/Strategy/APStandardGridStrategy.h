#pragma once
#include "APGridStrategy.h"


class APStandardGridStrategy : public APGridStrategy
{
public:
	APStandardGridStrategy();
	~APStandardGridStrategy();

	static APStrategy* create();

protected:
	//virtual void buildGrids(std::string gridsInfo);
	//virtual void resetGrids();
	void buildBuyGrids(std::vector<double>& longValues, std::vector<double>& shortValues);
	void buildSellGrids(std::vector<double>& longValues, std::vector<double>& shortValues);
	virtual void goGrids(double valueRef);

	virtual void locateGrids(double valueRef);

#ifdef _DEBUG
	virtual void printGrids();
#endif

	int getGridIndex(double curValue);// , bool reversed = false);
	//bool inOpenSection(int curIndex);
	APGridSectionType getSection(double valueRef);
	//void goBuyGrids(double valueRef);
	//void goSellGrids(double valueRef);
	virtual void enterGridInOpenWay(int gridIndex, APGridSectionType section);
	virtual void enterGridInCloseWay(int gridIndex, APGridSectionType section);

protected:
	std::vector<APGridData> m_grids;

	int m_openIndex;

	bool m_located;
	//int m_prevIndex;
	//int m_nextIndex;
	double m_lastValue;
	bool m_ignoreSection;
};

