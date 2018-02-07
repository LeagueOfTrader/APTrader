#include "APFuturesCombinationPosCtrl.h"
#include "../APMacro.h"
#include "../Utils/APJsonReader.h"
#include "../APMarketDataManager.h"
#include "../APInstrumentQuotation.h"
#include "../APPositionRepertory.h"

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
	std::string strCoDir = jr.getStrValue("CoDirection");
	if (strCoDir == "Buy") {
		m_coDirectionType = TD_Buy;
	}
	else if (strCoDir == "Sell") {
		m_coDirectionType = TD_Sell;
	}

#ifndef SIM
	m_coQuotation = APMarketDataMgr->subscribeInstrument(m_instrumentID);
#else 
	m_coQuotation = NULL;
#endif
}

std::vector<APPositionData> APFuturesCombinationPosCtrl::getHoldPositionDetail()
{
	std::vector<APPositionData> holdPosList;
	APPositionData pd;
	//memset(&pd, 0, sizeof(pd));
	pd.clear();
	pd.instrumentID = m_instrumentID;
	pd.direction = m_directionType;
	pd.holdPosition = m_prVolume; // + m_curOpenOperation.prVolume - m_curCloseOperation.prVolume;
	if (m_directionType == TD_Buy) {
		pd.longFrozenPosition = m_curCloseOperation.prTarget - m_curCloseOperation.prVolume;
	}
	else if (m_directionType == TD_Sell) {
		pd.shortFrozenPosition = m_curCloseOperation.prTarget - m_curCloseOperation.prVolume;
	}
	holdPosList.push_back(pd);

	//memset(&pd, 0, sizeof(pd));
	pd.clear();
	pd.instrumentID = m_coInstrumentID;
	pd.direction = m_coDirectionType;
	pd.holdPosition = m_coVolume; // + m_curOpenOperation.coVolume - m_curCloseOperation.coVolume;
	if (m_directionType == TD_Buy) {
		pd.longFrozenPosition = m_curCloseOperation.coTarget - m_curCloseOperation.coVolume;
	}
	else if (m_directionType == TD_Sell) {
		pd.shortFrozenPosition = m_curCloseOperation.coTarget - m_curCloseOperation.coVolume;
	}
	holdPosList.push_back(pd);
	return holdPosList;
}

void APFuturesCombinationPosCtrl::setHoldAmount(APASSETID instrumentID, long amount) 
{
	if (instrumentID == m_instrumentID) {
		m_prVolume = amount;
	}
	else if (instrumentID == m_coInstrumentID) {
		m_coVolume = amount;
	}
}

std::pair<std::vector<APASSETID>, std::vector<long>> APFuturesCombinationPosCtrl::getCombinationUnitVol()
{	
	std::vector<APASSETID> instruments;
	instruments.push_back(m_instrumentID);
	instruments.push_back(m_coInstrumentID);
	std::vector<long> unitVols;
	unitVols.push_back(m_prUnitVol);
	unitVols.push_back(m_coUnitVol);
	std::pair<std::vector<APASSETID>, std::vector<long>> combinationUnitVol = std::make_pair(instruments, unitVols);
	return combinationUnitVol;
}

void APFuturesCombinationPosCtrl::openPosition(long volume)
{
	if (m_curOpenOperation.hasTarget() 
		|| m_curCloseOperation.hasTarget()) {
		return;
	}

	m_curOpenOperation.setTarget(volume * m_prUnitVol, volume * m_coUnitVol, volume);
	openPrPosition();

	m_positionMutex.lock();
	m_availablePosition -= volume;
	m_openOrdersPosition += volume;
	m_positionMutex.unlock();
}

void APFuturesCombinationPosCtrl::closePosition(long volume)
{
	if (m_curCloseOperation.hasTarget() 
		|| m_curOpenOperation.hasTarget()) {
		return;
	}

	m_curCloseOperation.setTarget(volume * m_prUnitVol, volume * m_coUnitVol, volume);
	closePrPosition();

	m_positionMutex.lock();
	m_holdPosition -= volume;
	m_closeOrdersPosition += volume;
	m_positionMutex.unlock();
}

void APFuturesCombinationPosCtrl::openFullPosition()
{
	openPosition( m_availablePosition);	
}

void APFuturesCombinationPosCtrl::closeOffPosition()
{
	closePosition(m_holdPosition);
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
		m_positionMutex.lock();
		if (instrumentID == m_instrumentID) {
			// pr traded
			m_prVolume += deltaVolume;
			m_curOpenOperation.prVolume += deltaVolume;
			if (m_curOpenOperation.prVolume == m_curOpenOperation.prTarget) {
				// open co trade
				openCoPosition();
			}
		}
		else if (instrumentID == m_coInstrumentID) {
			// co traded
			m_coVolume += deltaVolume;
			m_curOpenOperation.coVolume += deltaVolume;
			if (m_curOpenOperation.coVolume == m_curOpenOperation.coTarget) {
				// finish
				m_holdPosition += m_curOpenOperation.groupCount;
				m_availablePosition += m_curOpenOperation.groupCount;
				m_openOrdersPosition -= m_curOpenOperation.groupCount;
				m_curOpenOperation.reset();
			}			
		}
		m_positionMutex.unlock();
		break;
	case TT_Close:
		m_positionMutex.lock();
		if (instrumentID == m_instrumentID) {
			// pr traded
			m_prVolume -= deltaVolume;
			m_curCloseOperation.prVolume += deltaVolume;
			if (m_curCloseOperation.prVolume == m_curCloseOperation.prTarget) {
				// open co trade
				openCoPosition();
			}
		}
		else if (instrumentID == m_coInstrumentID) {
			// co traded
			m_coVolume -= deltaVolume;
			m_curCloseOperation.coVolume += deltaVolume;
			if (m_curCloseOperation.coVolume == m_curCloseOperation.coTarget) {
				// finish
				m_holdPosition -= m_curCloseOperation.groupCount;
				m_availablePosition -= m_curCloseOperation.groupCount;

				m_closeOrdersPosition -= m_curCloseOperation.groupCount;
				m_curOpenOperation.reset();
			}
		}
		m_positionMutex.unlock();
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
			m_positionMutex.lock();
			m_availablePosition += m_curOpenOperation.groupCount;
			m_openOrdersPosition -= m_curOpenOperation.groupCount;
			m_openOrderList.remove(orderID);
			m_curOpenOperation.reset();
			m_positionMutex.unlock();
		}
		break;
	case TT_Close:
		if (instrumentID == m_instrumentID) {
			m_positionMutex.lock();
			m_marginPosition += m_curCloseOperation.groupCount;
			m_closeOrdersPosition -= m_curCloseOperation.groupCount;
			m_closeOrderList.remove(orderID);
			m_curCloseOperation.reset();
			m_positionMutex.unlock();
		}
		
		break;
	default:
		break;
	}
}

void APFuturesCombinationPosCtrl::correctPosition()
{
	APPositionData& prPosData = APPositionRepertory::getInstance()->getPositionData(m_instrumentID, m_directionType);
	APPositionData& coPosData = APPositionRepertory::getInstance()->getPositionData(m_coInstrumentID, m_coDirectionType);

	if (m_prVolume > prPosData.holdPosition) {
		long prHold = m_holdPosition * m_prUnitVol;
		prPosData.holdPosition -= prHold;
		long closedPrVolume = 0;
		long openedPrVolume = 0;
		if (m_curCloseOperation.hasTarget()) {
			closedPrVolume = m_curCloseOperation.prVolume;
			prPosData.holdPosition += closedPrVolume;
		}
		if (prPosData.holdPosition > 0) {
			if (m_curOpenOperation.hasTarget()) {
				m_curOpenOperation.prVolume = std::min(m_curOpenOperation.prVolume, prPosData.holdPosition);		
				openedPrVolume = m_curOpenOperation.prVolume;
				prPosData.holdPosition = 0;
			}
		}
		m_prVolume = m_curOpenOperation.prVolume + prHold;
	}

	if (m_coVolume > coPosData.holdPosition) {
		long coHold = m_holdPosition * m_coUnitVol;
		coPosData.holdPosition -= coHold;
		long closedCoVolume = 0;
		long openedCoVolume = 0;
		if (m_curCloseOperation.hasTarget()) {
			closedCoVolume = m_curCloseOperation.coVolume;
			coPosData.holdPosition += closedCoVolume;
		}
		if (coPosData.holdPosition > 0) {
			if (m_curOpenOperation.hasTarget()) {
				m_curOpenOperation.coVolume = std::min(m_curOpenOperation.coVolume, coPosData.holdPosition);
				openedCoVolume = m_curOpenOperation.coVolume;
				coPosData.holdPosition = 0;
			}
		}
		m_coVolume = m_curOpenOperation.coVolume + coHold;
	}
}

void APFuturesCombinationPosCtrl::modifyOrdersPosition(const APTradeOrderInfo& info)
{
	APASSETID instrumentID = info.instrumentID;
	APTradeDirection direction = info.direction;
	APTradeType tradeType = info.type;

	if (m_instrumentID == instrumentID && m_directionType == direction) {
		if (tradeType == TT_Open) {
			m_curOpenOperation.prTarget = info.volume;
			m_curOpenOperation.prVolume = info.volumeTraded;
		}
		else {
			m_curCloseOperation.prTarget = info.volume;
			m_curCloseOperation.prVolume = info.volumeTraded;
		}
	}
	else if (m_coInstrumentID == instrumentID && m_coDirectionType == direction) {
		if (tradeType == TT_Open) {
			m_curOpenOperation.coTarget = info.volume;
			m_curOpenOperation.coVolume = info.volumeTraded;
		}
		else {
			m_curCloseOperation.coTarget = info.volume;
			m_curCloseOperation.coVolume = info.volumeTraded;
		}
	}
}

Json::Value APFuturesCombinationPosCtrl::serializeToJsonValue()
{
	Json::Value v = APPositionCtrl::serializeToJsonValue();

	v["PrVolume"] = m_prVolume;
	v["CoVolume"] = m_coVolume;

	v["OpenOperation"] = serializeOperationData(m_curOpenOperation);
	v["CloseOperation"] = serializeOperationData(m_curCloseOperation);

	return v;
}

void APFuturesCombinationPosCtrl::deserialize(std::string str)
{
	APPositionCtrl::deserialize(str);
	Json::Value v;
	Json::Reader r;
	r.parse(str, v);

	m_prVolume = v["PrVolume"].asUInt();
	m_coVolume = v["CoVlume"].asUInt();

	deserializeOperationData(v["OpenOperation"], m_curOpenOperation);
	deserializeOperationData(v["CloseOperation"], m_curCloseOperation);
}

void APFuturesCombinationPosCtrl::openPrPosition()
{
	long vol = m_curOpenOperation.prTarget;
	double price = m_quotation->getCurPrice();
	open(m_instrumentID, m_directionType, price, vol);
}

void APFuturesCombinationPosCtrl::openCoPosition()
{
	long vol = m_curOpenOperation.coTarget;
	double price = m_coQuotation->getCurPrice();
	open(m_coInstrumentID, m_coDirectionType, price, vol);
}

void APFuturesCombinationPosCtrl::closePrPosition()
{
	long vol = m_curCloseOperation.prTarget;
	double price = m_quotation->getCurPrice();
	close(m_instrumentID, m_directionType, price, vol);
}

void APFuturesCombinationPosCtrl::closeCoPosition()
{
	long vol = m_curCloseOperation.coTarget;
	double price = m_coQuotation->getCurPrice();
	open(m_coInstrumentID, m_coDirectionType, price, vol);
}

Json::Value APFuturesCombinationPosCtrl::serializeOperationData(APCombinationOperationData & data)
{
	Json::Value v;

	v["PrVolume"] = data.prVolume;
	v["PrTarget"] = data.prTarget;
	v["CoVolume"] = data.coVolume;
	v["CoTarget"] = data.coTarget;
	v["GroupCount"] = data.groupCount;

	return v;
}

void APFuturesCombinationPosCtrl::deserializeOperationData(Json::Value& v, APCombinationOperationData & data)
{
	data.prVolume = v["PrVolume"].asUInt();
	data.prTarget = v["PrTarget"].asUInt();
	data.coVolume = v["CoVolume"].asUInt();
	data.coTarget = v["CoTarget"].asUInt();
	data.groupCount = v["GroupCount"].asUInt();
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

