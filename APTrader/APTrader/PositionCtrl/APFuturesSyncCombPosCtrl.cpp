#include "APFuturesSyncCombPosCtrl.h"



APFuturesSyncCombPosCtrl::APFuturesSyncCombPosCtrl()
{
}


APFuturesSyncCombPosCtrl::~APFuturesSyncCombPosCtrl()
{
}

void APFuturesSyncCombPosCtrl::openPosition(long volume)
{
	if (m_curOpenOperation.hasTarget()) {
		return;
	}

	m_curOpenOperation.setTarget(volume * m_prUnitVol, volume * m_coUnitVol, volume);
	openPrPosition();
	openCoPosition();

	m_positionMutex.lock();
	m_availablePosition -= volume;
	m_openOrdersPosition += volume;
	m_positionMutex.unlock();
}

void APFuturesSyncCombPosCtrl::closePosition(long volume)
{
	if (m_curCloseOperation.hasTarget()) {
		return;
	}

	m_curCloseOperation.setTarget(volume * m_prUnitVol, volume * m_coUnitVol, volume);
	closePrPosition();
	closeCoPosition();

	m_positionMutex.lock();
	m_holdPosition -= volume;
	m_closeOrdersPosition += volume;
	m_positionMutex.unlock();
}

//void APFuturesSyncCombPosCtrl::openFullPosition()
//{
//}
//
//void APFuturesSyncCombPosCtrl::closeOffPosition()
//{
//}

void APFuturesSyncCombPosCtrl::cancel(APTradeType type)
{
}

void APFuturesSyncCombPosCtrl::onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long deltaVolume, APORDERID orderID, APTradeDirection direction)
{
	switch (type) {
	case TT_Open:
		m_positionMutex.lock();
		if (instrumentID == m_instrumentID) {
			// pr traded
			m_prVolume += deltaVolume;
			m_curOpenOperation.prVolume += deltaVolume;			
		}
		else if (instrumentID == m_coInstrumentID) {
			// co traded
			m_coVolume += deltaVolume;
			m_curOpenOperation.coVolume += deltaVolume;
		}

		if (m_curOpenOperation.isFinishTarget()) {
			m_holdPosition += m_curOpenOperation.groupCount;
			m_availablePosition += m_curOpenOperation.groupCount;
			m_openOrdersPosition -= m_curOpenOperation.groupCount;
			m_curOpenOperation.reset();
		}

		m_positionMutex.unlock();
		break;
	case TT_Close:
		m_positionMutex.lock();
		if (instrumentID == m_instrumentID) {
			// pr traded
			m_prVolume -= deltaVolume;
			m_curCloseOperation.prVolume += deltaVolume;
		}
		else if (instrumentID == m_coInstrumentID) {
			// co traded
			m_coVolume -= deltaVolume;
			m_curCloseOperation.coVolume += deltaVolume;
		}
		if (m_curCloseOperation.isFinishTarget()) {
			// finish
			m_holdPosition -= m_curCloseOperation.groupCount;
			m_availablePosition -= m_curCloseOperation.groupCount;

			m_closeOrdersPosition -= m_curCloseOperation.groupCount;
			m_curOpenOperation.reset();
		}

		m_positionMutex.unlock();
		break;
	default:
		break;
	}
}

void APFuturesSyncCombPosCtrl::onTradeCanceled(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTradeDirection direction)
{
}
