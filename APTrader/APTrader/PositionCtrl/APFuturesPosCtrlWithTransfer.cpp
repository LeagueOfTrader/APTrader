#include "APFuturesPosCtrlWithTransfer.h"
#include <algorithm>


APFuturesPosCtrlWithTransfer::APFuturesPosCtrlWithTransfer()
{
}


APFuturesPosCtrlWithTransfer::~APFuturesPosCtrlWithTransfer()
{
}

void APFuturesPosCtrlWithTransfer::init(std::string positionInfo)
{
	APFuturesPositionCtrl::init(positionInfo);
}

void APFuturesPosCtrlWithTransfer::setTargetContractID(APASSETID contractID)
{
	m_targetContractID = contractID;
}

void APFuturesPosCtrlWithTransfer::onTradeFinished(APASSETID commodityID, APTradeType type,  double price, long amount, APTrendType trend)
{
	if (commodityID != m_commodityID || commodityID != m_targetContractID || type != m_trendType) {
		return;
	}	

	if (m_isTransferring && !m_finishTransferring && type == TDT_Open) {
		if (commodityID == m_commodityID) {
			m_holdPosition += amount;
		}
		else {
			m_targetContractHoldPosition += amount;
		}
		m_openOrdersPosition -= amount;		
	}
	else {
		APFuturesPositionCtrl::onTradeFinished(commodityID, type, price, amount, trend);
	}

	if (!m_finishTransferring) {
		checkTransferStatus();
	}
}

void APFuturesPosCtrlWithTransfer::open(APTrendType type, double price, long amount)
{
	if (m_isTransferring) {
		APFuturesPositionCtrl::open(m_targetContractID, type, price, amount);
		return;
	}

	APFuturesPositionCtrl::open(type, price, amount);
}

void APFuturesPosCtrlWithTransfer::close(APTrendType type, double price, long amount)
{
	if (m_isTransferring) {
		long srcContractAmount = std::min(amount, m_holdPosition - m_closeOrdersPosition);
		APFuturesPositionCtrl::close(type, price, srcContractAmount);
		if (amount > m_holdPosition - m_closeOrdersPosition) {
			long amountSurplus = amount - (m_holdPosition - m_closeOrdersPosition);
			APFuturesPositionCtrl::close(m_targetContractID, type, price, amountSurplus);
		}
		return;
	}

	APFuturesPositionCtrl::close(type, price, amount);
}

void APFuturesPosCtrlWithTransfer::beginTransfer()
{
	m_isTransferring = true;
	m_positonNeedTransfer = m_holdPosition;
}

bool APFuturesPosCtrlWithTransfer::isTransferring()
{
	return m_isTransferring;
}

bool APFuturesPosCtrlWithTransfer::isTransferFinished()
{
	return m_finishTransferring;
}

void APFuturesPosCtrlWithTransfer::transferContracts(double droppedContractPrice, double targetContractPrice, long amount)
{
	long realAmount = std::min(amount, m_holdPosition - m_closeOrdersPosition);
	//fak may has mistakes
	APFuturesPositionCtrl::close(m_commodityID, m_trendType, droppedContractPrice, realAmount);
	realAmount = std::min(realAmount, m_positonNeedTransfer - m_targetContractHoldPosition);
	if (realAmount > 0) {
		APFuturesPositionCtrl::open(m_targetContractID, m_trendType, targetContractPrice, realAmount);
	}
}

void APFuturesPosCtrlWithTransfer::checkTransferStatus()
{
	if (m_isTransferring) {
		if (m_holdPosition == 0 && m_targetContractHoldPosition >= m_positonNeedTransfer) {
			m_finishTransferring = true;
		}
	}
}
