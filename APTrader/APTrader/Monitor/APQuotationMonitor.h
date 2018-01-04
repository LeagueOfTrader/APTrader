#pragma once
#include <string>
#include <vector>
#include "../APDef.h"

class APQuotationDecision;

enum APMonitorPriorityLevel {
	MPL_None,
	MPL_Normal,
	MPL_Important,
	MPL_Urgent,
	MPL_Num
};

enum APMonitorGridType {
	MGT_Fix = 1,
	MGT_Ratio,
	MGT_Diff,
	MGT_Num
};

class APQuotationMonitor
{
public:
	APQuotationMonitor();
	~APQuotationMonitor();

	virtual void init(std::string filename);
	virtual void update();

private:
	void goGrids(double value);
	bool inOpenSection(double value);
	bool inCloseSection(double value);

	int getGridIndex(std::vector<double>& grids, double value, bool reverse = false);
	int getCurTrendGridIndex(std::vector<double>& grids, double value, bool open);

	void payAttention(std::string content, double value, APMonitorPriorityLevel lv = MPL_None);

	void initQuotationDecision(std::string info);
	void initFixGrids(std::string info);
	void initRatioGrids(std::string info);
	void initDiffGrids(std::string info);

protected:
	APQuotationDecision* m_quotationDecision;

protected:
	APTradeDirection m_direction;

	double m_waitFloor;
	double m_waitCeil;

	int m_curIndex;
	int m_lastIndex;

	double m_concernedDelta;
	double m_ignoredDelta;

	std::vector<double> m_openValues;
	std::vector<double> m_closeValues;

	std::vector<APMonitorPriorityLevel> m_openPriorities;
	std::vector<APMonitorPriorityLevel> m_closePriorities;

	std::string m_targetName;
	double m_lastValue;
};

