#include "APFloatingGridOrderedStrategy.h"
#include "../Utils/APJsonReader.h"
#include "../QuotationDecision/APSingleQuotationDecision.h"
#include "../APPositionCtrl.h"
#include "../Utils/APLog.h"
#include "../Futures/APFuturesIDSelector.h"
#include "../Utils/APJsonReader.h"

APFloatingGridOrderedStrategy::APFloatingGridOrderedStrategy()
{
	m_valid = false;
	//m_hasPosition = false;
	m_ignoreSection = true;

	m_orderCount = 1;
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

	APJsonReader jr;
	jr.initWithString(strategyInfo);
	if (jr.hasMember("OrderCount")) {
		int orderCount = jr.getIntValue("OrderCount");
		m_orderCount = std::max(1, orderCount);
		m_orderCount = std::min(m_orderCount, (int)(m_grids.size() / 2));
	}

	//m_gridsOverlapped.clear();
	//for (int i = 0; i < m_grids.size(); i++) {
	//	m_gridsOverlapped.push_back(false);
	//}
	m_gridsTarget.clear();
	m_gridsApplied.clear();
	m_gridsOrdered.clear();
	for (int i = 0; i < m_grids.size(); i++) {
		m_gridsTarget.push_back(0);
		m_gridsApplied.push_back(0);
		m_gridsOrdered.push_back(0);
	}

	if (m_positionCtrl != NULL) {
		if (m_positionCtrl->getOpenOrdersPosition() + m_positionCtrl->getCloseOrdersPosition() > 0) {
			m_positionCtrl->cancelAllTrade();
			m_positionCtrl->removeAllLocalOrders();
		}

		m_positionCtrl->addObserver(this);
	}
}

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
	m_prevIndex = m_curIndex - 1;
	
	//int openIndex = getReferIndex(m_curIndex, TT_Open);
	if (m_grids[m_curIndex].position > 0) {
		long hold = m_positionCtrl->getHoldPosition();
		if (hold < m_grids[m_curIndex].position) {
			open(m_grids[m_curIndex].valueRef, m_grids[m_curIndex].position - hold);
		}
		
		APLogger->log("Open index: %d, price: %f, volume: %d. ", m_curIndex, m_grids[m_curIndex].valueRef, m_grids[m_curIndex].position - hold);

		setAdjacentGridsTarget();
	}

	if (m_grids[m_curIndex].position > 0) {
		m_located = true;
	}
	
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

	if (!m_positionBuilt) {
		return;
	}
	followUp();
}

int APFloatingGridOrderedStrategy::getIndexByPrice(double price)
{
	int index = getGridIndex(price);

	double minDiff = DBL_MAX;
	int closestIndex = index;
	for (int i = index - 1; i < index + 1; i++) {
		if (i >= 0 && i < m_grids.size()) {
			double deltaVal = fabs(price - m_grids[i].valueRef);
			if (deltaVal < minDiff) {
				closestIndex = i;
				minDiff = deltaVal;
			}
		}
	}

	return closestIndex;
}

bool APFloatingGridOrderedStrategy::isIndexValid(int index)
{
	if (index < 0 || index >= 2 * m_gridsCount) {
		return false;
	}

	return true;
}

void APFloatingGridOrderedStrategy::enterGrid(int gridIndex)
{
	if (!m_valid) {
		return;
	}

	m_curIndex = gridIndex;
	m_prevIndex = m_curIndex - 1;
	m_nextIndex = m_curIndex + 1;

	setAdjacentGridsTarget();
}

void APFloatingGridOrderedStrategy::setAdjacentGridsTarget()
{
	if (m_direction == TD_Buy) {
		for (int i = 0; i < m_orderCount; i++) {
			int index = i + m_nextIndex;
			setGridTarget(index, -m_deltaPosition);
		}

		for (int i = 0; i < m_orderCount; i++) {
			int index = m_prevIndex - i;
			setGridTarget(index, m_deltaPosition);
		}
	}
	else {
		for (int i = 0; i < m_orderCount; i++) {
			int index = i + m_nextIndex;
			setGridTarget(index, m_deltaPosition);
		}

		for (int i = 0; i < m_orderCount; i++) {
			int index = m_prevIndex - i;
			setGridTarget(index, -m_deltaPosition);
		}
	}

	setGridTarget(m_curIndex, 0);

	for (int i = m_nextIndex + m_orderCount; i < m_grids.size(); i++) {
		setGridTarget(i, 0);
	}

	for (int i = m_prevIndex - m_orderCount; i >= 0; i--) {
		setGridTarget(i, 0);
	}

//#ifdef _DEBUG
//	printGridsTarget();
//#endif
}

void APFloatingGridOrderedStrategy::setGridTarget(int index, long target)
{
	if (index < 0 || index >= m_grids.size()) {
		return;
	}

	m_gridsTarget[index] = target;
}

void APFloatingGridOrderedStrategy::followUp()
{
	for (int i = 0; i < m_grids.size(); i++) {
		int commitedCount = m_gridsApplied[i] + m_gridsOrdered[i];
		if (commitedCount != m_gridsTarget[i]) {
			APTradeType type = TT_Open;
			if (commitedCount != 0 && commitedCount * m_gridsTarget[i] <= 0) {
				if (commitedCount < 0) {
					type = TT_Close;
				}
				cancelByIndex(i, type);
				
			}
			else if(m_gridsTarget[i] != 0){

				if (m_gridsTarget[i] > 0) {
					type = TT_Open;
				}
				else{
					type = TT_Close;
				}

				int vol = m_gridsTarget[i] - commitedCount;
				orderByIndex(i, type, abs(vol));
			}
		}
	}
}

void APFloatingGridOrderedStrategy::cancelByIndex(int index, APTradeType type)
{
	if (index < 0 || index >= m_grids.size()) {
		return;
	}

	if (m_positionCtrl != NULL) {
		m_positionCtrl->cancelTrade(type, m_grids[index].valueRef, m_direction);
	}

	m_gridsApplied[index] = 0;
	//m_gridsOrdered[index] = 0;
}

void APFloatingGridOrderedStrategy::orderByIndex(int index, APTradeType type, int volume)
{
	if (index < 0 || index >= m_grids.size()) {
		return;
	}

	if (m_positionCtrl != NULL) {
		if (type == TT_Open) {
			m_gridsApplied[index] += volume;
			open(m_grids[index].valueRef, volume);
		}
		else {
			m_gridsApplied[index] += -volume;
			close(m_grids[index].valueRef, volume);
		}
	}
}

#ifdef _DEBUG
void APFloatingGridOrderedStrategy::printGridsTarget()
{
	for (int i = 0; i < m_gridsTarget.size(); i++) {
		printf("Target[%d] = %d\n", i, m_gridsTarget[i]);
	}
}
#endif

void APFloatingGridOrderedStrategy::onTradeOrdered(APASSETID instrumentID, APTradeType type, double price, long volume, APTradeDirection direction)
{
	if (!m_positionBuilt) {
		return;
	}
	int index = getIndexByPrice(price);
	if (fabs(price - m_grids[index].valueRef) > 1.0) {
		return;
	}
	if (type == TT_Open) {
		m_gridsApplied[index] -= volume;
		m_gridsOrdered[index] += volume;
	}
	else {
		m_gridsApplied[index] += volume;
		m_gridsOrdered[index] -= volume;
	}
}

void APFloatingGridOrderedStrategy::onTradeCanceled(APASSETID instrumentID, APTradeType type, double price, long volume, APTradeDirection direction)
{
	if (!m_positionBuilt) {
		return;
	}
	int index = getIndexByPrice(price);
	if (fabs(price - m_grids[index].valueRef) > 1.0) {
		return;
	}
	if (type == TT_Open) {
		m_gridsOrdered[index] -= volume;
	}
	else {
		m_gridsOrdered[index] += volume;
	}
}

void APFloatingGridOrderedStrategy::onTradeFinished(APASSETID instrumentID, APTradeType type, double price, long volume, APTradeDirection direction)
{
}

void APFloatingGridOrderedStrategy::onTradeFailed(APASSETID instrumentID, APTradeType type, double price, long volume, APTradeDirection direction)
{
	if (!m_positionBuilt) {
		return;
	}
	int index = getIndexByPrice(price);
	if (fabs(price - m_grids[index].valueRef) > 1.0) {
		return;
	}
	if (type == TT_Open) {
		m_gridsApplied[index] -= volume;
	}
	else {
		m_gridsApplied[index] += volume;
	}
}
