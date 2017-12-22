#include "APQuotationMonitorManager.h"
#include "../Utils/APJsonReader.h"
#include "APQuotationMonitor.h"

const std::string monitorsPath = "Data/Monitor/";
const std::string monitorRunCfg = "Data/Monitor/run.cfg";

APQuotationMonitorManager::APQuotationMonitorManager()
{
	m_monitors.clear();
}


APQuotationMonitorManager::~APQuotationMonitorManager()
{
}

void APQuotationMonitorManager::init() {
	APJsonReader jr;
	jr.initWithFile(monitorRunCfg);

	int count = jr.getArraySize("Monitors");
	for (int i = 0; i < count; i++) {
		std::string monitorName = jr.getArrayStrValue("Monitors", i);
		runMonitor(monitorName);
	}
}

void APQuotationMonitorManager::update() {
	for (int i = 0; i < m_monitors.size(); i++) {
		APQuotationMonitor* pMonitor = m_monitors[i];
		if (pMonitor != NULL) {
			pMonitor->update();
		}
	}
}

void APQuotationMonitorManager::destroy() {
	for (int i = 0; i < m_monitors.size(); i++) {
		delete m_monitors[i];
	}

	m_monitors.clear();
}

void APQuotationMonitorManager::runMonitor(std::string monitorName) {
	std::string monitorFilename = monitorsPath + monitorName + ".cfg";
	APQuotationMonitor* mt = new APQuotationMonitor();
	mt->init(monitorFilename);
	m_monitors.push_back(mt);
}