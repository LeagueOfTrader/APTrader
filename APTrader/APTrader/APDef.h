#pragma once
enum APTradeDirection {
	TD_Buy,
	TD_Sell,
	//TT_Sideway,
	//TT_Suspend,
	TD_Num
};

enum APPositionDirection {
	PD_Long,
	PD_Short,
	PD_Num
};

enum APFinancialInstrumentType {
	FCT_Shares,
	FCT_Futrues,
	FCT_Options,
	FCT_Num
};

enum APTradeType {
	TT_Open,
	TT_Close,
	//TT_CancelOpen,
	//TT_CancelClose,
	TT_CloseYesterday,
	TT_CloseToday,
	TT_Lock, // lock position, only for futures
	TT_Num
};

enum APRecycleFundLevel {
	RFL_FreeFund,
	RFL_UnsealedOrder,
	RFL_ProfitPosition,
	RFL_UndeficitPosition,
	RFL_All,
	RFL_Num
};

enum APQuotationDecisionType {
	QDT_Single = 1,
	QDT_PriceMargin,
	QDT_PriceRatio,
	//Sim
	QDT_SimSingle = 101,
	QDT_Num
};

enum APRunMode {
	RM_Trade,
	RM_Backtest,
	RM_Num
};

enum APOpenPositionLimit {
	OPL_Fund,
	OPL_Position,
	OPL_Num
};

enum APOrderTimeCondition {
	OTC_GoodForDay,
	OTC_FAK,
	OTC_FOK,
	OTC_GoodTillDate,
	OTC_GoodForSection, // only for futures
	OTC_GoodForAuction,
	OTC_GoodTillCancel,
	OTC_Num,
};

enum APOrderPriceType {
	OPT_LimitPrice,
	OPT_AnyPrice,
	OPT_BestPrice,
	OPT_LastPrice,
	OPT_LastPricePlusOneTicks,
	OPT_LastPricePlusTwoTicks,
	OPT_LastPricePlusThreeTicks,
	OPT_AskPrice1,
	OPT_AskPrice1PlusOneTicks,
	OPT_AskPrice1PlusTwoTicks,
	OPT_AskPrice1PlusThreeTicks,
	OPT_BidPrice1,
	OPT_BidPrice1PlusOneTicks,
	OPT_BidPrice1PlusTwoTicks,
	OPT_BidPrice1PlusThreeTicks,
	OPT_FiveLevelPrice,
	OPT_Num
};

enum APOrderState {
	OS_None,
	OS_NotTouched,
	OS_Touched,
	OS_NoTradeQueueing,
	OS_NoTradeNotQueueing,
	OS_PartTradedQueueing,
	OS_AllTraded,	
	OS_PartTradedNotQueueing,	
	OS_Canceled,	
	OS_Unknown,
	OS_Num
};

enum APOrderVolumeCondition {
	OVC_Any,
	OVC_Min,
	OVC_All,
	OVC_Num
};

enum APOrderContingentCondition {
	OCC_Immediately,
	OCC_Touch,
	OCC_TouchProfit,
	OCC_ParkedOrder,
	OCC_LastPriceGreaterThanStopPrice,
	OCC_LastPriceGreaterEqualStopPrice,
	OCC_LastPriceLesserThanStopPrice,
	OCC_LastPriceLesserEqualStopPrice,
	OCC_AskPriceGreaterThanStopPrice,
	OCC_AskPriceGreaterEqualStopPrice,
	OCC_AskPriceLesserThanStopPrice,
	OCC_AskPriceLesserEqualStopPrice,
	OCC_BidPriceGreaterThanStopPrice,
	OCC_BidPriceGreaterEqualStopPrice,
	OCC_BidPriceLesserThanStopPrice,
	OCC_BidPriceLesserEqualStopPrice,
	OCC_Num
};

const unsigned int INVALID_ORDER_ID = 0;
const double MARKET_ORDER_PRICE = -0.00999;