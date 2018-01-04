#pragma once
#include "APGridStrategy.h"

class APStandardGridStrategy : public APGridStrategy
{
public:
	APStandardGridStrategy();
	~APStandardGridStrategy();

	static APStrategy* create();

	virtual void buildGrids(std::string gridsInfo);
	virtual void resetGrids();

private:
	//void buildGrids();
	void buildBuyGrids();
	void buildSellGrids();

private:
	double m_gridPercent;
	double m_pricePercent;
	long m_gridPosition;
	int m_gridsCount;
	double m_longPrice;
	double m_shortPrice;
	long m_basePosition;
};

