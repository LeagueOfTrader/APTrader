#pragma once
#include <string>
#include "../APTypes.h"

class APFuturesIDSelector
{
public:
	APFuturesIDSelector();
	~APFuturesIDSelector();

	static bool isMultipleID(APASSETID futuresID);
	static APASSETID getVarietyID(APASSETID futuresID);
	static std::string getTimeSymbolInMultiID(APASSETID futuresID);
	static std::string getTimeSymbolInFuturesID(APASSETID futuresID);
};

