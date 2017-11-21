#include "APFuturesIDSelector.h"

// use # as general symbol, eg. rb#1801 means, the earliest/nearest contract should be rb1801

APFuturesIDSelector::APFuturesIDSelector()
{
}


APFuturesIDSelector::~APFuturesIDSelector()
{
}

bool APFuturesIDSelector::isMultipleID(APASSETID futuresID) {
	std::string::size_type idx = futuresID.find("#");

	if (idx != std::string::npos) {
		return true;
	}

	return false;
}

APASSETID APFuturesIDSelector::getVarietyID(APASSETID futuresID) {
	std::string::size_type idx = futuresID.find("#");

	if (idx == std::string::npos) {
		return futuresID;
	}

	return futuresID.substr(0, idx);
}

std::string APFuturesIDSelector::getTimeSymbolInMultiID(APASSETID futuresID)
{
	std::string::size_type idx = futuresID.find("#");

	if (idx == std::string::npos) {
		return "";
	}

	return futuresID.substr(idx);
}

std::string APFuturesIDSelector::getTimeSymbolInFuturesID(APASSETID futuresID)
{
	return futuresID.substr(futuresID.length() - 4);
}
