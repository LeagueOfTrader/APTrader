#include "APFloatingGridOrderedStrategy.h"
#include "../Utils/APJsonReader.h"
#include "../QuotationDecision/APSingleQuotationDecision.h"
#include "../APPositionCtrl.h"
#include "../Utils/APLog.h"
#include "../Futures/APFuturesIDSelector.h"

APFloatingGridOrderedStrategy::APFloatingGridOrderedStrategy()
{
	m_valid = false;
	//m_ordered = false;
	m_ignoreSection = true;
}

APStrategy * APFloatingGridOrderedStrategy::create()
{
	return new APFloatingGridOrderedStrategy();
}

void APFloatingGridOrderedStrategy::init(std::string strategyInfo)
{
	APFloatingGridStrategy::init(strategyInfo);

	APSingleQuotationDecision* singleQuotation = dynamic_cast<APSingleQuotationDecision*>(m_quotationDecision);
	if (singleQuotation != NULL) {
		m_valid = true;
	}

	m_gridsOverlapped.clear();
	for (int i = 0; i < m_grids.size(); i++) {
		m_gridsOverlapped.push_back(false);
	}

	if (m_positionCtrl != NULL) {
		if (m_positionCtrl->getOpenOrdersPosition() + m_positionCtrl->getCloseOrdersPosition() > 0) {
			m_positionCtrl->cancelAllTrade();
			m_positionCtrl->removeAllLocalOrders();
		}

		m_positionCtrl->addObserver(this);
	}
}

//void APFloatingGridOrderedStrategy::setOrdered(bool ordered)
//{
//	m_ordered = ordered;
//}

void APFloatingGridOrderedStrategy::exit()
{
	if (!m_valid) {
		return;
	}

	if (m_positionCtrl != NULL) {
		m_positionCtrl->cancelAllTrade();
		m_positionCtrl->removeAllLocalOrders();
	}

	APFloatingGridStrategy::exit();
}

void APFloatingGridOrderedStrategy::locateGrids(double valueRef)
{
	if (!m_valid) {
		return;
	}

	if (m_positionCtrl == NULL) {
		return;
	}

	m_curIndex = getGridIndex(valueRef);
	m_nextIndex = m_curIndex + 1;
	m_prevIndex = m_curIndex;
	
	int openIndex = getReferIndex(m_curIndex, TT_Open);
	if (m_grids[openIndex].position > 0) {
		long hold = m_positionCtrl->getHoldPosition();
		if (hold < m_grids[openIndex].position) {
			open(m_grids[openIndex].valueRef, m_grids[openIndex].position - hold);
		}
		
		//APLogger->log("Open index: %d, price: %f, volume: %d. ", openIndex, m_grids[openIndex].valueRef, m_grids[openIndex].position);
		m_gridsOverlapped[openIndex] = true;

		setAdjacentIndex(m_direction);
		tryOrderAdjacentGrid();
	}

	m_located = true;
	m_lastIndex = m_curIndex;
	m_lastValue = valueRef;
}

void APFloatingGridOrderedStrategy::enterGridInOpenWay(int gridIndex, APGridSectionType section)
{
	enterGrid(gridIndex);
}

void APFloatingGridOrderedStrategy::enterGridInCloseWay(int gridIndex, APGridSectionType section)
{
	enterGrid(gridIndex);
}

void APFloatingGridOrderedStrategy::goGrids(double valueRef)
{
	APFloatingGridStrategy::goGrids(valueRef);

	tryOrderAdjacentGrid();
}

void APFloatingGridOrderedStrategy::openIfNotOverlapped(int index)
{
	if (!m_valid) {
		return;
	}

	if (isIndexValid(index)) {
		if (m_gridsOverlapped[index] == false) {
			open(m_grids[index].valueRef, m_deltaPosition);
			//APLogger->log("Open index: %d, price: %f, volume: %d. ", index, m_grids[index].valueRef, m_deltaPosition);
			m_gridsOverlapped[index] = true;
		}
	}
}

void APFloatingGridOrderedStrategy::closeIfNotOverlapped(int index)
{
	if (!m_valid) {
		return;
	}

	if (m_positionCtrl == NULL) {
		return;
	}
	long available = m_positionCtrl->getAvailablePosition();
	if (isIndexValid(index)) {
		if (m_gridsOverlapped[index] == false && available >= m_deltaPosition) {
			close(m_grids[index].valueRef, m_deltaPosition);
			//APLogger->log("Close index: %d, price: %f, volume: %d. ", index, m_grids[index].valueRef, m_deltaPosition);
			m_gridsOverlapped[index] = true;
		}
	}
}

bool APFloatingGridOrderedStrategy::closeIfAvailable(int index)
{
	if (!m_valid) {
		return false;
	}

	bool ret = false;
	long available = m_positionCtrl->getAvailablePosition();
	if (isIndexValid(index)) {
		if (available >= m_deltaPosition) {
			close(m_grids[index].valueRef, m_deltaPosition);
			//APLogger->log("Close index: %d, price: %f, volume: %d. ", index, m_grids[index].valueRef, m_deltaPosition);
			m_gridsOverlapped[index] = true;
			ret = true;
		}
	}
	return ret;
}

void APFloatingGridOrderedStrategy::setAdjacentIndex(APTradeDirection direction)
{
	//if (direction == TD_Buy) {
	//	m_nextIndex = m_curIndex + 2;
	//	m_prevIndex = m_curIndex;
	//}
	//else {
	//	m_nextIndex = m_curIndex + 1;
	//	m_prevIndex = m_curIndex - 1;
	//}

	m_nextIndex = m_curIndex + 1;
	m_prevIndex = m_curIndex - 1;
}

bool APFloatingGridOrderedStrategy::isIndexValid(int index)
{
	if (index < 0 || index >= 2 * m_gridsCount) {
		return false;
	}

	return true;
}

int APFloatingGridOrderedStrategy::getReferIndex(int index, APTradeType tradeType)
{
	int targetIndex = index;
	if (tradeType == TT_Open) {
		if (m_direction == TD_Buy) {
			targetIndex = index + 1;
		}
	}
	else {
		if (m_direction == TD_Sell) {
			targetIndex = index + 1;
		}
	}

	return targetIndex;
}

void APFloatingGridOrderedStrategy::enterGrid(int gridIndex)
{
	if (!m_valid) {
		return;
	}

	if (gridIndex > m_curIndex) {
		for (int i = m_curIndex; i <= gridIndex; i++) {
			m_gridsOverlapped[i] = false;
		}
	}
	else if (gridIndex < m_curIndex) {
		for (int i = gridIndex; i <= m_curIndex; i++) {
			m_gridsOverlapped[i] = false;
		}
	}

	m_curIndex = gridIndex;

	if (m_direction == TD_Buy) {
		setAdjacentIndex(TD_Buy);
		openIfNotOverlapped(m_prevIndex);
		closeIfNotOverlapped(m_nextIndex);
	}
	else {
		setAdjacentIndex(TD_Sell);
		openIfNotOverlapped(m_nextIndex);
		closeIfNotOverlapped(m_prevIndex);
	}
}

void APFloatingGridOrderedStrategy::tryOrderAdjacentGrid()
{
	if (m_direction == TD_Buy) {
		openIfNotOverlapped(m_prevIndex);
		closeIfNotOverlapped(m_nextIndex);
	}
	else {
		openIfNotOverlapped(m_nextIndex);
		closeIfNotOverlapped(m_prevIndex);
	}
}

void APFloatingGridOrderedStrategy::onTradeOrdered(APASSETID instrumentID, APTradeType type, APTradeDirection direction)
{
}

void APFloatingGridOrderedStrategy::onTradeCanceled(APASSETID instrumentID, APTradeType type, APTradeDirection direction)
{
	if (direction == TD_Buy) {
		m_gridsOverlapped[m_prevIndex] = false;
	}
	else {
		m_gridsOverlapped[m_nextIndex] = false;
	}
}

void APFloatingGridOrderedStrategy::onTradeFinished(APASSETID instrumentID, APTradeType type, APTradeDirection direction)
{
}

void APFloatingGridOrderedStrategy::onTradeFailed(APASSETID instrumentID, APTradeType type, APTradeDirection direction)
{
	if (direction == TD_Buy) {
		m_gridsOverlapped[m_prevIndex] = false;
	}
	else {
		m_gridsOverlapped[m_nextIndex] = false;
	}
}
