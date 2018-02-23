#include "APFuturesIDSelector.h"
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
	m_exchangeInstrumentsList["SHFE"] = shfeInstrumentsSet;

	std::set<std::string> dceInstrumentsSet;
	dceInstrumentsSet.insert("rm");
	dceInstrumentsSet.insert("i");
	dceInstrumentsSet.insert("j");
	dceInstrumentsSet.insert("jm");
	m_exchangeInstrumentsList["DCE"] = dceInstrumentsSet;

	std::set<std::string> czceInstrumentsSet;
	czceInstrumentsSet.insert("FG");
	czceInstrumentsSet.insert("PTA");
	czceInstrumentsSet.insert("M");
	czceInstrumentsSet.insert("MA");
	m_exchangeInstrumentsList["CZCE"] = czceInstrumentsSet;
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
