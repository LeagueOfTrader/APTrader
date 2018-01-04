#include "APAccountAssets.h"
#include "Futures/APFuturesIDSelector.h"
#include "APGlobalConfig.h"
#include "Utils/APTimeUtility.h"
#include "APPositionCtrl.h"


#ifdef USE_CTP
#include "Impl/CTP/APFuturesCTPTraderAgent.h"
#endif // USE_CTP

#ifdef USE_CTP
APTradeDirection parseCTPDirection(TThostFtdcPosiDirectionType dt) {
	APTradeDirection dir = TD_Buy;
	switch (dt) {
	case THOST_FTDC_PD_Long:
		dir = TD_Buy;
		break;
	case THOST_FTDC_PD_Short:
		dir = TD_Sell;
		break;
	case THOST_FTDC_PD_Net:
		break;
	default:
		break;
	}

	return dir;
}
#endif


APAccountAssets::APAccountAssets()
{
	m_inited = false;
}

APAccountAssets::~APAccountAssets()
{
}

void APAccountAssets::init()
{
}

bool APAccountAssets::inited()
{
	return m_inited;
}

void APAccountAssets::verifyPosition(APASSETID instrumentID, APTradeDirection dir, APPositionCtrl * posCtrl)
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

void APAccountAssets::queryPosition(APASSETID instrumentID, APTradeDirection dir, APPositionCtrl * posCtrl)
{
	APPositionCtrlWrapper pcw;
	memset(&pcw, 0, sizeof(pcw));
	pcw.instrumentID = instrumentID;
	pcw.direction = dir;
	pcw.posCtrl = posCtrl;

	m_allocQueue.push(pcw);

	if (m_instruments.find(instrumentID) != m_instruments.end()) {
		return;
	}
	m_instruments.insert(instrumentID);
}

void APAccountAssets::verify()
{
	std::set<APASSETID>::iterator it = m_instruments.begin();
	while (it != m_instruments.end()) {
		APASSETID id = *it;
#ifdef USE_CTP
		APFuturesCTPTraderAgent::getInstance()->reqQryInvestorPosition(id);
#endif
	}
}

#ifdef USE_CTP
void APAccountAssets::onGetPositionData(CThostFtdcInvestorPositionField * pInvestorPosition)
{
	if (pInvestorPosition == NULL) {
		return;
	}

	APPositionData pd;
	memset(&pd, 0, sizeof(pd));
	pd.instrumentID = pInvestorPosition->InstrumentID;
	pd.direction = parseCTPDirection(pInvestorPosition->PosiDirection);
	pd.holdPosition = pInvestorPosition->Position;
	pd.frozenPosition = 0;
	pd.remainPosition = pd.holdPosition;

	onGetPositionData(pd);
}
#endif

void APAccountAssets::onGetPositionData(APPositionData data)
{
	if (data.direction == TD_Buy) {
		m_buyPositionData[data.instrumentID] = data;
	}
	else if (data.direction == TD_Sell) {
		m_sellPositionData[data.instrumentID] = data;
	}

	checkFinish();
}

void APAccountAssets::checkFinish()
{
	int reqCounts = m_instruments.size();
	std::set<APASSETID> receivedPosData;
	std::map<APASSETID, APPositionData>::iterator it;
	for (it = m_buyPositionData.begin(); it != m_buyPositionData.end(); it++) {
		receivedPosData.insert(it->first);
	}
	for (it = m_sellPositionData.begin(); it != m_sellPositionData.end(); it++) {
		receivedPosData.insert(it->first);
	}

	if (reqCounts == receivedPosData.size()) {
		m_inited = true;
		//local verify end
	}
}

//bool APAccountAssets::getPositionData(APASSETID instrumentID, APTradeDirection direction, APPositionData & data)
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
