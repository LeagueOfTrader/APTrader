#include "APFloatingGridOrderedStrategy.h"
#include "../Utils/APJsonReader.h"
#include "../QuotationDecision/APSingleQuotationDecision.h"
#include "../APPositionCtrl.h"

APFloatingGridOrderedStrategy::APFloatingGridOrderedStrategy()
{
	m_valid = false;
	m_ordered = false;
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
}

void APFloatingGridOrderedStrategy::setOrdered(bool ordered)
{
	m_ordered = ordered;
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

	if (!m_ordered) {
		if (m_positionCtrl->getOpenOrdersPosition() + m_positionCtrl->getCloseOrdersPosition() > 0) {
			m_ordered = true;
		}
	}

	APFloatingGridStrategy::locateGrids(valueRef);
	
	int targetIndex = m_curIndex;
	APGridSectionType section = getSection(valueRef);
	if (section == GST_Open) {
		if (m_direction == TD_Buy) {
			targetIndex = m_curIndex + 1;
		}
		else {
			targetIndex = m_curIndex;
		}		
	}
	else if (section == GST_Close) {
		if (m_direction == TD_Buy) {
			targetIndex = m_curIndex;
		}
		else {
			targetIndex = m_curIndex + 1;
		}
	}
	m_gridsOverlapped[targetIndex] = true;

	if (!m_ordered) {
		long hold = m_positionCtrl->getHoldPosition();
		int closeCount = hold / m_deltaPosition;

		if (m_direction == TD_Buy) {
			for (int i = targetIndex - 1; i >= 0; i--) {
				open(m_grids[i].valueRef, m_deltaPosition);
				m_gridsOverlapped[i] = true;
			}
			for (int i = targetIndex + 1; i < targetIndex + 1 + closeCount && i < 2 * m_gridsCount; i++) {
				closeIfAvailable(i);
			}
		}
		else {
			for (int i = targetIndex - 1; i >= targetIndex - 1 - closeCount && i >= 0; i--) {
				closeIfAvailable(i);
			}
			for (int i = targetIndex + 1; i < 2 * m_gridsCount; i++) {
				open(m_grids[i].valueRef, m_deltaPosition);
				m_gridsOverlapped[i] = true;
			}
		}
	}
	else {
		long openPos = m_positionCtrl->getOpenOrdersPosition();
		long closePos = m_positionCtrl->getCloseOrdersPosition();
		long openCount = openPos / m_deltaPosition;
		long closeCount = closePos / m_deltaPosition;

		if (m_direction == TD_Buy) {
			for (int i = targetIndex - 1; i >= targetIndex - 1 - openCount && i >= 0; i--) {
				m_gridsOverlapped[i] = true;
			}
			for (int i = targetIndex + 1; i < targetIndex + 1 + closeCount && i < 2 * m_gridsCount; i++) {
				m_gridsOverlapped[i] = true;
			}
		}
		else {
			for (int i = targetIndex - 1; i >= targetIndex - 1 - closeCount && i >= 0; i--) {
				m_gridsOverlapped[i] = true;
			}
			for (int i = targetIndex + 1; i < targetIndex + 1 + openCount && i < 2 * m_gridsCount; i++) {
				m_gridsOverlapped[i] = true;
			}
		}

		if (openCount == 0) {
			long openIndex = 0;
			if (m_direction == TD_Buy) {
				openIndex = m_curIndex;
			}
			else {
				openIndex = m_curIndex + 1;
			}

			openIfNotOverlapped(openIndex);
		}

		if (closeCount == 0) {
			long closeIndex = 0;
			if (m_direction == TD_Buy) {
				closeIndex = m_curIndex + 1;
			}
			else {
				closeIndex = m_curIndex;
			}

			closeIfNotOverlapped(closeIndex);
		}
	}
}

void APFloatingGridOrderedStrategy::enterGridInOpenWay(int gridIndex, APGridSectionType section)
{
	if (!m_valid) {
		return;
	}

	int nextIndex = gridIndex + 1;
	//int prevIndex = gridIndex - 1;
	m_curIndex = gridIndex;

	if (m_direction == TD_Buy) {
		close(m_grids[nextIndex].valueRef, m_deltaPosition);
		openIfNotOverlapped(gridIndex);
	}
	else {
		close(m_grids[gridIndex].valueRef, m_deltaPosition);
		openIfNotOverlapped(nextIndex);
	}
}

void APFloatingGridOrderedStrategy::enterGridInCloseWay(int gridIndex, APGridSectionType section)
{
	if (!m_valid) {
		return;
	}

	int nextIndex = gridIndex + 1;
	//int prevIndex = gridIndex - 1;
	m_curIndex = gridIndex;

	if (m_direction == TD_Buy) {
		open(m_grids[gridIndex].valueRef, m_deltaPosition);
		closeIfNotOverlapped(nextIndex);
	}
	else {
		open(m_grids[nextIndex].valueRef, m_deltaPosition);
		closeIfNotOverlapped(gridIndex);
	}
}

void APFloatingGridOrderedStrategy::openIfNotOverlapped(int index)
{
	if (!m_valid) {
		return;
	}

	if (index >= 0 && index < m_gridsCount * 2) {
		if (m_gridsOverlapped[index] == false) {
			open(m_grids[index].valueRef, m_deltaPosition);
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
	if (index >= 0 && index < m_gridsCount * 2) {
		if (m_gridsOverlapped[index] == false && available >= m_deltaPosition) {
			close(m_grids[index].valueRef, m_deltaPosition);
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
	if (index >= 0 && index < m_gridsCount * 2) {
		if (available >= m_deltaPosition) {
			close(m_grids[index].valueRef, m_deltaPosition);
			m_gridsOverlapped[index] = true;
			ret = true;
		}
	}
	return ret;
}
