#include "APAccountAssets.h"
#include "Futures/APFuturesIDSelector.h"
#include "APGlobalConfig.h"
#include "Utils/APTimeUtility.h"

APAccountAssets::APAccountAssets()
{
}

APAccountAssets::~APAccountAssets()
{
}

void APAccountAssets::init()
{
}

bool APAccountAssets::getPositionData(APASSETID instrumentID, APTrendType trend, APPositionData & data)
{
	bool ret = false;

	bool isMultiID = false;
	switch (APGlobalConfig::getInstance()->getInstrumentType()) {
		case FCT_Futrues:
			isMultiID = APFuturesIDSelector::isMultipleID(instrumentID);
			break;
		case FCT_Shares:
			break;
		case FCT_Options:
			break;
		default:
			break;
	}

	if (!isMultiID) {
		std::map<APASSETID, APPositionData>::iterator it;

		if (trend == TT_Long) {
			it = m_longPositionData.find(instrumentID);

			if (it != m_longPositionData.end()) {
				data = it->second;
				ret = true;
			}
		}
		else if (trend == TT_Short) {
			it = m_shortPositionData.find(instrumentID);

			if (it != m_shortPositionData.end()) {
				data = it->second;
				ret = true;
			}
		}
	}
	else {
		std::map<APASSETID, APPositionData>& positionData = m_longPositionData;
		if (trend == TT_Long) {
			positionData = m_longPositionData;
		}
		else {
			positionData = m_shortPositionData;
		}

		data.id = instrumentID;
		data.type = trend;

		std::string timeSymbol = APFuturesIDSelector::getTimeSymbolInMultiID(instrumentID);
		std::string varietyID = APFuturesIDSelector::getVarietyID(instrumentID);

		std::map<APASSETID, APPositionData>::iterator it;
		for (it = positionData.begin(); it != positionData.end(); it++) {
			std::string holdID = it->first;
			if (holdID.find(varietyID) != std::string::npos) {
				std::string futuresTimeSymbol = APFuturesIDSelector::getTimeSymbolInFuturesID(instrumentID);
				if (APTimeUtility::compareFuturesYearMonth(timeSymbol, futuresTimeSymbol) <= 0) {
					data.append(it->second);
					ret = true;
				}
			}
		}
	}

	return ret;
}

