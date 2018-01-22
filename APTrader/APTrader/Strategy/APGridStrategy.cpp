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

APGridStrategy::APGridStrategy()
{
	m_curIndex = 0;
	m_lastIndex = 0;
	m_openGrids.clear();
	m_closeGrids.clear();

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

#if _DEBUG
	printGrids();
#endif
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

		m_curIndex = 0;
		goTrendGrid(curValue);
		goThroughGrids();
		m_lastIndex = m_curIndex;
	}
}

void APGridStrategy::alert()
{
}

void APGridStrategy::printGrids()
{
	std::cout << "Open grids" << std::endl;
	
	for (int i = 0; i < m_openGrids.size(); i++) {
		APGridData& data = m_openGrids[i];
		std::stringstream ss;
		ss << "[" << i + 1 << "], quotation: " << data.valueRef << ", price: " << data.price << ", position: " << data.position << ". ";
		APLogger->log(ss.str().c_str());
		//std::cout << "[" << i + 1 << "], quotation: " << data.valueRef << ", price: " << data.price << ", position: " << data.position << ". " << std::endl;
	}

	std::cout << "Close grids" << std::endl;

	for (int i = 0; i < m_closeGrids.size(); i++) {
		APGridData& data = m_closeGrids[i];
		std::stringstream ss;
		ss << "[" << i + 1 << "], quotation: " << data.valueRef << ", price: " << data.price << ", position: " << data.position << ". ";
		APLogger->log(ss.str().c_str());
		//std::cout << "[" << i + 1 << "], quotation: " << data.valueRef << ", price: " << data.price << ", position: " << data.position << ". " << std::endl;
	}
}

void APGridStrategy::goTrendGrid(double value) // , std::vector<APGridData>& openGrids, std::vector<APGridData>& closeGrids)
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
			m_positionCtrl->closePosition(m_direction, m_closeGrids[i].price, volume);
		}
	}
	else if (inOpenSection(value)) {
		if (m_closeOnly) {
			return;
		}

		int i = getCurTrendGridIndex(m_openGrids, value, true);

		if (i >= m_openGrids.size()) {
			m_positionCtrl->openFullPosition(m_direction, m_openGrids[m_openGrids.size() - 1].price);
			return;
		}

		m_curIndex = -(i + 1);

		long amount = m_positionCtrl->getForeseeableHoldPosition();
		if (amount < m_openGrids[i].position) {
			long volume = m_openGrids[i].position - amount;
			m_positionCtrl->openPosition(m_direction, m_openGrids[i].price, volume);
		}
	}
}


void APGridStrategy::goThroughGrids()
{
	if (m_curIndex == m_lastIndex || m_lastIndex == 0) {
		return;
	}	

	APTradeType type = TT_Num;
	if(m_lastIndex < 0){
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
		refPrice = m_closeGrids[gridIndex - 1].price;
	}
	else {
		refPrice = m_openGrids[-gridIndex - 1].price;
	}

	if (m_positionCtrl != NULL) {
		m_positionCtrl->cancelTrade(type, refPrice, m_direction);
	}
}

bool APGridStrategy::inOpenSection(double value)
{
	if (m_direction == TD_Buy) {
		return value < m_waitFloor;
	}
	else if (m_direction == TD_Sell) {
		return value > m_waitCeil;
	}

	return false;
}

bool APGridStrategy::inCloseSection(double value)
{
	if (m_direction == TD_Buy) {
		return value >= m_waitCeil;
	}
	else if (m_direction == TD_Sell) {
		return value <= m_waitFloor;
	}

	return false;
}

int APGridStrategy::getGridIndex(std::vector<APGridData>& grids, double value, bool reverse)
{
	int i = 0;

	if (!reverse) {
		for (i = 0; i < grids.size(); i++) {
			if (value - grids[i].valueRef > 0.0f
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
			if (grids[i].valueRef - value < APGlobalConfig::getInstance()->getNearByPrice()) {
				break;
			}
		}
	}	

	return i;
}

int APGridStrategy::getCurTrendGridIndex(std::vector<APGridData>& grids, double value, bool open)
{
	bool reverse = false;
	if ((m_direction == TD_Buy && open == false)
		|| (m_direction == TD_Sell && open == true)) {
		reverse = true;
	}

	return getGridIndex(grids, value, reverse);
}



