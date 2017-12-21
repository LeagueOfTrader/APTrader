#include "APFuturesCombinationPosCtrl.h"



APFuturesCombinationPosCtrl::APFuturesCombinationPosCtrl()
{
	m_coQuotation = NULL;
}

APFuturesCombinationPosCtrl::~APFuturesCombinationPosCtrl()
{
	if (m_coQuotation != NULL) {
		m_coQuotation = NULL;
	}
}

void APFuturesCombinationPosCtrl::init(std::string positionInfo)
{
}

void APFuturesCombinationPosCtrl::open(APTrendType type, double price, long volume)
{
}

void APFuturesCombinationPosCtrl::close(APTrendType type, double price, long volume)
{
}

void APFuturesCombinationPosCtrl::openAll(APTrendType type, double price)
{
}

void APFuturesCombinationPosCtrl::closeAll(APTrendType type, double price)
{
}
