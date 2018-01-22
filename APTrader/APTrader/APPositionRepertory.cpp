#include "APPositionRepertory.h"



APPositionRepertory::APPositionRepertory()
{
}


APPositionRepertory::~APPositionRepertory()
{
}

APPositionData & APPositionRepertory::getPositionData(APASSETID instrumentID, APTradeDirection direction)
{
	if (direction == TD_Buy) {
		return m_buyPositionData[instrumentID];
	}
	else if (direction == TD_Sell) {
		return m_sellPositionData[instrumentID];
	}
}

bool APPositionRepertory::capable(const APPositionData & pd)
{
	if (pd.direction == TD_Buy) {
		return capable(m_buyPositionData, pd);
	}
	else {
		return capable(m_sellPositionData, pd);
	}
}

void APPositionRepertory::handle(const APPositionData & pd)
{
	if (pd.direction == TD_Buy) {
		handle(m_buyPositionData, pd);
	}
	else {
		handle(m_sellPositionData, pd);
	}
}

void APPositionRepertory::store(APPositionData data)
{
	if (data.direction == TD_Buy) {
		m_buyPositionData[data.instrumentID] = data;
	}
	else if (data.direction == TD_Sell) {
		m_sellPositionData[data.instrumentID] = data;
	}
}

void APPositionRepertory::remove(APASSETID instrumentID, APTradeDirection direction)
{
	if (direction == TD_Buy) {
		m_buyPositionData.erase(instrumentID);
	}
	else if (direction == TD_Sell) {
		m_sellPositionData.erase(instrumentID);
	}
}

int APPositionRepertory::getPositionDataInstrumentsCount()
{
	std::set<APASSETID> receivedPosData;
	std::map<APASSETID, APPositionData>::iterator it;
	for (it = m_buyPositionData.begin(); it != m_buyPositionData.end(); it++) {
		receivedPosData.insert(it->first);
	}
	for (it = m_sellPositionData.begin(); it != m_sellPositionData.end(); it++) {
		receivedPosData.insert(it->first);
	}

	return receivedPosData.size();
}

void APPositionRepertory::deredundance()
{
	std::map<APASSETID, APPositionData>::iterator it;
	for (it = m_buyPositionData.begin(); it != m_buyPositionData.end(); ) {
		APPositionData& data = it->second;
		if (data.holdPosition == 0 && data.longFrozenPosition == 0 && data.shortFrozenPosition == 0) {
			it = m_buyPositionData.erase(it);
		}
		else {
			it++;
		}
	}

	for (it = m_sellPositionData.begin(); it != m_sellPositionData.end(); ) {
		APPositionData& data = it->second;
		if (data.holdPosition == 0 && data.longFrozenPosition == 0 && data.shortFrozenPosition == 0) {
			it = m_buyPositionData.erase(it);
		}
		else {
			it++;
		}
	}
}

bool APPositionRepertory::capable(std::map<APASSETID, APPositionData>& data, const APPositionData & pd)
{
	if (data.find(pd.instrumentID) != data.end()) {
		if (data[pd.instrumentID].capable(pd)) {
			return true;
		}
	}

	return false;;
}

void APPositionRepertory::handle(std::map<APASSETID, APPositionData>& data, const APPositionData & pd)
{
	if (data.find(pd.instrumentID) != data.end()) {
		if (data[pd.instrumentID].capable(pd)) {
			data[pd.instrumentID].handle(pd);
		}
	}
}
