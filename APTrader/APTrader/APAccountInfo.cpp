#include "APAccountInfo.h"
#include "Futures/APFuturesIDSelector.h"
#include "APGlobalConfig.h"
#include "Utils/APTimeUtility.h"
#include "APPositionCtrl.h"
#include "APTradeManager.h"
#include "APPositionRepertory.h"


#ifdef USE_CTP
#include "Impl/CTP/APFuturesCTPTraderAgent.h"
#endif // USE_CTP

#ifdef USE_CTP
const long POSITION_QUERY_INTERVAL = 1000;
#endif

APAccountInfo::APAccountInfo()
{
	m_inited = false;
	m_accountID = "Default";
	m_interfaceType = "";
}

APAccountInfo::~APAccountInfo()
{
}

void APAccountInfo::init()
{
#ifdef USE_CTP
	m_accountID = APFuturesCTPTraderAgent::getInstance()->getUserID();
	m_interfaceType = "CTP";
#endif
}

bool APAccountInfo::inited()
{
	return m_inited;
}

void APAccountInfo::update()
{
#ifdef USE_CTP
#endif
}

void APAccountInfo::verifyPosition(APASSETID instrumentID, APTradeDirection dir, APPositionCtrl * posCtrl)
{
	APPositionCtrlWrapper pcw;
	memset(&pcw, 0, sizeof(pcw));
	pcw.instrumentID = instrumentID;
	pcw.direction = dir;
	pcw.posCtrl = posCtrl;

	m_verificationQueue.push(pcw);

	if (m_instruments.find(instrumentID) != m_instruments.end()) {
		return;
	}
	m_instruments.insert(instrumentID);
}

void APAccountInfo::processVerification()
{
	while (m_verificationQueue.size() > 0) {
		const APPositionCtrlWrapper& pcw = m_verificationQueue.top();
		APPositionCtrl* posCtrl = pcw.posCtrl;
		if (posCtrl != NULL) {
			std::vector<APPositionData> holdPosList = posCtrl->getHoldPositionDetail();
			if (holdPosList.size() > 0) {
				bool match = false;
				for (int i = 0; i < holdPosList.size(); i++) {
					const APPositionData& pd = holdPosList[i];
					match = APPositionRepertory::getInstance()->capable(pd);
					if (!match) {
						break;
					}
				}

				if (match) {
					for (int i = 0; i < holdPosList.size(); i++) {
						const APPositionData& pd = holdPosList[i];
						APPositionRepertory::getInstance()->handle(pd);
					}
				}
				else
				{
					// correct position
					if (APGlobalConfig::getInstance()->isAutoCorrectPosition()) {
						posCtrl->correctPosition();
					}
				}
			}
		}
		m_verificationQueue.pop();
	}	

	APPositionRepertory::getInstance()->deredundance();
}

//void APAccountInfo::verify(std::map<APASSETID, APPositionData>& data, const APPositionData & pd, APPositionCtrl * posCtrl)
//{
//	if (posCtrl == NULL) {
//		return;
//	}
//
//	if (data.find(pd.instrumentID) != data.end()) {
//		if (data[pd.instrumentID].capable(pd)) {
//			data[pd.instrumentID].handle(pd);
//		}
//		else {
//			// position ctrl correction
//			//posCtrl->
//		}
//	}
//}

void APAccountInfo::beginVerify()
{
	std::set<APASSETID>::iterator it = m_instruments.begin();
	while (it != m_instruments.end()) {
		APASSETID id = *it;
#ifdef USE_CTP
		APFuturesCTPTraderAgent::getInstance()->reqQryInvestorPosition(id);
#endif
	}
}

std::string APAccountInfo::getAccountID()
{
	return m_accountID;
}

std::string APAccountInfo::getInterfaceType()
{
	return m_interfaceType;
}

#ifdef USE_CTP

void APAccountInfo::appendPositionInfo(APPositionData & pd, CThostFtdcInvestorPositionField & info)
{
	pd.holdPosition += info.Position;
	pd.longFrozenPosition += info.LongFrozen;
	pd.shortFrozenPosition += info.ShortFrozen;
	pd.todayPosition += info.TodayPosition;
	pd.yesterdayPosition += info.YdPosition;
}

void APAccountInfo::onGetPositionData(APASSETID instrumentID, CThostFtdcInvestorPositionField& posData)
{
	APPositionData posDataBuy;
	memset(&posDataBuy, 0, sizeof(posDataBuy));
	posDataBuy.direction = TD_Buy;
	posDataBuy.instrumentID = instrumentID;
	APPositionData posDataSell;
	memset(&posDataSell, 0, sizeof(posDataSell));
	posDataSell.direction = TD_Sell;
	posDataSell.instrumentID = instrumentID;

	if (posData.PosiDirection == THOST_FTDC_PD_Long) {
		appendPositionInfo(posDataBuy, posData);
	}
	else if (posData.PosiDirection == THOST_FTDC_PD_Short) {
		appendPositionInfo(posDataSell, posData);
	}	

	if (posDataBuy.holdPosition > 0) {
		onGetPositionData(posDataBuy);
	}

	if (posDataSell.holdPosition > 0) {
		onGetPositionData(posDataSell);
	}

	//verifyAfterCheck();
}

void APAccountInfo::onSyncPositionData()
{
	if (!m_inited) {
		//
	}
}

#endif

void APAccountInfo::onGetPositionData(APPositionData data)
{
	APPositionRepertory::getInstance()->store(data);
}

//void APAccountInfo::verifyAfterCheck()
//{
//	bool positionDataComplete = false;
//	int reqCounts = m_instruments.size();	
//
//	if (reqCounts == APPositionRepertory::getInstance()->getPositionDataInstrumentsCount()) {
//		positionDataComplete = true;		
//	}
//
//	bool orderDataComplete = false;
//	if (APTradeManager::getInstance()->inited()) {
//		APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
//		if (trader != NULL) {
//			orderDataComplete = trader->isOrderDataComplete();
//		}
//	}
//	
//	if (orderDataComplete && positionDataComplete) {
//		processVerification();
//		m_inited = true;
//	}
//}

//bool APAccountInfo::getPositionData(APASSETID instrumentID, APTradeDirection direction, APPositionData & data)
//{
//	bool ret = false;
//
//	bool isMultiID = false;
//	switch (APGlobalConfig::getInstance()->getInstrumentType()) {
//		case FCT_Futrues:
//			isMultiID = APFuturesIDSelector::isMultipleID(instrumentID);
//			break;
//		case FCT_Shares:
//			break;
//		case FCT_Options:
//			break;
//		default:
//			break;
//	}
//
//	if (!isMultiID) {
//		std::map<APASSETID, APPositionData>::iterator it;
//
//		if (direction == TD_Buy) {
//			it = m_longPositionData.find(instrumentID);
//
//			if (it != m_longPositionData.end()) {
//				data = it->second;
//				ret = true;
//			}
//		}
//		else if (direction == TD_Sell) {
//			it = m_shortPositionData.find(instrumentID);
//
//			if (it != m_shortPositionData.end()) {
//				data = it->second;
//				ret = true;
//			}
//		}
//	}
//	else {
//		std::map<APASSETID, APPositionData>& positionData = m_longPositionData;
//		if (direction == TD_Buy) {
//			positionData = m_longPositionData;
//		}
//		else {
//			positionData = m_shortPositionData;
//		}
//
//		data.id = instrumentID;
//		data.type = direction;
//
//		std::string timeSymbol = APFuturesIDSelector::getTimeSymbolInMultiID(instrumentID);
//		std::string varietyID = APFuturesIDSelector::getVarietyID(instrumentID);
//
//		std::map<APASSETID, APPositionData>::iterator it;
//		for (it = positionData.begin(); it != positionData.end(); it++) {
//			std::string holdID = it->first;
//			if (holdID.find(varietyID) != std::string::npos) {
//				std::string futuresTimeSymbol = APFuturesIDSelector::getTimeSymbolInFuturesID(instrumentID);
//				if (APTimeUtility::compareFuturesYearMonth(timeSymbol, futuresTimeSymbol) <= 0) {
//					data.append(it->second);
//					ret = true;
//				}
//			}
//		}
//	}
//
//	return ret;
//}

bool APPositionCtrlWrapperComparer::operator()(APPositionCtrlWrapper p0, APPositionCtrlWrapper p1) const
{
	if (p0.posCtrl == NULL || p1.posCtrl == NULL) {
		return false;
	}

	return p0.posCtrl->getPriority() >= p1.posCtrl->getPriority();

}
