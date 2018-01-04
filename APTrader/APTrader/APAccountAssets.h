#pragma once
#include "Common/Singleton.h"
#include <string>
#include <map>
#include "APDef.h"
#include "APTypes.h"
#include <queue>
#include <set>
#include "APMarco.h"

#ifdef USE_CTP
#include "ThostFtdcUserApiStruct.h"
#endif

class APPositionCtrl;

//struct APPositionData {
//	std::string id = "";
//	APTradeDirection direction = TT_Num;
//	long holdPosition = 0;
//	long frozenPosition = 0;
//	long openOrdersPosition = 0;
//	long closeOrdersPosition = 0;
//
//	void append(const APPositionData& data) {
//		holdPosition += data.holdPosition;
//		frozenPosition += data.frozenPosition;
//		openOrdersPosition += data.openOrdersPosition;
//		closeOrdersPosition += data.closeOrdersPosition;
//	}
//};

struct APPositionData {
	std::string instrumentID;
	APTradeDirection direction;
	long holdPosition;
	long frozenPosition;
	
	// used for local verification
	long remainPosition;
};

struct APPositionCtrlWrapper {
	APASSETID instrumentID;
	APTradeDirection direction;
	APPositionCtrl* posCtrl;
};

class APPositionCtrlWrapperComparer {
public :
	bool operator()(APPositionCtrlWrapper p0, APPositionCtrlWrapper p1) const;
};

class APAccountAssets : public Singleton<APAccountAssets>
{
public:
	APAccountAssets();
	~APAccountAssets();

	virtual void init();
	 
	bool inited();
	//bool getPositionData(APASSETID instrumentID, APTradeDirection direction, APPositionData& positionData);	

	void verifyPosition(APASSETID instrumentID, APTradeDirection dir, APPositionCtrl* posCtrl);
	void queryPosition(APASSETID instrumentID, APTradeDirection dir, APPositionCtrl* posCtrl);
	
	void verify();

#ifdef USE_CTP
	void onGetPositionData(CThostFtdcInvestorPositionField* pInvestorPosition);
#endif

protected:
	void onGetPositionData(APPositionData data);
	void checkFinish();

private:
	std::map<APASSETID, APPositionData> m_buyPositionData;
	std::map<APASSETID, APPositionData> m_sellPositionData;

	std::priority_queue<APPositionCtrlWrapper, std::vector<APPositionCtrlWrapper>, APPositionCtrlWrapperComparer> m_verificationQueue;
	std::priority_queue<APPositionCtrlWrapper, std::vector<APPositionCtrlWrapper>, APPositionCtrlWrapperComparer> m_allocQueue;

	std::set<APASSETID> m_instruments;

	bool m_inited;
};

