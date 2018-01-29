#include "APStandardGridStrategy.h"
#include "../Utils/APJsonReader.h"

//----
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

//----

APStandardGridStrategy::APStandardGridStrategy()
{
}


APStandardGridStrategy::~APStandardGridStrategy()
{
}

APStrategy * APStandardGridStrategy::create()
{
	return new APStandardGridStrategy();
}

void APStandardGridStrategy::buildGrids(std::string gridsInfo)
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
}

//void APStandardGridStrategy::resetGrids()
//{
//	m_openGrids.clear();
//	m_closeGrids.clear();
//
//	if (m_direction == TD_Buy) {
//		buildBuyGrids();
//	}
//	else if (m_direction == TD_Sell) {
//		buildSellGrids();
//	}
//}

void APStandardGridStrategy::buildBuyGrids(std::vector<double>& longValues, std::vector<double>& shortValues)
{
	APGridData data = {m_shortValue, m_basePosition};
	m_openGrids.push_back(data);
	for (int i = 1; i < m_gridsCount; i++) {
		APGridData gd = { shortValues[i], m_openGrids[i - 1].position + m_deltaPosition };
		m_openGrids.push_back(gd);
	}

	data = { m_longValue, m_deltaPosition };
	m_closeGrids.push_back(data);
	for (int i = 1; i < m_gridsCount; i++) {
		APGridData gd = { longValues[i], m_deltaPosition };
		m_closeGrids.push_back(gd);
	}
}

void APStandardGridStrategy::buildSellGrids(std::vector<double>& longValues, std::vector<double>& shortValues)
{
	APGridData data = { m_shortValue, m_deltaPosition };
	m_closeGrids.push_back(data);
	for (int i = 1; i < m_gridsCount; i++) {
		APGridData gd = { shortValues[i], m_deltaPosition };
		m_closeGrids.push_back(gd);
	}

	data = { m_longValue, m_basePosition };
	m_openGrids.push_back(data);
	for (int i = 1; i < m_gridsCount; i++) {
		APGridData gd = { longValues[i], m_openGrids[i - 1].position + m_deltaPosition };
		m_openGrids.push_back(gd);
	}
}

void APStandardGridStrategy::buildEqualRatioGrids(std::string info)
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

void APStandardGridStrategy::buildEqualDiffGrids(std::string info)
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

void APStandardGridStrategy::buildFixedDataGrids(std::string info)
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

