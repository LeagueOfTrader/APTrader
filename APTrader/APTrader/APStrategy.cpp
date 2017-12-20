#include "APStrategy.h"
#include "APPositionCtrl.h"
#include <stdio.h>
#include "APObjectFactory.h"
#include "Utils/APJsonReader.h"
#include "APPositionManager.h"
#include "APStrategyManager.h"

APStrategy::APStrategy()
{
	m_isSubStrategy = false;
	m_priority = 0;
	m_positionCtrl = NULL;
	m_quotationDecision = NULL;
	m_closeOnly = false;
}

APStrategy::~APStrategy()
{
	if(!m_isSubStrategy){
		delete m_positionCtrl;
		m_positionCtrl = NULL;
	}

	if (m_quotationDecision != NULL) {
		delete m_quotationDecision;
		m_quotationDecision = NULL;
	}
}

void APStrategy::init(std::string strategyInfo)
{
	APJsonReader jr;
	jr.initWithString(strategyInfo);
	if (jr.hasMember("Subsidiary")) {
		m_isSubStrategy = jr.getBoolValue("Subsidiary");
	}

	if (jr.hasMember("Position")) {
		std::string positionInfo = jr.getObjValue("Position");
		m_positionCtrl = APPositionManager::getInstance()->createPositionCtrl(positionInfo);
		//m_positionCtrl->init(positionInfo);
	}
	
	if (jr.hasMember("Quotation")) {
		std::string quotationInfo = jr.getObjValue("Quotation");
		initQuotationDecision(quotationInfo);
	}

	if (jr.hasMember("SubStrategies")) {
		int subStrategiesCount = jr.getArraySize("SubStrategies");
		for (int i = 0; i < subStrategiesCount; i++) {
			std::string subFile = jr.getArrayStrValue("SubStrategies", i);
			APStrategy* subStrategy = APStrategyManager::getInstance()->runStrategy(subFile);
			this->attach(subStrategy);
		}
	}
}

//void APStrategy::setPositionCtrl(APPositionCtrl * positionCtrl)
//{
//	m_positionCtrl = positionCtrl;
//}

void APStrategy::setPriority(UINT priority)
{
	m_priority = priority;
}

UINT APStrategy::getPriority()
{
	return m_priority;
}

APPositionCtrl * APStrategy::getPositionCtrl()
{
	return m_positionCtrl;
}

bool APStrategy::equals(APStrategy * strategy)
{
	return m_id == strategy->getID();
}

UINT APStrategy::getID()
{
	return m_id;
}

void APStrategy::setID(UINT id)
{
	m_id = id;
}

void APStrategy::initQuotationDecision(std::string quotationInfo)
{
	APJsonReader jr;
	jr.initWithString(quotationInfo);
	APQuotationDecisionType type = (APQuotationDecisionType)jr.getIntValue("QuotationDecisionType");
	std::string srcID = jr.getStrValue("SrcID");
	if (jr.hasMember("TargetID")) {
		std::string targetID = jr.getStrValue("TargetID");	
		if (jr.hasMember("Param")) {
			double param = jr.getDoubleValue("Param");
			m_quotationDecision = APObjectFactory::newQuotationDecision(type, srcID, targetID, param);
		}
	}
}

void APStrategy::setInstrumentID(APASSETID instrumentID)
{
	m_instrumentID = instrumentID;
}

void APStrategy::setCloseOnly(bool closeOnly)
{
	m_closeOnly = closeOnly;
}


