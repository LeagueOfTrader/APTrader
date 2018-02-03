#include "APStandardGridStrategy.h"
#include "../Utils/APJsonReader.h"

APStandardGridStrategy::APStandardGridStrategy()
{
	m_located = false;
}


APStandardGridStrategy::~APStandardGridStrategy()
{
}

APStrategy * APStandardGridStrategy::create()
{
	return new APStandardGridStrategy();
}

//void APStandardGridStrategy::buildGrids(std::string gridsInfo)
//{
//	APJsonReader jr;
//	jr.initWithString(gridsInfo);
//	std::string strDirection = jr.getStrValue("Direction");
//	if (strDirection == "Buy") {
//		m_direction = TD_Buy;
//	}
//	else if (strDirection == "Sell") {
//		m_direction = TD_Sell;
//	}
//
//	m_deltaPosition = jr.getIntValue("DeltaPosition");
//	m_basePosition = jr.getIntValue("BasePosition");
//	m_gridsCount = jr.getIntValue("GridsCount");
//	m_longValue = jr.getDoubleValue("LongValue");
//	m_shortValue = jr.getDoubleValue("ShortValue");
//
//	m_gridType = (APGridType)jr.getIntValue("GridType");
//}

void APStandardGridStrategy::buildBuyGrids(std::vector<double>& longValues, std::vector<double>& shortValues)
{
	m_grids.clear();
	m_grids.resize(2 * m_gridsCount);
	
	m_grids[m_gridsCount - 1].assign(m_shortValue, m_basePosition);
	for (int i = 1; i < m_gridsCount; i++) {
		int index = m_gridsCount - i - 1;
		m_grids[index].assign(shortValues[i], m_grids[index + 1].position + m_deltaPosition);
	}

	m_grids[m_gridsCount].assign(m_longValue, m_basePosition / 2);
	for (int i = 1; i < m_gridsCount; i++) {
		int index = m_gridsCount + i;
		m_grids[index].assign(longValues[i], m_grids[index - 1].position / 2);
	}
}

void APStandardGridStrategy::buildSellGrids(std::vector<double>& longValues, std::vector<double>& shortValues)
{
	m_grids.clear();
	m_grids.resize(2 * m_gridsCount);

	m_grids[m_gridsCount].assign(m_longValue, m_basePosition);
	for (int i = 1; i < m_gridsCount; i++) {
		int index = m_gridsCount + i;
		m_grids[index].assign(longValues[i], m_grids[index - 1].position + m_deltaPosition);
	}

	m_grids[m_gridsCount - 1].assign(m_shortValue, m_basePosition / 2);
	for (int i = 1; i < m_gridsCount; i++) {
		int index = m_gridsCount - i - 1;
		m_grids[index].assign(shortValues[i], m_grids[index + 1].position / 2);
	}
}

void APStandardGridStrategy::goGrids(double valueRef)
{
	//
}

void APStandardGridStrategy::locateGrids(double valueRef)
{
	// todo: init location of grids

	m_located = true;
}

#ifdef _DEBUG
void APStandardGridStrategy::printGrids()
{
}

#endif
