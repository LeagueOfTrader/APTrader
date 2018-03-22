#include "APFuturesSyncCombPosCtrl.h"
#include "../APInstrumentQuotation.h"


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

void APFuturesSyncCombPosCtrl::setHoldPosition(long position)
{
	APPositionCtrl::setHoldPosition(position);

	m_prVolume = m_holdPosition * m_prUnitVol;
	m_coVolume = m_holdPosition * m_coUnitVol;
}

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
			//m_curOpenOperation.prVolume += deltaVolume;			
		}
		else if (instrumentID == m_coInstrumentID) {
			// co traded
			m_coVolume += deltaVolume;
			//m_curOpenOperation.coVolume += deltaVolume;
		}

		//if (m_curOpenOperation.isFinishTarget()) {
		//	m_holdPosition += m_curOpenOperation.groupCount;
		//	m_availablePosition += m_curOpenOperation.groupCount;
		//	m_openOrdersPosition -= m_curOpenOperation.groupCount;
		//	m_curOpenOperation.reset();
		//}

		m_positionMutex.unlock();
		break;
	case TT_Close:
	case TT_CloseToday:
		m_positionMutex.lock();
		if (instrumentID == m_instrumentID) {
			// pr traded
			m_prVolume -= deltaVolume;
			//m_curCloseOperation.prVolume += deltaVolume;
		}
		else if (instrumentID == m_coInstrumentID) {
			// co traded
			m_coVolume -= deltaVolume;
			//m_curCloseOperation.coVolume += deltaVolume;
		}

		//if (m_curCloseOperation.isFinishTarget()) {
		//	// finish
		//	m_holdPosition -= m_curCloseOperation.groupCount;
		//	m_availablePosition -= m_curCloseOperation.groupCount;

		//	m_closeOrdersPosition -= m_curCloseOperation.groupCount;
		//	m_curOpenOperation.reset();
		//}

		m_positionMutex.unlock();
		break;
	default:
		break;
	}
}

void APFuturesSyncCombPosCtrl::onTradeRollback(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTradeDirection direction)
{
	//APFuturesPositionCtrl::onTradeRollback(instrumentID, type, volume, orderID, direction);
}

void APFuturesSyncCombPosCtrl::onTradeFinished(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction)
{
	APFuturesPositionCtrl::onTradeFinished(instrumentID, type, price, volume, orderID, direction);

	long maxVol = std::max(m_prVolume / m_prUnitVol, m_coVolume / m_coUnitVol);
	long minVol = std::min(m_prVolume / m_prUnitVol, m_coVolume / m_coUnitVol);

	if (type == TT_Open) {
		m_holdPosition = minVol;
		m_availablePosition = minVol;
	}
	else {
		m_holdPosition = maxVol;
		m_availablePosition = maxVol;
	}
}

void APFuturesSyncCombPosCtrl::openPrPosition()
{
	long vol = m_curOpenOperation.prTarget;
	double price = m_quotation->getOpponentPrice(m_directionType);//m_quotation->getCurPrice();
	//open(m_instrumentID, m_directionType, price, vol);
	open(m_instrumentID, m_directionType, OPT_AnyPrice, price, OTC_GoodForDay, "", OVC_Any, vol);
}

void APFuturesSyncCombPosCtrl::closePrPosition()
{
	long vol = m_curCloseOperation.prTarget;
	APTradeDirection dir = getReversedDirection(m_directionType);
	double price = m_quotation->getOpponentPrice(dir);//m_quotation->getCurPrice();

	close(m_coInstrumentID, dir, OPT_AnyPrice, price, OTC_GoodForDay, "", OVC_Any, vol);
}

//void APFuturesSyncCombPosCtrl::openCoPosition()
//{
//	long vol = m_curOpenOperation.coTarget;
//	double price = m_coQuotation->getOpponentPrice(m_coDirectionType);//m_coQuotation->getCurPrice();
//	open(m_coInstrumentID, m_coDirectionType, price, vol);
//}
//
//void APFuturesSyncCombPosCtrl::closeCoPosition()
//{
//	long vol = m_curCloseOperation.coTarget;
//	APTradeDirection dir = getReversedDirection(m_coDirectionType);
//	double price = m_coQuotation->getOpponentPrice(dir);//m_coQuotation->getCurPrice();
//	close(m_coInstrumentID, dir, price, vol);
//}
