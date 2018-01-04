#include "APFuturesCombinationPosCtrl.h"
#include "../APMarco.h"
#include "../Utils/APJsonReader.h"
#include "../APMarketDataManager.h"
#include "../APInstrumentQuotation.h"

APFuturesCombinationPosCtrl::APFuturesCombinationPosCtrl()
{
	m_coQuotation = NULL;

	m_priority = 2;
	m_curOpenOperation.reset();
	m_curCloseOperation.reset();
}

APFuturesCombinationPosCtrl::~APFuturesCombinationPosCtrl()
{
	if (m_coQuotation != NULL) {
		APMarketDataMgr->unSubscribeInstrument(m_coQuotation->getInstrumentID());
		m_coQuotation = NULL;
	}
}

void APFuturesCombinationPosCtrl::initWithData(std::string positionInfo)
{
	APPositionCtrl::initWithData(positionInfo);

	// derived func
	APJsonReader jr;
	jr.initWithString(positionInfo);
	
	m_coInstrumentID = jr.getStrValue("CoInstrumentID");
	m_prUnitVol = jr.getIntValue("PrUnit");
	m_coUnitVol = jr.getIntValue("CoUnit");
	std::string strCoTrend = jr.getStrValue("CoTrend");
	if (strCoTrend == "Buy") {
		m_coTrend = TD_Buy;
	}
	else if (strCoTrend == "Sell") {
		m_coTrend = TD_Sell;
	}

#ifndef SIM
	m_coQuotation = APMarketDataMgr->subscribeInstrument(m_instrumentID);
#else 
	m_coQuotation = NULL;
#endif
}

void APFuturesCombinationPosCtrl::openPosition(APTradeDirection direction, double price, long volume)
{
	if (m_curOpenOperation.hasTarget()) {
		return;
	}

	m_curOpenOperation.setTarget(volume * m_prUnitVol, volume * m_coUnitVol, volume);
	openPrPosition();

	m_availablePosition -= volume;
	m_openOrdersPosition += volume;
}

void APFuturesCombinationPosCtrl::closePosition(APTradeDirection direction, double price, long volume)
{
	if (m_curCloseOperation.hasTarget()) {
		return;
	}

	m_curCloseOperation.setTarget(volume * m_prUnitVol, volume * m_coUnitVol, volume);
	closePrPosition();

	m_holdPosition -= volume;
	m_closeOrdersPosition += volume;
}

void APFuturesCombinationPosCtrl::openFullPosition(APTradeDirection direction, double price)
{
	openPosition(direction, price, m_availablePosition);
}

void APFuturesCombinationPosCtrl::closeOffPosition(APTradeDirection direction, double price)
{
	closePosition(direction, price, m_holdPosition);
}

void APFuturesCombinationPosCtrl::cancel(APTradeType type)
{
	bool canCancel = false;

	if (type == TT_Open) {
		if (m_curOpenOperation.hasTarget() && !m_curOpenOperation.isFinishTarget()) {
			if (m_curOpenOperation.prVolume == 0) {
				//m_curOpenOperation.reset();
				canCancel = true;
			}
		}
	}
	else if (type == TT_Close) {
		if (m_curCloseOperation.hasTarget() && !m_curCloseOperation.isFinishTarget()) {
			if (m_curCloseOperation.prVolume == 0) {
				//m_curCloseOperation.reset();
				canCancel = true;
			}
		}
	}
	
	if (canCancel) {
		APFuturesPositionCtrl::cancel(type);
	}
}

void APFuturesCombinationPosCtrl::onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long deltaVolume, APORDERID orderID, APTradeDirection direction)
{
	switch (type) {
	case TT_Open:
		if (instrumentID == m_instrumentID) {
			// pr traded
			m_curOpenOperation.prVolume += deltaVolume;
			if (m_curOpenOperation.prVolume == m_curOpenOperation.prTarget) {
				// open co trade
				openCoPosition();
			}
		}
		else if (instrumentID == m_coInstrumentID) {
			// co traded
			m_curOpenOperation.coVolume += deltaVolume;
			if (m_curOpenOperation.coVolume == m_curOpenOperation.coTarget) {
				// finish
				m_holdPosition += m_curOpenOperation.groupCount;
				m_openOrdersPosition -= m_curOpenOperation.groupCount;
				m_curOpenOperation.reset();
			}
		}
		
		break;
	case TT_Close:
		if (instrumentID == m_instrumentID) {
			// pr traded
			m_curCloseOperation.prVolume += deltaVolume;
			if (m_curCloseOperation.prVolume == m_curCloseOperation.prTarget) {
				// open co trade
				openCoPosition();
			}
		}
		else if (instrumentID == m_coInstrumentID) {
			// co traded
			m_curCloseOperation.coVolume += deltaVolume;
			if (m_curCloseOperation.coVolume == m_curCloseOperation.coTarget) {
				// finish
				m_availablePosition += m_curCloseOperation.groupCount;
				m_closeOrdersPosition -= m_curCloseOperation.groupCount;
				m_curOpenOperation.reset();
			}
		}

		break;
	default:
		break;
	}
}

void APFuturesCombinationPosCtrl::onTradeCanceled(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTradeDirection direction)
{
	switch (type) {
	case TT_Open:
		if (instrumentID == m_instrumentID) {
			m_availablePosition += m_curOpenOperation.groupCount;
			m_openOrdersPosition -= m_curOpenOperation.groupCount;
			m_openOrderList.remove(orderID);
			m_curOpenOperation.reset();
		}
		break;
	case TT_Close:
		if (instrumentID == m_instrumentID) {
			m_holdPosition += m_curCloseOperation.groupCount;
			m_closeOrdersPosition -= m_curCloseOperation.groupCount;
			m_closeOrderList.remove(orderID);
			m_curCloseOperation.reset();
		}
		
		break;
	default:
		break;
	}
}

void APFuturesCombinationPosCtrl::openPrPosition()
{
	long vol = m_curOpenOperation.prTarget;
	double price = m_quotation->getCurPrice();
	open(m_instrumentID, m_directionType, OPT_AnyPrice, price);
}

void APFuturesCombinationPosCtrl::openCoPosition()
{
	long vol = m_curOpenOperation.coTarget;
	double price = m_coQuotation->getCurPrice();
	open(m_coInstrumentID, m_coTrend, OPT_AnyPrice, price);
}

void APFuturesCombinationPosCtrl::closePrPosition()
{
	long vol = m_curCloseOperation.prTarget;
	double price = m_quotation->getCurPrice();
	close(m_instrumentID, m_directionType, OPT_AnyPrice, price);
}

void APFuturesCombinationPosCtrl::closeCoPosition()
{
	long vol = m_curCloseOperation.coTarget;
	double price = m_coQuotation->getCurPrice();
	open(m_coInstrumentID, m_coTrend, OPT_AnyPrice, price);
}

//void APFuturesCombinationPosCtrl::onFinishOpenOperation()
//{
//	m_curOpenOperation.reset();
//}
//
//void APFuturesCombinationPosCtrl::onFinishCloseOperation()
//{
//	m_curCloseOperation.reset();
//}

