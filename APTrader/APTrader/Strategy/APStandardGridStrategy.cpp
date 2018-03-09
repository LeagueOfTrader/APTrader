#include "APStandardGridStrategy.h"
#include "../Utils/APJsonReader.h"
#include <algorithm>
#include "../APPositionCtrl.h"
#include "../Utils/APLog.h"
#include <sstream>

APStandardGridStrategy::APStandardGridStrategy()
{
	m_located = false;
	m_positionBuilt = false;

	m_curIndex = -1;
	//m_prevIndex = -1;
	//m_nextIndex = -1;
	m_lastValue = 0.0;
	m_ignoreSection = false;
}


APStandardGridStrategy::~APStandardGridStrategy()
{
}

APStrategy * APStandardGridStrategy::create()
{
	return new APStandardGridStrategy();
}

void APStandardGridStrategy::buildBuyGrids(std::vector<double>& longValues, std::vector<double>& shortValues)
{
	m_grids.clear();
	m_grids.resize(2 * m_gridsCount);
	
	m_grids[m_gridsCount - 1].assign(m_shortValue, m_basePosition);
	for (int i = 1; i < m_gridsCount; i++) {
		int index = m_gridsCount - i - 1;
		m_grids[index].assign(shortValues[i], m_grids[index + 1].position + m_deltaPosition);
	}

	m_openIndex = m_gridsCount - 1;

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

	m_openIndex = m_gridsCount;

	m_grids[m_gridsCount - 1].assign(m_shortValue, m_basePosition / 2);
	for (int i = 1; i < m_gridsCount; i++) {
		int index = m_gridsCount - i - 1;
		m_grids[index].assign(shortValues[i], m_grids[index + 1].position / 2);
	}
}

void APStandardGridStrategy::goGrids(double valueRef)
{
	if (!m_located) {
		locateGrids(valueRef);

		return;
	}

	APGridSectionType section = getSection(valueRef);
	if (section == GST_Indeterminate && !m_ignoreSection) {
		return;
	}

	int nextIndex = m_curIndex + 1;
	int prevIndex = m_curIndex - 1;
	bool moved = false;

	if (valueRef > m_lastValue) {
		if (nextIndex < 2 * m_gridsCount) {
			if (valueRef > m_grids[nextIndex].valueRef) {
				int index = getGridIndex(valueRef);
				// move to new grid;
				if (m_positionBuilt) {
					if (m_direction == TD_Buy) {
						enterGridInCloseWay(index, section);
					}
					else
					{
						enterGridInOpenWay(index, section);
					}
				}

				moved = true;
			}
		}
	}
	else if (valueRef < m_lastValue) {
		if (prevIndex >= 0) {
			if (valueRef < m_grids[prevIndex].valueRef) {
				int index = getGridIndex(valueRef);

				if (m_positionBuilt) {
					if (m_direction == TD_Buy) {
						enterGridInOpenWay(index, section);
					}
					else
					{
						enterGridInCloseWay(index, section);
					}
				}

				moved = true;
			}
		}
	}
	if (!m_positionBuilt) {
		if (moved) {			
			m_positionCtrl->cancelAllTrade();
			m_located = false;			
		}
		else {
			if (m_positionCtrl->getHoldPosition() >= m_grids[m_curIndex].position) {
				m_positionBuilt = true;
			}
		}
	}

	m_lastValue = valueRef;
}

void APStandardGridStrategy::locateGrids(double valueRef)
{
	// LONG [n ~ n+1] : index = n, <= max(n)
	// SHORT [n ~ n+1] : index = n + 1, <= max(n)
	m_curIndex = getGridIndex(valueRef);

	if (m_positionCtrl == NULL) {
		return;
	}

	APGridSectionType section = getSection(valueRef);
	//int targetIndex = m_curIndex;
	
	if (section == GST_Open) {	
		long hold = m_positionCtrl->getForeseeableHoldPosition();
		if (hold < m_grids[m_curIndex].position) {
			long volume = m_grids[m_curIndex].position - hold;
			m_positionCtrl->openPosition(volume);
		}
	}
	else if(section == GST_Close){
		long available = m_positionCtrl->getAvailablePosition();
		if (available > m_grids[m_curIndex].position) {
			long volume = available - m_grids[m_curIndex].position;
			m_positionCtrl->closePosition(volume);
		}
	}

	if (m_grids[m_curIndex].position > 0) {
		m_located = true;
	}
	
	m_lastIndex = m_curIndex;
	m_lastValue = valueRef;
}

int APStandardGridStrategy::getGridIndex(double curValue)//, bool reversed)
{
	int index = 0;
	int i = 0;
	for (i = 0; i < m_grids.size(); i++) {
		if (curValue < m_grids[i].valueRef) {
			break;
		}
	}

	index = std::max(i - 1, 0);

	if (m_direction == TD_Buy) {
		index++;
		index = std::min(index, (int)(m_grids.size() - 1));
	}

	return index;
}

//bool APStandardGridStrategy::inOpenSection(int curIndex)
//{
//	bool ret = false;
//	if (m_direction == TD_Buy) {
//		ret = (m_curIndex <= m_openIndex);
//	}
//	else {
//		ret = (m_curIndex >= m_openIndex);
//	}
//	return ret;
//}

APGridSectionType APStandardGridStrategy::getSection(double valueRef)
{
	APGridSectionType section = GST_Indeterminate;
	if (m_direction == TD_Buy) {
		if (valueRef > m_longValue) {
			section = GST_Close;
		}
		else if(valueRef < m_shortValue){
			section = GST_Open;
		}
	}
	else {
		if (valueRef > m_longValue) {
			section = GST_Open;
		}
		else if (valueRef < m_shortValue) {
			section = GST_Close;
		}
	}

	return section;
}

void APStandardGridStrategy::enterGridInOpenWay(int gridIndex, APGridSectionType section)
{
	if (m_positionCtrl == NULL) {
		return;
	}

	int nextIndex = m_curIndex + 1;
	int prevIndex = m_curIndex - 1;

	m_curIndex = gridIndex;
	int targetIndex = m_curIndex;
	long hold = m_positionCtrl->getForeseeableHoldPosition();
	if (section == GST_Open) {
		if (m_direction == TD_Buy) {
			targetIndex = m_curIndex + 1;
		}
		else {
			targetIndex = m_curIndex;
		}

		if (hold < m_grids[m_curIndex].position) {
			long volume = m_grids[m_curIndex].position - hold;
			m_positionCtrl->openPosition(volume);
		}
	}
	//else {
	//	if (hold < m_basePosition) {
	//		m_positionCtrl->openPosition(m_deltaPosition);
	//	}
	//}
}

void APStandardGridStrategy::enterGridInCloseWay(int gridIndex, APGridSectionType section)
{
	if (m_positionCtrl == NULL) {
		return;
	}

	int targetIndex = m_curIndex;
	m_curIndex = gridIndex;
	if (section == GST_Open) {
		if (m_direction == TD_Buy) {
			targetIndex = m_curIndex + 1;
		}
		else {
			targetIndex = m_curIndex;
		}
		long hold = m_positionCtrl->getForeseeableHoldPosition();
		long base = std::max(m_basePosition, m_grids[targetIndex].position);
		if (hold > base) {
			long volume = hold - base;
			m_positionCtrl->closePosition(volume);
		}
	}
	else {
		if (m_direction == TD_Buy) {
			targetIndex = m_curIndex;
		}
		else {
			targetIndex = m_curIndex + 1;
		}
		long available = m_positionCtrl->getAvailablePosition();
		long target = m_grids[targetIndex].position;
		if (available > target) {
			long volume = available - target;
			m_positionCtrl->closePosition(volume);
		}
		else {
			//m_positionCtrl->closePosition(m_deltaPosition);
		}
	}
}




#ifdef _DEBUG
void APStandardGridStrategy::printGrids()
{
	for (int i = 0; i < m_grids.size(); i++) {
		APGridData& data = m_grids[i];
		std::stringstream ss;
		ss << "[" << i << "], quotation: " << data.valueRef << ", position: " << data.position << ". ";
		APLogger->log(ss.str().c_str());
	}
}
#endif
