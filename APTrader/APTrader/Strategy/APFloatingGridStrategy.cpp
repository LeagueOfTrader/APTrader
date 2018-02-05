#include "APFloatingGridStrategy.h"
#include "../APPositionCtrl.h"

APStrategy * APFloatingGridStrategy::create()
{
	return new APFloatingGridStrategy();
}

void APFloatingGridStrategy::enterGridInOpenWay(int gridIndex, APGridSectionType section)
{
	if (m_positionCtrl == NULL) {
		return;
	}

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
	else {
		if (hold < m_basePosition) {
			m_positionCtrl->openPosition(m_deltaPosition);
		}
	}
}

void APFloatingGridStrategy::enterGridInCloseWay(int gridIndex, APGridSectionType section)
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
			m_positionCtrl->closePosition(m_deltaPosition);
		}
	}
}
