#pragma once
#include "Common/Singleton.h"
#include <string>
#include <map>
#include "APDef.h"
#include "APTypes.h"

struct APPositionData {
	std::string id = "";
	APTrendType type = TT_Num;
	long holdPosition = 0;
	long frozenPosition = 0;
	long openOrdersPosition = 0;
	long closeOrdersPosition = 0;

	void append(const APPositionData& data) {
		holdPosition += data.holdPosition;
		frozenPosition += data.frozenPosition;
		openOrdersPosition += data.openOrdersPosition;
		closeOrdersPosition += data.closeOrdersPosition;
	}
};

class APAccountAssets : public Singleton<APAccountAssets>
{
public:
	APAccountAssets();
	~APAccountAssets();

	virtual void init();

	bool getPositionData(APASSETID commodityID, APTrendType type, APPositionData& positionData);	

private:
	std::map<APASSETID, APPositionData> m_longPositionData;
	std::map<APASSETID, APPositionData> m_shortPositionData;
};

