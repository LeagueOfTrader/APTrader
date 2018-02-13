#include "APTransferPositionStrategy.h"
#include "../Utils/APTimeUtility.h"
#include "../Futures/APFuturesIDSelector.h"
#include "../APMarketDataManager.h"
#include "../Quotation/APFuturesQuotation.h"
#include "../PositionCtrl/APFuturesPosCtrlWithTransfer.h"
#include <algorithm>
#include "../Utils/APJsonReader.h"

UINT BEYOND_TRANSFER_MONTHS = 3;

APTransferPositionStrategy::APTransferPositionStrategy()
{
}


APTransferPositionStrategy::~APTransferPositionStrategy()
{
}

APStrategy * APTransferPositionStrategy::create()
{
	return new APTransferPositionStrategy();
}

void APTransferPositionStrategy::init(std::string strategyInfo)
{
	APStrategy::init(strategyInfo);

	APJsonReader jr;
	jr.initWithString(strategyInfo);
	std::string transferInfo = jr.getObjValue("Transfer");
	initWithTransferInfo(transferInfo);
}

void APTransferPositionStrategy::initWithTransferInfo(std::string transferInfo)
{
	APJsonReader jr;
	jr.initWithString(transferInfo);
	std::string directionType = jr.getStrValue("Direction");
	if (directionType == "Buy") {
		m_directionType = TD_Buy;
	}
	else if (directionType == "Sell") {
		m_directionType = TD_Sell;
	}

	m_instrumentID = jr.getStrValue("SrcID");
	m_targetContractID = jr.getStrValue("TargetID");
	m_criticalDays = jr.getIntValue("CriticalDays");
	m_deadlineDays = jr.getIntValue("DeadlineDays");

	m_priceMargin = jr.getDoubleValue("PriceMargin");
	m_priceMarginMax = jr.getDoubleValue("PriceMarginMax");

	m_safePrice = jr.getDoubleValue("SafePrice");

	std::string interpType = jr.getStrValue("InterpolationType");
	if (interpType == "Linear") {
		m_interpType = IMT_Linear;
	}
	else if (interpType == "Quad") {
		m_interpType = IMT_Quad;
	}

	m_curQuotation = dynamic_cast<APFuturesQuotation*>(APMarketDataMgr->subscribeInstrument(m_instrumentID));
	m_targetQuotation = dynamic_cast<APFuturesQuotation*>(APMarketDataMgr->subscribeInstrument(m_targetContractID));
}

void APTransferPositionStrategy::update()
{
	if (!m_work) {
		return;
	}

	if (isCloseToDeliver()) {
		APFuturesPosCtrlWithTransfer* pc = dynamic_cast<APFuturesPosCtrlWithTransfer*>(m_positionCtrl);
		if (pc != NULL) {
			if (pc->isTransferFinished()) {
				//onFinishTransfer();
				return;
			}

			if (!pc->isTransferring()) {
				pc->beginTransfer();
			}

			if (canTransferWithCurrentPrice()) {
				transferContracts();
			}
		}
	}
}

void APTransferPositionStrategy::alert()
{
}

bool APTransferPositionStrategy::isCloseToDeliver()
{
	std::string ym = APFuturesIDSelector::getTimeSymbol(m_targetContractID);
	UINT months = APTimeUtility::getMonthsToFuturesDeliveryDate(ym);
	if (months > BEYOND_TRANSFER_MONTHS) {
		return false;
	}
	UINT days = APTimeUtility::getDaysToFuturesDeliveryDate(ym);
	if (days <= m_criticalDays) {
		return true;
	}

	return false;
}

void APTransferPositionStrategy::transferContracts()
{
	if (m_curQuotation != NULL && m_targetQuotation != NULL) {
		double srcPrice = m_curQuotation->getBuyPrice();
		long srcVolume = m_curQuotation->getBuyVolume();
		double targetPrice = m_targetQuotation->getSellPrice();
		long targetVolume = m_targetQuotation->getSellVolume();
		long volume = std::min(srcVolume, targetVolume);
		APFuturesPosCtrlWithTransfer* pc = dynamic_cast<APFuturesPosCtrlWithTransfer*>(m_positionCtrl);
		if (pc != NULL) {
			pc->transferContracts(srcPrice, targetPrice, volume);
		}
	}
}

bool APTransferPositionStrategy::canArbitage()
{
	if (m_curQuotation != NULL && m_targetQuotation != NULL) {
		if (m_directionType == TD_Buy) {
			if (m_targetQuotation->getCurPrice() <= m_curQuotation->getCurPrice()) {
				return true;
			}
		}
		else if (m_directionType == TD_Sell) {
			if (m_targetQuotation->getCurPrice() >= m_curQuotation->getCurPrice()) {
				return true;
			}
		}
	}

	return false;
}

double APTransferPositionStrategy::getCurPriceMargin()
{
	std::string ym = APFuturesIDSelector::getTimeSymbol(m_targetContractID);
	UINT days = APTimeUtility::getDaysToFuturesDeliveryDate(ym);

	double pm = APInterpolator::interplate<double, double>(m_priceMarginMax, m_priceMargin, (double)m_deadlineDays, (double)m_criticalDays, (double)days, m_interpType);
	return pm;
}

bool APTransferPositionStrategy::canTransferWithCurrentPrice()
{
	if (m_curQuotation != NULL && m_targetQuotation != NULL) {
		double curTargetPrice = m_targetQuotation->getCurPrice();
		if ((m_directionType == TD_Buy && curTargetPrice > m_safePrice) ||
			(m_directionType == TD_Sell && curTargetPrice < m_safePrice)){
			return false;
		}

		double priceMargin = getCurPriceMargin();
		if (m_directionType == TD_Buy) {
			if (m_targetQuotation->getSellPrice() - m_curQuotation->getBuyPrice() <= priceMargin) {
				return true;
			}
		}
		else if (m_directionType == TD_Sell) {
			if (m_curQuotation->getBuyPrice() - m_targetQuotation->getSellPrice() >= priceMargin) {
				return true;
			}
		}
	}

	return false;
}

//void APTransferPositionStrategy::onFinishTransfer()
//{
//	// set parent strategy's new instrumentID, also position ctrl's instrumentID and sync positionCtrl's status
//	if (m_positionCtrl != NULL) {
//		m_positionCtrl->setInstrumentID(m_targetContractID);
//		//m_positionCtrl->syncPositionStatus();
//	}
//	// detach from parent strategy -- or generate new delivery date
//	APStrategy* parentStrategy = getParent();
//	if (parentStrategy != NULL) {
//		parentStrategy->setInstrumentID(m_targetContractID);
//		//parentStrategy->detach(this);
//	}
//
//	detachFromParent();
//}
