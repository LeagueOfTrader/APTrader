#pragma once
#include <map>
#include "APAccountInfo.h"

#include "APStructs.h"


class APPositionRepertory : public Singleton<APPositionRepertory>
{
public:
	APPositionRepertory();
	~APPositionRepertory();

	APPositionData& getPositionData(APASSETID instrumentID, APTradeDirection direction);
	bool capable(const APPositionData& pd);
	void handle(const APPositionData& pd);

	void store(APPositionData data);
	void remove(APASSETID instrumentID, APTradeDirection direction);

	int getPositionDataInstrumentsCount();

	void deredundance();

private:
	bool capable(std::map<APASSETID, APPositionData>& data, const APPositionData& pd);
	void handle(std::map<APASSETID, APPositionData>& data, const APPositionData& pd);

private:
	std::map<APASSETID, APPositionData> m_buyPositionData;
	std::map<APASSETID, APPositionData> m_sellPositionData;
};

