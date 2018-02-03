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
	virtual void buildBuyGrids(std::vector<double>& longValues, std::vector<double>& shortValues);
	virtual void buildSellGrids(std::vector<double>& longValues, std::vector<double>& shortValues);
	virtual void goGrids(double valueRef);

	void locateGrids(double valueRef);

#ifdef _DEBUG
	virtual void printGrids();
#endif


private:
	std::vector<APGridData> m_grids;

	int m_openIndex;

	bool m_located;
};

