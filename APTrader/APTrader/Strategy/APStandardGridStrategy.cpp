#include "APStandardGridStrategy.h"
#include "../Utils/APJsonReader.h"
#include <algorithm>
#include "../APPositionCtrl.h"


APStandardGridStrategy::APStandardGridStrategy()
{
	m_located = false;
	m_curIndex = -1;
	m_prevIndex = -1;
	m_nextIndex = -1;
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
	if (section == GST_Indeterminate) {
		return;
	}

	if (valueRef > m_grids[m_curIndex].valueRef) {
		if (m_nextIndex < 2 * m_gridsCount) {
			if (valueRef > m_grids[m_nextIndex].valueRef) {
				int index = getGridIndex(valueRef);
				// move to new grid;
				if (m_direction == TD_Buy) {
					enterGridInCloseWay(index, section);
				}
				else
				{
					enterGridInOpenWay(index, section);
				}
			}
		}
	}
	else if (valueRef < m_grids[m_curIndex].valueRef) {
		if (m_prevIndex >= 0) {
			if (valueRef < m_grids[m_prevIndex].valueRef) {
				int index = getGridIndex(valueRef);

				if (m_direction == TD_Buy) {
					enterGridInOpenWay(index, section);
				}
				else
				{
					enterGridInCloseWay(index, section);
				}
			}
		}
	}
}

void APStandardGridStrategy::locateGrids(double valueRef)
{
	// todo: init location of grids
	
	m_curIndex = getGridIndex(valueRef);

	m_prevIndex = m_curIndex - 1;
	m_nextIndex = m_curIndex + 1;

	if (m_positionCtrl == NULL) {
		return;
	}

	APGridSectionType section = getSection(valueRef);
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

	m_located = true;
	m_lastIndex = m_curIndex;
}

int APStandardGridStrategy::getGridIndex(double curValue)//, bool reversed)
{
	bool reversed = false;
	int index = 0;
	if (curValue > m_longValue)
	{
		if (m_direction == TD_Buy) {
			reversed = false;
		}
		else {
			reversed = true;
		}
	}
	else if (curValue < m_shortValue)
	{
		if (m_direction == TD_Buy) {
			reversed = true;
		}
		else {
			reversed = false;
		}
	}
	else {
		m_curIndex = m_openIndex;
	}

	if (!reversed) {
		int i = 0;
		for (i = 0; i < m_grids.size(); i++) {
			if (curValue > m_grids[i].valueRef) {
				continue;
			}
		}

		index = std::min(i, (int)m_grids.size() - 1);
	}
	else
	{
		int i = 0;
		for (i = m_grids.size() - 1; i > 0; i--) {
			if (curValue < m_grids[i].valueRef) {
				continue;
			}
		}

		index = std::max(i, 0);
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

	m_curIndex = gridIndex;
	if (section == GST_Open) {
		long hold = m_positionCtrl->getForeseeableHoldPosition();
		if (hold < m_grids[m_curIndex].position) {
			long volume = m_grids[m_curIndex].position - hold;
			m_positionCtrl->openPosition(volume);
		}
	}
}

void APStandardGridStrategy::enterGridInCloseWay(int gridIndex, APGridSectionType section)
{
		if (m_positionCtrl == NULL) {
		return;
	}

	m_curIndex = gridIndex;
	m_prevIndex = m_curIndex - 1;
	if (section == GST_Open) {
		long hold = m_positionCtrl->getForeseeableHoldPosition();
		long base = std::max(m_basePosition, m_grids[m_prevIndex].position);
		if (hold > base) {
			long volume = base - hold;
			m_positionCtrl->closePosition(volume);
		}
	}
	else {
		long available = m_positionCtrl->getAvailablePosition();
		long target = m_grids[m_curIndex].position;
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
}
#endif
