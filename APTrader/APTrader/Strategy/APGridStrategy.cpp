#include "APGridStrategy.h"
#include "../APInstrumentQuotation.h"
#include "../APGlobalConfig.h"
#include "../APPositionCtrl.h"
#include <algorithm>
#include "../APQuotationDecision.h"
#include "../Utils/APJsonReader.h"
#include <iostream>
#include <sstream>  
#include "../Utils/APLog.h"

// ----
void buildEqualRatioArray(std::vector<double>& arr, double baseVal, double ratio, int count) {
	arr.clear();
	double accumVal = 1.0;
	for (int i = 0; i < count; i++) {
		double val = accumVal * baseVal;
		arr.push_back(val);
		accumVal *= ratio;
	}
}

void buildEqualDiffArray(std::vector<double>& arr, double baseVal, double diff, int count) {
	double val = baseVal;
	for (int i = 0; i < count; i++) {
		arr.push_back(val);
		val += diff;
	}
}
// ----


APGridStrategy::APGridStrategy()
{
	m_curIndex = 0;
	m_lastIndex = 0;
	

	/*m_closeOnly = false;*/
}

APGridStrategy::~APGridStrategy()
{
}

//APStrategy * APGridStrategy::create()
//{
//	return new APGridStrategy();
//}

void APGridStrategy::init(std::string strategyInfo)
{
	APStrategy::init(strategyInfo);

	APJsonReader jr;
	jr.initWithString(strategyInfo);
	std::string gridsInfo = jr.getObjValue("Grids");

	buildGrids(gridsInfo);

#ifdef _DEBUG
	printGrids();
#endif
}

void APGridStrategy::alert()
{
}

void APGridStrategy::update()
{
	if (!m_work) {
		return;
	}

	if (m_quotationDecision != NULL) {
		double curValue = m_quotationDecision->getValueReference();
		if (!m_quotationDecision->isValueRefValid(curValue)) {
			return;
		}

		goGrids(curValue);
	}
}


void APGridStrategy::buildGrids(std::string gridsInfo)
{
	APJsonReader jr;
	jr.initWithString(gridsInfo);
	std::string strDirection = jr.getStrValue("Direction");
	if (strDirection == "Buy") {
		m_direction = TD_Buy;
	}
	else if (strDirection == "Sell") {
		m_direction = TD_Sell;
	}

	m_indeterminateCeil = jr.getDoubleValue("Ceil");
	m_indeterminateFloor = jr.getDoubleValue("Floor");
	m_deltaPosition = jr.getIntValue("DeltaPosition");
	m_basePosition = jr.getIntValue("BasePosition");
	m_gridsCount = jr.getIntValue("GridsCount");
	m_longValue = jr.getDoubleValue("LongValue");
	m_shortValue = jr.getDoubleValue("ShortValue");

	m_gridType = (APGridType)jr.getIntValue("GridType");

	//resetGrids();
	switch (m_gridType)
	{
	case GT_Diff:
		buildEqualDiffGrids(gridsInfo);
		break;
	case GT_Ratio:
		buildEqualRatioGrids(gridsInfo);
		break;
	case GT_Fix:
		buildFixedDataGrids(gridsInfo);
		break;
	default:
		break;
	}

#ifdef _DEBUG
	printGrids();
#endif
}


void APGridStrategy::buildEqualRatioGrids(std::string info)
{
	APJsonReader jr;
	jr.initWithString(info);

	double ratio = jr.getDoubleValue("Ratio");
	std::vector<double> longVals;
	buildEqualRatioArray(longVals, m_longValue, ratio, m_gridsCount);
	std::vector<double> shortVals;
	buildEqualRatioArray(shortVals, m_shortValue, -ratio, m_gridsCount);

	if (m_direction == TD_Buy) {
		buildBuyGrids(longVals, shortVals);
	}
	else {
		buildSellGrids(longVals, shortVals);
	}
}

void APGridStrategy::buildEqualDiffGrids(std::string info)
{
	APJsonReader jr;
	jr.initWithString(info);

	double diff = jr.getDoubleValue("Diff");
	std::vector<double> longVals;
	buildEqualDiffArray(longVals, m_longValue, diff, m_gridsCount);
	std::vector<double> shortVals;
	buildEqualDiffArray(shortVals, m_shortValue, -diff, m_gridsCount);

	if (m_direction == TD_Buy) {
		buildBuyGrids(longVals, shortVals);
	}
	else {
		buildSellGrids(longVals, shortVals);
	}
}

void APGridStrategy::buildFixedDataGrids(std::string info)
{
	APJsonReader jr;
	jr.initWithString(info);

	std::vector<double> longVals;
	for (int i = 0; i < m_gridsCount; i++) {
		double val = jr.getArrayDoubleValue("LongValues", i);
		longVals.push_back(val);
	}
	std::vector<double> shortVals;
	for (int i = 0; i < m_gridsCount; i++) {
		double val = jr.getArrayDoubleValue("ShortValues", i);
		shortVals.push_back(val);
	}

	if (m_direction == TD_Buy) {
		buildBuyGrids(longVals, shortVals);
	}
	else {
		buildSellGrids(longVals, shortVals);
	}
}
