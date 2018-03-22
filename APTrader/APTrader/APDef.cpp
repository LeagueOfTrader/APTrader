#include "APDef.h"

APTradeDirection getReversedDirection(APTradeDirection direction) {
	APTradeDirection rdir;
	if (direction == TD_Buy) {
		rdir = TD_Sell;
	}
	else {
		rdir = TD_Buy;
	}
	return rdir;
}