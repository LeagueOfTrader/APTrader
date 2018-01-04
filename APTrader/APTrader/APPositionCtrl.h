#pragma once
#include "APDef.h"
#include <string>
#include "APAccountAssets.h"
#include "APTypes.h"
#include <list>
#include "Common/SerializedObject.h"

class APTrade;
class APInstrumentQuotation;

class APPositionCtrl : public SerializedObject
{
public:
	APPositionCtrl();
	~APPositionCtrl();

	virtual void init(std::string positionInfo) = 0;

	virtual void syncPositionStatus();	

	void setInstrumentID(APASSETID instrumentID);
	APTradeDirection getTrendType();
	const APASSETID& getInstrumentID();
	UINT getID();

	void setTag(std::string tag);
	int getPriority();

	//void subscribeGoodsInfo();
	void setAvailableFund(double fund);
	void setFrozenFund(double fund); //
	void setFreeFund(double fund);
	double getAvailableFund();
	double getFreeFund();
	double getFrozenFund();

	void setMaxPosition(long position);
	void setFrozenPosition(long position);
	void setAvailablePosition(long position);
	void resetAvailablePosition();
	long getMaxPosition();
	long getAvailablePosition();
	long getFrozenPosition();
	void unfreezePosition(long position = -1);

	void recycleFund(APRecycleFundLevel level = RFL_UndeficitPosition);

	long getCurPosition();
	long getTradablePosition();
	long getOpenOrderedPosition();
	long getCloseOrderedPosition();

	virtual void openPosition(APTradeDirection direction, double price, long volume);
	virtual void closePosition(APTradeDirection direction, double price, long volume);
	virtual void openFullPosition(APTradeDirection direction, double price);
	virtual void closeOffPosition(APTradeDirection direction, double price);
	//void cancelTrade(APTradeType type, double price, long volume); // may implement later
	virtual void cancelTrade(APTradeType type, double price, APTradeDirection direction);
	virtual void cancelTrade(APTradeType type);
	virtual void cancelAllTrade();

	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long deltaVolume, APORDERID orderID, APTradeDirection direction = TD_Buy) = 0;
	virtual void onTradeCanceled(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy) = 0;

	virtual void update();

	void bindTrade(APTrade* trade);

	void onCompleteOrder(APORDERID orderID, APTradeType type);

protected:
	virtual void open(APTradeDirection direction, double price, long volume) = 0;
	virtual void close(APTradeDirection direction, double price, long volume) = 0;
	virtual void openAll(APTradeDirection direction, double price) = 0;
	virtual void closeAll(APTradeDirection direction, double price) = 0;
	//virtual void cancel(APTradeType type, double price, long volume) = 0;
	virtual void cancel(APTradeType type, double price, APTradeDirection direction = TD_Buy);
	virtual void cancel(APTradeType type) = 0;
	virtual void cancelAll();
	void cancel(APORDERID orderID);
	
	virtual void onSyncPositionStatus(const APPositionData& data);

	virtual void initWithData(std::string positionInfo);

	virtual std::string serialize();
	virtual void deserialize(std::string str);

protected:
	long m_openOrdersPosition;
	long m_closeOrdersPosition;
	long m_holdPosition; // position to close
	long m_maxPosition; // maxium position
	long m_availablePosition; // position available to open
	long m_frozenPosition; // freezed position, cannot close until t+n

	// process later, fund
	double m_availableFund;
	double m_freeFund;
	double m_frozenFund;

	APTrade* m_trade;
	bool m_isRecyclingFund;
	APASSETID m_instrumentID;
	APTradeDirection m_directionType;

	UINT m_id;

	APInstrumentQuotation* m_quotation;

	std::list<APORDERID> m_openOrderList;
	std::list<APORDERID> m_closeOrderList;

	std::string m_tag;

	int m_priority;

public:
	friend class APPositionManager;
};

