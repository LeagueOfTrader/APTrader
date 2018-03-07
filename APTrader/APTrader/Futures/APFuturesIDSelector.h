#pragma once
#include <string>
#include <map>
#include <set>
#include "../APTypes.h"

class APFuturesIDSelector
{
public:

	//static bool isMultipleID(APASSETID futuresID);
	//static APASSETID getVarietyID(APASSETID futuresID);
	//static std::string getTimeSymbolInMultiID(APASSETID futuresID);

	static std::string getTimeSymbol(APASSETID futuresID);
	static std::string getTypeID(APASSETID instrumentID);

	static void init();

	static std::string getDomesticExchangeID(APASSETID instrumentID);

	static bool isInstrumentInTradeTime(APASSETID instrumentID, std::string dateTime);

protected:
	static std::map<std::string, std::set<std::string>> m_exchangeInstrumentsList;
	static std::set<std::string> m_nonferrousMetalList;
	static std::set<std::string> m_nobleMetalList;
};

