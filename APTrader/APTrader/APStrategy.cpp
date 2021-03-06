#include "APStrategy.h"
#include "APPositionCtrl.h"
#include <stdio.h>
#include "APObjectFactory.h"
#include "Utils/APJsonReader.h"
#include "APPositionManager.h"
#include "APStrategyManager.h"
#include "APQuotationDecision.h"
#include "Utils/APLog.h"

APStrategy::APStrategy()
{
	m_isSubStrategy = false;
	m_priority = 0;
	m_positionCtrl = NULL;
	m_quotationDecision = NULL;
	m_closeOnly = false;
	m_work = false;
	m_tag = "";
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

	if (jr.hasMember("Tag")) {
		m_tag = jr.getStrValue("Tag");
	}

	//if (jr.hasMember("Subsidiary")) {
	//	m_isSubStrategy = jr.getBoolValue("Subsidiary");
	//}

	if (jr.hasMember("Position")) {
		std::string positionInfo = jr.getObjValue("Position");
		m_positionCtrl = APPositionManager::getInstance()->createPositionCtrl(positionInfo);
		m_positionCtrl->setTag(m_tag);
		//m_positionCtrl->init(positionInfo);
	}
	
	if (jr.hasMember("Quotation")) {
		std::string quotationInfo = jr.getObjValue("Quotation");
		initQuotationDecision(quotationInfo);
	}

	//if (jr.hasMember("SubStrategies")) {
	//	int subStrategiesCount = jr.getArraySize("SubStrategies");
	//	for (int i = 0; i < subStrategiesCount; i++) {
	//		std::string subFile = jr.getArrayStrValue("SubStrategies", i);
	//		APStrategy* subStrategy = APStrategyManager::getInstance()->loadStrategy(subFile);
	//		if (subStrategy != NULL) {
	//			this->attach(subStrategy);
	//			subStrategy->setWork(true);
	//		}
	//	}
	//}

	if (m_positionCtrl != NULL) {
		APLogger->log("Strategy: %s inited, PositionCtrl: %d", m_tag.c_str(), m_positionCtrl->getID());
		m_positionCtrl->outputInfo();
	}
}

//void APStrategy::setPositionCtrl(APPositionCtrl * positionCtrl)
//{
//	m_positionCtrl = positionCtrl;
//}

void APStrategy::exit()
{
	if (m_positionCtrl != NULL) {
		m_positionCtrl->save();
	}
}

void APStrategy::setPriority(UINT priority)
{
	m_priority = priority;
}

UINT APStrategy::getPriority()
{
	return m_priority;
}

void APStrategy::setPositionCtrl(APPositionCtrl * posCtrl)
{
	m_positionCtrl = posCtrl;
}

APPositionCtrl * APStrategy::getPositionCtrl()
{
	return m_positionCtrl;
}

bool APStrategy::equals(APStrategy * strategy)
{
	return m_id == strategy->getID();
}

void APStrategy::initQuotationDecision(std::string quotationInfo)
{
	APJsonReader jr;
	jr.initWithString(quotationInfo);
	APQuotationDecisionType type = (APQuotationDecisionType)jr.getIntValue("QuotationDecisionType");
	std::string srcID = jr.getStrValue("SrcID");
	
	std::string targetID = jr.getStrValue("TargetID");	

	double upper = jr.getDoubleValue("UpperLimit");
	double lower = jr.getDoubleValue("LowerLimit");
	m_quotationDecision = APObjectFactory::newQuotationDecision(type, srcID, targetID, upper, lower);
		
	
}

void APStrategy::open(long volume)
{
	if (m_positionCtrl != NULL) {
		m_positionCtrl->openPosition(volume);
	}
}

void APStrategy::close(long volume)
{
	if (m_positionCtrl != NULL) {
		m_positionCtrl->closePosition(volume);
	}
}

void APStrategy::open(double price, long volume)
{
	if (m_positionCtrl != NULL) {
		m_positionCtrl->openPosition(price, volume);
	}
}

void APStrategy::close(double price, long volume)
{
	if (m_positionCtrl != NULL) {
		m_positionCtrl->closePosition(price, volume);
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

void APStrategy::setWork(bool work)
{
	m_work = work;
}

bool APStrategy::isCloseOnly()
{
	return m_closeOnly;
}

bool APStrategy::isWork()
{
	return m_work;
}


