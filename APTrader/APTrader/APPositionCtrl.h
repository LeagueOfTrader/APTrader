#pragma once
#include "APDef.h"
#include <string>
#include "APAccountAssets.h"
#include "APTypes.h"
#include <list>
#include "Utils/APRedisSerializedObject.h"

class APTrade;
class APInstrumentQuotation;

class APPositionCtrl : public APRedisSerializedObject
{
public:
	APPositionCtrl();
	~APPositionCtrl();

	virtual void init(std::string positionInfo) = 0;

	virtual void syncPositionStatus();	

	void setInstrumentID(APASSETID instrumentID);
	APTradeDirection getTradeDirection();
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
	void setMarginPosition(long position);
	void resetAvailablePosition();
	long getMaxPosition();
	long getAvailablePosition();
	long getMarginPosition();
	long getHoldPosition();
	long getFrozenPosition();
	//void unfreezePosition(long position = -1);

	virtual std::vector<APPositionData> getHoldPositionDetail();
	virtual void setHoldAmount(APASSETID instrumentID, long amount);

	void recycleFund(APRecycleFundLevel level = RFL_UndeficitPosition);

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
	void onOrderOutdated(APORDERID orderID);

	void save();
	void load();

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
	long m_openOrdersPosition; // 开仓冻结量
	long m_closeOrdersPosition; // 平仓冻结量
	
	long m_holdPosition; // 持仓量
	long m_availablePosition; // 可平量
	long m_maxPosition; // 最大开仓上限
	long m_marginPosition; // 还可开仓的量
	long m_frozenPosition; // 冻结的量

	long m_todayHoldPosition; // 今持
	long m_yesterdayHoldPosition; // 昨持

	long m_cancelOpenOrdersPosition; // 撤销开仓量
	long m_cancelCloseOrdersPosition; // 撤销平仓量

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

