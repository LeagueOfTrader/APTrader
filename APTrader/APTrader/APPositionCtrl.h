#pragma once
#include "APDef.h"
#include <string>
#include "APAccountAssets.h"
#include "APTypes.h"
#include <list>

class APTrade;
class APCommodityQuotation;

class APPositionCtrl
{
public:
	APPositionCtrl();
	~APPositionCtrl();

	virtual void init(std::string positionInfo) = 0;

	virtual void syncPositionStatus();	

	void setCommodityID(APASSETID commodityID);
	APTrendType getTrendType();
	const APASSETID& getCommodityID();
	UINT getID();

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

	void openTrade(APTrendType type, double price, long volume);
	void closeTrade(APTrendType type, double price, long volume);
	void openAllTrade(APTrendType type, double price);
	void closeAllTrade(APTrendType type, double price);
	//void cancelTrade(APTradeType type, double price, long volume); // may implement later
	void cancelTrade(APTradeType type, double price, APTrendType trend);
	void cancelTrade(APTradeType type);
	void cancelAllTrade();

	virtual void onTradeFinished(APASSETID commodityID, APTradeType type,  double price, long volume, APORDERID orderID, APTrendType trend = TT_Long) = 0;

	virtual void update();

	void bindTrade(APTrade* trade);

	void onCompleteOrder(APORDERID orderID, APTradeType type);

protected:
	virtual void open(APTrendType type, double price, long volume) = 0;
	virtual void close(APTrendType type, double price, long volume) = 0;
	virtual void openAll(APTrendType type, double price) = 0;
	virtual void closeAll(APTrendType type, double price) = 0;
	//virtual void cancel(APTradeType type, double price, long volume) = 0;
	virtual void cancel(APTradeType type, double price, APTrendType trend = TT_Long);
	virtual void cancel(APTradeType type) = 0;
	virtual void cancelAll();
	
	virtual void onSyncPositionStatus(const APPositionData& data);

	void setBaseParam(std::string positionInfo);

protected:
	long m_openOrdersPosition;
	long m_closeOrdersPosition;
	long m_holdPosition; // position to close
	long m_maxPosition; // maxium position
	long m_availablePosition; // position available to open
	long m_frozenPosition; // freezed position, cannot close until t+n

	double m_availableFund;
	double m_freeFund;
	double m_frozenFund;

	APTrade* m_trade;
	bool m_isRecyclingFund;
	APASSETID m_commodityID;
	APTrendType m_trendType;

	UINT m_id;

	APCommodityQuotation* m_quotation;

	std::list<APORDERID> m_openOrderList;
	std::list<APORDERID> m_closeOrderList;

public:
	friend class APPositionManager;
};

