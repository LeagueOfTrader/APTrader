#pragma once
#include "../APStrategy.h"
#include "../APDef.h"
#include <vector>

class APPositionCtrl;
class APInstrumentQuotation;

struct APGridData {
	double valueRef;
	long position;

	void assign(double val, long vol) {
		valueRef = val;
		position = vol;
	}
};

enum APGridType {
	GT_Fix = 1,
	GT_Ratio,
	GT_Diff,
	GT_Num
};

class APGridStrategy : public APStrategy 
{
public:
	APGridStrategy();
	~APGridStrategy();

	virtual void init(std::string strategyInfo);
	virtual void update();
	virtual void alert();

protected:
	virtual void buildGrids(std::string gridsInfo);

	//void buildEqualRatioArray(std::vector<double>& arr, double baseVal, double ratio, int count);
	//void buildEqualDiffArray(std::vector<double>& arr, double baseVal, double diff, int count);

	void buildEqualRatioGrids(std::string info);
	void buildEqualDiffGrids(std::string info);
	void buildFixedDataGrids(std::string info);

	virtual void buildBuyGrids(std::vector<double>& longValues, std::vector<double>& shortValues) = 0;
	virtual void buildSellGrids(std::vector<double>& longValues, std::vector<double>& shortValues) = 0;

	virtual void goGrids(double valueRef) = 0;

#ifdef _DEBUG
	virtual void printGrids() = 0;
#endif

protected:
	APTradeDirection m_direction;
	APGridType m_gridType;

	double m_longValue;
	double m_shortValue;

	long m_basePosition;
	long m_deltaPosition;

	int m_gridsCount;

	int m_curIndex;
	int m_lastIndex;


	double m_indeterminateFloor;
	double m_indeterminateCeil;
	//bool m_closeOnly;
};