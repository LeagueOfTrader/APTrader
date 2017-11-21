#include "APGridStrategy.h"
#include "../APCommodityQuotation.h"
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
	if (m_quotationDecision != NULL) {
		double curValue = m_quotationDecision->getValueReference();

		m_curIndex = 0;
		goTrendGrid(curValue);
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
		if (m_positionCtrl->getTradablePosition() == 0) {
			return;
		}

		int i = getCurTrendGridIndex(m_closeGrids, value, false);
		if (i < 0) {
			i = 0;
		}

		m_curIndex = i + 1;

		if (m_curIndex > m_lastIndex) {
			long amount = std::min(m_positionCtrl->getTradablePosition(), m_closeGrids[i].position);
			if (i == m_closeGrids.size() - 1) {
				amount = m_positionCtrl->getTradablePosition();
			}
			close(m_trend, m_closeGrids[i].price, amount);
		}
	}
	else if (inOpenSection(value)) {
		int i = getCurTrendGridIndex(m_openGrids, value, true);

		if (i >= m_openGrids.size()) {
			openAll(m_trend, m_openGrids[m_openGrids.size() - 1].price);
			return;
		}

		m_curIndex = -(i + 1);

		if (m_positionCtrl->getCurPosition() + m_positionCtrl->getOpenOrderedPosition() < m_openGrids[i].position) {
			long amount = m_openGrids[i].position - (m_positionCtrl->getCurPosition() + m_positionCtrl->getOpenOrderedPosition());
			open(m_trend, m_openGrids[i].price, amount);
		}
	}
}

bool APGridStrategy::inCloseSection(double value)
{
	if (m_trend == TT_Long) {
		return value >= m_waitCeil;
	}
	else if (m_trend == TT_Short) {
		return value <= m_waitFloor;
	}

	return false;
}

void APGridStrategy::goThroughGrids()
{
	if (m_curIndex == m_lastIndex) {
		return;
	}

	int gridIndex = m_curIndex;

	APTradeType type = TDT_Num;
	if (m_curIndex < 0) {
		// in open section
		type = TDT_Open;
	}
	else if(m_curIndex > 0){
		// in close section
		type = TDT_Close;
	}
	else {
		// in waiting section
		if(m_lastIndex < 0){
			type = TDT_Open;
		}
		else {
			type = TDT_Close;
		}

		gridIndex = m_lastIndex;
	}

	bool isGoingPoles = true;
	if (abs(m_curIndex) > abs(m_lastIndex)) {
		isGoingPoles = false;
	}


	if (m_positionCtrl != NULL) {
		m_positionCtrl->cancelTrade(type, , );
	}
}

bool APGridStrategy::inOpenSection(double value)
{
	if (m_trend == TT_Long) {
		return value < m_waitFloor;
	}
	else if (m_trend == TT_Short) {
		return value > m_waitCeil;
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
	if ((m_trend == TT_Long && open == false)
		|| (m_trend == TT_Short && open == true)) {
		reverse = true;
	}

	return getGridIndex(grids, value, reverse);
}



