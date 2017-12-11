#include "APPositionManager.h"
#include "APObjectFactory.h"
#include "APGlobalConfig.h"
#include "APPositionCtrl.h"
//#include "APTradeManager.h"

APPositionManager::APPositionManager()
{
	m_idAccumulator = new APIntAccumulator();
}


APPositionManager::~APPositionManager()
{
	delete m_idAccumulator;
}

APPositionCtrl * APPositionManager::createPositionCtrl(std::string positionInfo)
{
	APPositionCtrl* pc = APObjectFactory::newPositionCtrl(APGlobalConfig::getInstance()->getInstrumentType());
	pc->m_id = m_idAccumulator->generateID();
	pc->init(positionInfo);
	m_positionCtrls[pc->m_id] = pc;
	//APTradeManager::getInstance()->registerPositionCtrl(pc->getInstrumentID(), pc->getTrendType(), pc);
	return pc;
}

APPositionCtrl * APPositionManager::getPositionCtrl(UINT id) {
	if (m_positionCtrls.find(id) != m_positionCtrls.end()) {
		return m_positionCtrls[id];
	}

	return NULL;
}

void APPositionManager::removePositionCtrl(UINT id) {
	if (m_positionCtrls.find(id) != m_positionCtrls.end()) {
		m_positionCtrls.erase(id);
	}
}

void APPositionManager::removePositionCtrl(APPositionCtrl* pc) {
	if (pc != NULL) {
		removePositionCtrl(pc->getID());
	}
}

void APPositionManager::update()
{
	std::map<UINT, APPositionCtrl*>::iterator it;
	for (it = m_positionCtrls.begin(); it != m_positionCtrls.end(); it++) {
		APPositionCtrl* pc = it->second;
		if (pc != NULL) {
			pc->update();
		}
	}
}

std::map<UINT, APPositionCtrl*>& APPositionManager::getAllPositionCtrls()
{
	return m_positionCtrls;
}
