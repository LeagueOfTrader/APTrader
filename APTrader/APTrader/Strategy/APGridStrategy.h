#pragma once
#include "../APStrategy.h"
#include "../APDef.h"
#include <vector>

class APPositionCtrl;
class APInstrumentQuotation;

struct APGridData {
	double valueRef;
	//double price;
	long position;

	//APGridData(double theValueRef, double thePrice, long thePosition) {
	//	valueRef = theValueRef;
	//	price = thePrice;
	//	position = thePosition;
	//}
};

class APGridStrategy : public APStrategy 
{
public:
	APGridStrategy();
	~APGridStrategy();

	virtual void init(std::string strategyInfo);
	virtual void update();
	virtual void alert();

	//test function
	void printGrids();

protected:
	virtual void buildGrids(std::string gridsInfo) = 0;
	virtual void resetGrids() = 0;

	int getGridIndex(std::vector<APGridData>& grids, double value, bool reverse = false);

	int getCurTrendGridIndex(std::vector<APGridData>& grids, double value, bool open);

	void goTrendGrid(double curPrice);// , std::vector<APGridData>& openGrids, std::vector<APGridData>& closeGrids);

	bool inOpenSection(double value);
	bool inCloseSection(double value);

	void goThroughGrids();

protected:
	APTradeDirection m_direction;
	
	//APASSETID m_instrumentID;

	std::vector<APGridData> m_openGrids;
	std::vector<APGridData> m_closeGrids;

	double m_waitFloor;
	double m_waitCeil;

	int m_curIndex;
	int m_lastIndex;

	//bool m_closeOnly;
};