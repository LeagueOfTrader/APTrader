#pragma once
#include "APGridStrategy.h"


class APTrendGridStrategy : public APGridStrategy
{
public:
	APTrendGridStrategy();
	~APTrendGridStrategy();

	static APStrategy* create();

	//virtual void update();

protected:
	//virtual void buildGrids(std::string gridsInfo);
	//virtual void resetGrids();

	int getGridIndex(std::vector<APGridData>& grids, double value, bool reverse = false);

	int getCurTrendGridIndex(std::vector<APGridData>& grids, double value, bool open);

	void goTrendGrid(double curPrice);// , std::vector<APGridData>& openGrids, std::vector<APGridData>& closeGrids);

	bool inOpenSection(double value);
	bool inCloseSection(double value);

	void goThroughGrids();
	
	virtual void buildBuyGrids(std::vector<double>& longValues, std::vector<double>& shortValues);
	virtual void buildSellGrids(std::vector<double>& longValues, std::vector<double>& shortValues);

	virtual void goGrids(double valueRef);

#ifdef _DEBUG
	virtual void printGrids();
#endif
	

private:

	std::vector<APGridData> m_openGrids;
	std::vector<APGridData> m_closeGrids;

};


