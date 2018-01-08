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

void APFuturesPosCtrlWithTransfer::setTargetContractID(APASSETID instrumentID)
{
	m_targetContractID = instrumentID;
}

void APFuturesPosCtrlWithTransfer::onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long deltaVolume, APORDERID orderID, APTradeDirection direction)
{
	if (instrumentID != m_instrumentID || instrumentID != m_targetContractID || type != m_directionType) {
		return;
	}	

	if (m_isTransferring && !m_finishTransferring && type == TT_Open) {
		if (instrumentID == m_instrumentID) {
			//m_availablePosition += deltaVolume;
			m_holdPosition += deltaVolume;
		}
		else {
			m_targetContractHoldPosition += deltaVolume;
		}
		m_availablePosition += deltaVolume;
		m_openOrdersPosition -= deltaVolume;
	}
	else {
		APFuturesPositionCtrl::onTradeDealt(instrumentID, type, price, deltaVolume, orderID, direction);
	}

	if (!m_finishTransferring) {
		checkTransferStatus();
	}
}

void APFuturesPosCtrlWithTransfer::open(APTradeDirection direction, double price, long volume)
{
	if (m_isTransferring) {
		APFuturesPositionCtrl::open(m_targetContractID, direction, price, volume);
		return;
	}

	APFuturesPositionCtrl::open(direction, price, volume);
}

void APFuturesPosCtrlWithTransfer::close(APTradeDirection direction, double price, long volume)
{
	if (m_isTransferring) {
		long srcContractVolume = std::min(volume, m_holdPosition - m_closeOrdersPosition);
		APFuturesPositionCtrl::close(direction, price, srcContractVolume);
		if (volume > m_holdPosition - m_closeOrdersPosition) {
			long volumeSurplus = volume - (m_holdPosition - m_closeOrdersPosition);
			APFuturesPositionCtrl::close(m_targetContractID, direction, price, volumeSurplus);
		}
		return;
	}

	APFuturesPositionCtrl::close(direction, price, volume);
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

void APFuturesPosCtrlWithTransfer::transferContracts(double droppedContractPrice, double targetContractPrice, long volume)
{
	long realVolume = std::min(volume, m_holdPosition - m_closeOrdersPosition);
	//fak may has mistakes
	APFuturesPositionCtrl::close(m_instrumentID, m_directionType, droppedContractPrice, realVolume);
	realVolume = std::min(realVolume, m_positonNeedTransfer - m_targetContractHoldPosition);
	if (realVolume > 0) {
		APFuturesPositionCtrl::open(m_targetContractID, m_directionType, targetContractPrice, realVolume);
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
