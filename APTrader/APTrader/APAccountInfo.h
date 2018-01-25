#pragma once
#include "Common/Singleton.h"
#include <string>
#include <map>
#include "APDef.h"
#include "APTypes.h"
#include <queue>
#include <vector>
#include "APMacro.h"
#include "Common/InitializableObject.h"
#include "APStructs.h"
#include <mutex>

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


struct APPositionCtrlWrapper {
	APASSETID instrumentID;
	APTradeDirection direction;
	APPositionCtrl* posCtrl;
};

class APPositionCtrlWrapperComparer {
public :
	bool operator()(APPositionCtrlWrapper p0, APPositionCtrlWrapper p1) const;
};

class APAccountInfo : public InitializableObject, public Singleton<APAccountInfo>
{
public:
	APAccountInfo();
	~APAccountInfo();

	virtual void init();

	//bool getPositionData(APASSETID instrumentID, APTradeDirection direction, APPositionData& positionData);	
	void update();

	void verifyPosition(APASSETID instrumentID, APTradeDirection dir, APPositionCtrl* posCtrl);	
	void verify();
	void queryAllPosition();

	const std::vector<APPositionData>& getPositionInfo();
	
	std::string getAccountID();
	std::string getInterfaceType();

#ifdef USE_CTP
	void onGetPositionData(APASSETID instrumentID, std::vector<CThostFtdcInvestorPositionField>& positionData);
	void onSyncPositionData();
#endif

protected:
	void onGetPositionData(APPositionData data);
	//void verifyAfterCheck();
	void processVerification();

#ifdef USE_CTP
	void appendPositionInfo(APPositionData& pd, CThostFtdcInvestorPositionField& info);
#endif

private:
	std::priority_queue<APPositionCtrlWrapper, std::vector<APPositionCtrlWrapper>, APPositionCtrlWrapperComparer> m_verificationQueue;

	//std::set<APASSETID> m_instruments;
	std::vector<APPositionData> m_positionInfo;
	std::vector<APPositionData> m_cachedPositionInfo;

	std::string m_accountID;
	std::string m_interfaceType;
	long m_lastTick;
	std::mutex m_mutex;
};

