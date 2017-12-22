#pragma once
#include "../Common/Singleton.h"
#include <vector>
#include <string>
#include "APQuotationMonitor.h"

class APQuotationMonitorManager : public Singleton<APQuotationMonitorManager>
{
public:
	APQuotationMonitorManager();
	~APQuotationMonitorManager();

	void init();
	void update();

	void destroy();

private:
	void runMonitor(std::string monitorName);

private:
	std::vector<APQuotationMonitor*> m_monitors;
};

