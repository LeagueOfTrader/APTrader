#pragma once
#include "APGridStrategy.h"

enum APGridType {
	GT_Fix = 1,
	GT_Ratio,
	GT_Diff,
	GT_Num
};

class APStandardGridStrategy : public APGridStrategy
{
public:
	APStandardGridStrategy();
	~APStandardGridStrategy();

	static APStrategy* create();

	virtual void buildGrids(std::string gridsInfo);
	//virtual void resetGrids();

private:
	//void buildGrids();
	void buildBuyGrids(std::vector<double>& longValues, std::vector<double>& shortValues);
	void buildSellGrids(std::vector<double>& longValues, std::vector<double>& shortValues);

	void buildEqualRatioGrids(std::string info);
	void buildEqualDiffGrids(std::string info);
	void buildFixedDataGrids(std::string info);

private:
	//double m_gridPercent;
	//double m_pricePercent;
	long m_deltaPosition;
	int m_gridsCount;
	double m_longValue;
	double m_shortValue;
	long m_basePosition;

	APGridType m_gridType;
};

