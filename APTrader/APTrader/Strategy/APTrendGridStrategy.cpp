#include "APTrendGridStrategy.h"
#include "../Utils/APJsonReader.h"
#include "../APQuotationDecision.h"
#include "../APPositionCtrl.h"
#include "../Utils/APLog.h"
#include <sstream>

//----

APTrendGridStrategy::APTrendGridStrategy()
{
	m_openGrids.clear();
	m_closeGrids.clear();
}


APTrendGridStrategy::~APTrendGridStrategy()
{
}

APStrategy * APTrendGridStrategy::create()
{
	return new APTrendGridStrategy();
}

//void APTrendGridStrategy::update()
//{
//	if (!m_work) {
//		return;
//	}
//
//	if (m_quotationDecision != NULL) {
//		double curValue = m_quotationDecision->getValueReference();
//		if (!m_quotationDecision->isValueRefValid(curValue)) {
//			return;
//		}
//
//		m_curIndex = 0;
//		goTrendGrid(curValue);
//		goThroughGrids();
//		m_lastIndex = m_curIndex;
//	}
//}

//void APTrendGridStrategy::buildGrids(std::string gridsInfo)
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
//	m_indeterminateCeil = jr.getDoubleValue("Ceil");
//	m_indeterminateFloor = jr.getDoubleValue("Floor");
//	m_deltaPosition = jr.getIntValue("DeltaPosition");
//	m_basePosition = jr.getIntValue("BasePosition");
//	m_gridsCount = jr.getIntValue("GridsCount");
//	m_longValue = jr.getDoubleValue("LongValue");
//	m_shortValue = jr.getDoubleValue("ShortValue");
//
//	m_gridType = (APGridType)jr.getIntValue("GridType");
//
//	//resetGrids();
//	switch (m_gridType)
//	{
//	case GT_Diff:
//		buildEqualDiffGrids(gridsInfo);
//		break;
//	case GT_Ratio:
//		buildEqualRatioGrids(gridsInfo);
//		break;
//	case GT_Fix:
//		buildFixedDataGrids(gridsInfo);
//		break;
//	default:
//		break;
//	}
//
//#ifdef _DEBUG
//	printGrids();
//#endif
//}

//void APTrendGridStrategy::resetGrids()
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

void APTrendGridStrategy::buildBuyGrids(std::vector<double>& longValues, std::vector<double>& shortValues)
{
	APGridData data = { m_shortValue, m_basePosition };
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

void APTrendGridStrategy::buildSellGrids(std::vector<double>& longValues, std::vector<double>& shortValues)
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

void APTrendGridStrategy::goGrids(double valueRef)
{
	m_curIndex = 0;
	goTrendGrid(valueRef);
	goThroughGrids();
	m_lastIndex = m_curIndex;

}



void APTrendGridStrategy::goTrendGrid(double value) // , std::vector<APGridData>& openGrids, std::vector<APGridData>& closeGrids)
{
	if (m_positionCtrl == NULL) {
		return;
	}

	double price = 0.0;
	if (inCloseSection(value)) {
		if (m_positionCtrl->getAvailablePosition() == 0) {
			return;
		}

		int i = getCurTrendGridIndex(m_closeGrids, value, false);
		if (i < 0) {
			i = 0;
		}

		m_curIndex = i + 1;

		if (m_curIndex > m_lastIndex) {
			long volume = std::min(m_positionCtrl->getAvailablePosition(), m_closeGrids[i].position);
			if (i == m_closeGrids.size() - 1) {
				volume = m_positionCtrl->getAvailablePosition();
			}
			//m_positionCtrl->closePosition(m_direction, volume);
			close(volume);
		}
	}
	else if (inOpenSection(value)) {
		if (m_closeOnly) {
			return;
		}

		int i = getCurTrendGridIndex(m_openGrids, value, true);

		if (i >= m_openGrids.size()) {
			//m_positionCtrl->openFullPosition(m_direction);
			return;
		}

		m_curIndex = -(i + 1);

		long amount = m_positionCtrl->getForeseeableHoldPosition();
		if (amount < m_openGrids[i].position) {
			long volume = m_openGrids[i].position - amount;
			//m_positionCtrl->openPosition(m_direction, volume);
			open(volume);
		}
	}
}


void APTrendGridStrategy::goThroughGrids()
{
	if (m_curIndex == m_lastIndex || m_lastIndex == 0) {
		return;
	}

	APTradeType type = TT_Num;
	if (m_lastIndex < 0) {
		type = TT_Open;
	}
	else {
		type = TT_Close;
	}

	int gridIndex = m_lastIndex;

	//bool isGoingPoles = true;
	//if (abs(m_curIndex) > abs(m_lastIndex)) {
	//	isGoingPoles = false;
	//}

	double refPrice = 0.0;
	if (gridIndex > 0) {
		refPrice = m_closeGrids[gridIndex - 1].valueRef;
	}
	else {
		refPrice = m_openGrids[-gridIndex - 1].valueRef;
	}

	//if (m_positionCtrl != NULL) {
	//	m_positionCtrl->cancelTrade(type, refPrice, m_direction);
	//}
}

bool APTrendGridStrategy::inOpenSection(double value)
{
	if (m_direction == TD_Buy) {
		return value < m_indeterminateFloor;
	}
	else if (m_direction == TD_Sell) {
		return value > m_indeterminateCeil;
	}

	return false;
}

bool APTrendGridStrategy::inCloseSection(double value)
{
	if (m_direction == TD_Buy) {
		return value >= m_indeterminateCeil;
	}
	else if (m_direction == TD_Sell) {
		return value <= m_indeterminateFloor;
	}

	return false;
}

int APTrendGridStrategy::getGridIndex(std::vector<APGridData>& grids, double value, bool reverse)
{
	int i = 0;

	if (!reverse) {
		for (i = 0; i < grids.size(); i++) {
			if (grids[i].valueRef - value > 0.0f
				//&& curPrice - grids[i].price < APGlobalConfig::getInstance()->getNearByPrice()
				) {
				break;
			}
		}

		if (i > 0) {
			i--;
		}
	}
	else {
		for (i = grids.size() - 1; i >= 0; i--) {
			if (grids[i].valueRef - value > 0.0f) {
				break;
			}
		}

		if (i < grids.size() - 1) {
			i++;
		}
	}

	return i;
}

int APTrendGridStrategy::getCurTrendGridIndex(std::vector<APGridData>& grids, double value, bool open)
{
	bool reverse = true;
	if ((m_direction == TD_Buy && open == false)
		|| (m_direction == TD_Sell && open == true)) {
		reverse = false;
	}

	return getGridIndex(grids, value, reverse);
}

#ifdef _DEBUG
void APTrendGridStrategy::printGrids()
{
	std::cout << "Open grids" << std::endl;

	for (int i = 0; i < m_openGrids.size(); i++) {
		APGridData& data = m_openGrids[i];
		std::stringstream ss;
		ss << "[" << i + 1 << "], quotation: " << data.valueRef << ", position: " << data.position << ". ";
		APLogger->log(ss.str().c_str());
		//std::cout << "[" << i + 1 << "], quotation: " << data.valueRef << ", price: " << data.price << ", position: " << data.position << ". " << std::endl;
	}

	std::cout << "Close grids" << std::endl;

	for (int i = 0; i < m_closeGrids.size(); i++) {
		APGridData& data = m_closeGrids[i];
		std::stringstream ss;
		ss << "[" << i + 1 << "], quotation: " << data.valueRef << ", position: " << data.position << ". ";
		APLogger->log(ss.str().c_str());
		//std::cout << "[" << i + 1 << "], quotation: " << data.valueRef << ", price: " << data.price << ", position: " << data.position << ". " << std::endl;
	}
}
#endif