#include "APFuturesIDSelector.h"
#include "../Utils/APTimeUtility.h"

//
//// use # as general symbol, eg. rb#1801 means, the earliest/nearest contract should be rb1801
//
//
//bool APFuturesIDSelector::isMultipleID(APASSETID futuresID) {
//	std::string::size_type idx = futuresID.find("#");
//
//	if (idx != std::string::npos) {
//		return true;
//	}
//
//	return false;
//}
//
//APASSETID APFuturesIDSelector::getVarietyID(APASSETID futuresID) {
//	std::string::size_type idx = futuresID.find("#");
//
//	if (idx == std::string::npos) {
//		return futuresID;
//	}
//
//	return futuresID.substr(0, idx);
//}
//
//std::string APFuturesIDSelector::getTimeSymbolInMultiID(APASSETID futuresID)
//{
//	std::string::size_type idx = futuresID.find("#");
//
//	if (idx == std::string::npos) {
//		return "";
//	}
//
//	return futuresID.substr(idx);
//}

std::map<std::string, std::set<std::string>> APFuturesIDSelector::m_exchangeInstrumentsList;
std::set<std::string> APFuturesIDSelector::m_nonferrousMetalList;
std::set<std::string> APFuturesIDSelector::m_nobleMetalList;
std::map<std::string, int> APFuturesIDSelector::m_minPriceUnit;

std::string APFuturesIDSelector::getTimeSymbol(APASSETID instrumentID)
{
	return instrumentID.substr(instrumentID.length() - 4);
}

std::string APFuturesIDSelector::getTypeID(APASSETID instrumentID)
{
	return instrumentID.substr(0, instrumentID.length() - 4);
}

void APFuturesIDSelector::init()
{
	m_exchangeInstrumentsList.clear();

	std::set<std::string> shfeInstrumentsSet;
	shfeInstrumentsSet.insert("au");
	shfeInstrumentsSet.insert("ag");
	shfeInstrumentsSet.insert("rb");
	shfeInstrumentsSet.insert("hc");
	shfeInstrumentsSet.insert("bu");
	shfeInstrumentsSet.insert("ru");
	shfeInstrumentsSet.insert("cu");
	shfeInstrumentsSet.insert("al");
	shfeInstrumentsSet.insert("zn");
	shfeInstrumentsSet.insert("ni");
	shfeInstrumentsSet.insert("pb");
	shfeInstrumentsSet.insert("sn");
	shfeInstrumentsSet.insert("jd");
	m_exchangeInstrumentsList["SHFE"] = shfeInstrumentsSet;

	std::set<std::string> dceInstrumentsSet;
	dceInstrumentsSet.insert("a");
	dceInstrumentsSet.insert("pp");
	dceInstrumentsSet.insert("rm");
	dceInstrumentsSet.insert("i");
	dceInstrumentsSet.insert("j");
	dceInstrumentsSet.insert("jm");
	dceInstrumentsSet.insert("l");
	m_exchangeInstrumentsList["DCE"] = dceInstrumentsSet;

	std::set<std::string> czceInstrumentsSet;
	czceInstrumentsSet.insert("FG");
	czceInstrumentsSet.insert("AP");
	czceInstrumentsSet.insert("CF");
	czceInstrumentsSet.insert("SF");
	czceInstrumentsSet.insert("SM");
	czceInstrumentsSet.insert("TA");
	czceInstrumentsSet.insert("M");
	czceInstrumentsSet.insert("MA");
	czceInstrumentsSet.insert("SR");
	czceInstrumentsSet.insert("CF");
	czceInstrumentsSet.insert("ZC");
	czceInstrumentsSet.insert("OI");
	m_exchangeInstrumentsList["CZCE"] = czceInstrumentsSet;

	// category
	m_nonferrousMetalList.clear();
	m_nobleMetalList.clear();

	m_nonferrousMetalList.insert("cu");
	m_nonferrousMetalList.insert("al");
	m_nonferrousMetalList.insert("zn");
	m_nonferrousMetalList.insert("ni");
	m_nonferrousMetalList.insert("pb");
	m_nonferrousMetalList.insert("sn");

	m_nobleMetalList.insert("ag");
	m_nobleMetalList.insert("au");

	// min unit
	m_minPriceUnit["au"] = 1;
	m_minPriceUnit["ag"] = 1;
	m_minPriceUnit["jd"] = 1;
	m_minPriceUnit["rb"] = 1;
	m_minPriceUnit["hc"] = 1;
	m_minPriceUnit["bu"] = 2;
	m_minPriceUnit["ru"] = 5;
	m_minPriceUnit["cu"] = 10;
	m_minPriceUnit["al"] = 5;
	m_minPriceUnit["zn"] = 5;
	m_minPriceUnit["ni"] = 10;
	m_minPriceUnit["pb"] = 1;
	m_minPriceUnit["sn"] = 1;
	m_minPriceUnit["a"] = 1;
	m_minPriceUnit["pp"] = 1;
	m_minPriceUnit["rm"] = 1;
	m_minPriceUnit["i"] = 0.5;
	m_minPriceUnit["j"] = 0.5;
	m_minPriceUnit["jm"] = 0.5;
	m_minPriceUnit["FG"] = 1;
	m_minPriceUnit["TA"] = 2;
	m_minPriceUnit["M"] = 1;
	m_minPriceUnit["MA"] = 1;
	m_minPriceUnit["SR"] = 1;
	m_minPriceUnit["CF"] = 1;
	m_minPriceUnit["ZC"] = 1;
	m_minPriceUnit["AP"] = 1;
	m_minPriceUnit["CF"] = 1;
	m_minPriceUnit["OI"] = 1;
	m_minPriceUnit["SF"] = 1;
	m_minPriceUnit["SM"] = 1;
}

std::string APFuturesIDSelector::getDomesticExchangeID(APASSETID instrumentID)
{
	std::string exchangeID = "";
	std::map<std::string, std::set<std::string>>::iterator it;
	std::string typeID = getTypeID(instrumentID);
	for (it = m_exchangeInstrumentsList.begin(); it != m_exchangeInstrumentsList.end(); it++) {
		std::set<std::string>& instrumentsSet = it->second;
		if (instrumentsSet.find(typeID) != instrumentsSet.end()) {
			exchangeID = it->first;
			break;
		}
	}

	return exchangeID;
}

bool APFuturesIDSelector::isInstrumentInTradeTime(APASSETID instrumentID, std::string dateTime)
{
	UINT hour = APTimeUtility::getHourInDateTime(dateTime);
	UINT minute = APTimeUtility::getMinuteInDateTime(dateTime);
	UINT second = APTimeUtility::getSecondInDateTime(dateTime);
	if (hour >= 3 && hour < 9) {
		return false;
	}

	if (hour == 11 && minute > 30) {
		return false;
	}

	if (hour > 12 && hour < 13) {
		return false;
	}

	if (hour == 13 && minute <= 30) {
		return false;
	}

	if (hour >= 15 && hour < 21) {
		return false;
	}

	std::string exchangeID = getDomesticExchangeID(instrumentID);
	if (exchangeID == "CZCE" || exchangeID == "DCE") {
		if (hour < 3) {
			return false;
		}

		if (hour == 23 && minute >= 30) {
			return false;
		}
	}
	else {
		std::string typeID = getTypeID(instrumentID);
		if (m_nonferrousMetalList.find(typeID) != m_nonferrousMetalList.end()) {
			if (hour >= 1) {
				return false;
			}
		}
		else if (m_nobleMetalList.find(typeID) != m_nobleMetalList.end()) {
			if (hour == 2 && minute >= 30) {
				return false;
			}
		}
		else if (hour >= 23 || hour < 3) {
			return false;
		}
	}

	return true;
}

double APFuturesIDSelector::getMinPriceUnit(APASSETID instrumentID)
{
	std::string typeID = getTypeID(instrumentID);
	if (m_minPriceUnit.find(typeID) != m_minPriceUnit.end()) {
		return m_minPriceUnit[typeID];
	}

	return 0.0;
}
