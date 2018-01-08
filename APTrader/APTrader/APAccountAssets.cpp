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
	m_accountID = "Default";
	m_interfaceType = "";
}

APAccountAssets::~APAccountAssets()
{
}

void APAccountAssets::init()
{
#ifdef USE_CTP
	m_accountID = APFuturesCTPTraderAgent::getInstance()->getUserID();
	m_interfaceType = "CTP";
#endif
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

	m_distributionQuque.push(pcw);

	if (m_instruments.find(instrumentID) != m_instruments.end()) {
		return;
	}
	m_instruments.insert(instrumentID);
}

void APAccountAssets::processVerification()
{
	while (m_verificationQueue.size() > 0) {
		const APPositionCtrlWrapper& pcw = m_verificationQueue.top();
		APPositionCtrl* posCtrl = pcw.posCtrl;
		if (posCtrl != NULL) {
			std::vector<APPositionData> holdPosList = posCtrl->getHoldPositionDetail();
			if (holdPosList.size() > 0) {
				//bool match = false;
				for (int i = 0; i < holdPosList.size(); i++) {
					const APPositionData& pd = holdPosList[i];
					if (pd.direction == TD_Buy) {
						verifyWithStub(m_buyPositionData, pd, posCtrl);
					}
					else if (pd.direction == TD_Sell) {
						verifyWithStub(m_sellPositionData, pd, posCtrl);
					}
				}
			}
		}
		m_verificationQueue.pop();
	}	
}

void APAccountAssets::verifyWithStub(std::map<APASSETID, APPositionDataStub>& stub, const APPositionData & pd, APPositionCtrl * posCtrl)
{
	if (posCtrl == NULL) {
		return;
	}

	if (stub.find(pd.instrumentID) != stub.end()) {
		if (stub[pd.instrumentID].remainPosition >= pd.holdPosition) {
			stub[pd.instrumentID].remainPosition -= pd.holdPosition;
		}
		else {
			// check order
			//posCtrl->setHoldAmount(pd.instrumentID, stub[pd.instrumentID].remainPosition);
			//stub[pd.instrumentID].remainPosition = 0;
		}
	}
}

void APAccountAssets::processDistribution()
{
	std::vector<APPositionCtrlWrapper> distributionList;
	while (m_distributionQuque.size() > 0) {
		APPositionCtrlWrapper pcw = m_distributionQuque.top();
		distributionList.push_back(pcw);
		m_distributionQuque.pop();
	}

	for (int i = 0; i < distributionList.size(); i++) {
		const APPositionCtrlWrapper& pcw = distributionList[i];
		if (pcw.direction == TD_Buy) {
			//
		}
		else if (pcw.direction == TD_Sell) {
			//
		}
	}
}

void APAccountAssets::process()
{
	processVerification();
	processDistribution();
	m_inited = true;
}

void APAccountAssets::beginVerify()
{
	std::set<APASSETID>::iterator it = m_instruments.begin();
	while (it != m_instruments.end()) {
		APASSETID id = *it;
#ifdef USE_CTP
		APFuturesCTPTraderAgent::getInstance()->reqQryInvestorPosition(id);
#endif
	}
}

std::string APAccountAssets::getAccountID()
{
	return m_accountID;
}

std::string APAccountAssets::getInterfaceType()
{
	return m_interfaceType;
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

	onGetPositionData(pd);
}
#endif

void APAccountAssets::onGetPositionData(APPositionData data)
{
	APPositionDataStub localData;
	localData.assign(data);
	localData.remainPosition = data.holdPosition;

	if (data.direction == TD_Buy) {
		m_buyPositionData[data.instrumentID] = localData;
	}
	else if (data.direction == TD_Sell) {
		m_sellPositionData[data.instrumentID] = localData;
	}

	checkFinish();
}

void APAccountAssets::checkFinish()
{
	int reqCounts = m_instruments.size();
	std::set<APASSETID> receivedPosData;
	std::map<APASSETID, APPositionDataStub>::iterator it;
	for (it = m_buyPositionData.begin(); it != m_buyPositionData.end(); it++) {
		receivedPosData.insert(it->first);
	}
	for (it = m_sellPositionData.begin(); it != m_sellPositionData.end(); it++) {
		receivedPosData.insert(it->first);
	}

	if (reqCounts == receivedPosData.size()) {
		//local verify processing
		process();
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
