#include "APQuotationMonitor.h"
#include "../APQuotationDecision.h"
#include "../APGlobalConfig.h"
#include "../Utils/APJsonReader.h"
#include <sstream>  
#include "../Utils/APLog.h"
#include "../APObjectFactory.h"

const double MONITOR_IGNORE_VAL = 0.001;
const double MONITOR_SINGLE_INST_UNIT_PRICE = 1.0;

void buildEqualRatioArray(std::vector<double>& arr, double baseVal, double ratio, int count) {
	arr.clear();
	double accumVal = 1.0;
	for (int i = 0; i < count; i++) {
		double val = accumVal * baseVal;
		arr.push_back(val);
		accumVal *= ratio;
	}
}

void buildEqualDiffArray(std::vector<double>& arr, double baseVal, double diff, int count) {
	double val = baseVal;
	for (int i = 0; i < count; i++) {
		arr.push_back(val);
		val += diff;
	}
}

APQuotationMonitor::APQuotationMonitor()
{
	m_quotationDecision = NULL;
	m_lastIndex = INT_MAX;

	m_concernedDelta = MONITOR_SINGLE_INST_UNIT_PRICE;
	m_ignoredDelta = MONITOR_IGNORE_VAL;
}


APQuotationMonitor::~APQuotationMonitor()
{
}

void APQuotationMonitor::init(std::string filename)
{
	APJsonReader jr;
	jr.initWithFile(filename);

	if (jr.hasMember("Quotation")) {
		std::string quotationInfo = jr.getObjValue("Quotation");
		initQuotationDecision(quotationInfo);
	}

	APMonitorGridType gt = (APMonitorGridType)jr.getIntValue("Type");
	std::string gridsInfo = jr.getObjValue("Grids");
	if (jr.hasMember("TargetName")) {
		m_targetName = jr.getStrValue("TargetName");
	}

	if (jr.hasMember("ConcernedDelta")) {
		m_concernedDelta = jr.getDoubleValue("ConcernedDelta");
	}

	if (jr.hasMember("IgnoredDelta")) {
		m_ignoredDelta = jr.getDoubleValue("IgnoredDelta");
	}

	switch (gt) {
	case MGT_Fix:
		initFixGrids(gridsInfo);
		break;
	case MGT_Diff:
		initDiffGrids(gridsInfo);
		break;
	case MGT_Ratio:
		initRatioGrids(gridsInfo);
		break;
	default:
		break;
	}
}

void APQuotationMonitor::update()
{
	if (m_quotationDecision != NULL) {
		double curValue = m_quotationDecision->getValueReference();
		if (curValue < 0.0) {
			return;
		}

		m_curIndex = 0;
		goGrids(curValue);

		if (m_curIndex == 0 && m_lastIndex == INT_MAX) {
			std::stringstream ss;
			ss << "[" << m_targetName << "], INIT: " << curValue;
			APLogger->log(ss.str().c_str());
		}

		m_lastIndex = m_curIndex;
	}
}

void APQuotationMonitor::goGrids(double value)
{
	double price = 0.0;
	if (inCloseSection(value)) {
		int i = getCurTrendGridIndex(m_closeValues, value, false);
		if (i < 0) {
			return;
			//i = 0;
		}

		m_curIndex = i + 1;

		std::stringstream ss;
		ss << "[" << m_targetName << "], CLOSE: ";
		payAttention(ss.str(), value, m_closePriorities[i]);
	}
	else if (inOpenSection(value)) {

		int i = getCurTrendGridIndex(m_openValues, value, true);

		if (i >= m_openValues.size()) {
			return;
		}

		m_curIndex = -(i + 1);

		std::stringstream ss;
		ss << "[" << m_targetName << "], OPEN: ";
		payAttention(ss.str(), value, m_openPriorities[i]);
	}
	//else {
	//	m_curIndex = 0;
	//}
}

bool APQuotationMonitor::inOpenSection(double value)
{
	if (m_direction == TD_Buy) {
		return value < m_waitFloor;
	}
	else if (m_direction == TD_Sell) {
		return value > m_waitCeil;
	}

	return false;
}

bool APQuotationMonitor::inCloseSection(double value)
{
	if (m_direction == TD_Buy) {
		return value >= m_waitCeil;
	}
	else if (m_direction == TD_Sell) {
		return value <= m_waitFloor;
	}

	return false;
}

int APQuotationMonitor::getGridIndex(std::vector<double>& grids, double value, bool reverse)
{
	int i = 0;
	if (!reverse) {
		for (i = 0; i < grids.size(); i++) {
			if (value - grids[i] > 0.0f) {
				break;
			}
		}
		if (i > 0) {
			i--;
		}
	}
	else {
		for (i = grids.size() - 1; i >= 0; i--) {
			if (grids[i] - value < APGlobalConfig::getInstance()->getNearByPrice()) {
				break;
			}
		}
	}
	return i;
}

int APQuotationMonitor::getCurTrendGridIndex(std::vector<double>& grids, double value, bool open)
{
	bool reverse = false;
	if ((m_direction == TD_Buy && open == false)
		|| (m_direction == TD_Sell && open == true)) {
		reverse = true;
	}
	return getGridIndex(grids, value, reverse);
}

void APQuotationMonitor::payAttention(std::string content, double value, APMonitorPriorityLevel lv)
{
	bool bOutput = false;

	if (lv == MPL_Urgent) {
		bOutput = true;
	}
	else if (m_curIndex != m_lastIndex) {
		bOutput = true;
	}
	else if (fabs(value - m_lastValue) > m_concernedDelta - DBL_EPSILON) //MONITOR_IGNORE_VAL) 
	{
		bOutput = true;
	}
	else if(fabs(value - m_lastValue) > m_ignoredDelta)
	{
		if (lv == MPL_Important) {
			bOutput = true;
		}
	}

	if (bOutput) {
		std::stringstream ss;
		ss << " " << value;
		APLogger->log((content + ss.str()).c_str());

		m_lastValue = value;
	}
}

void APQuotationMonitor::initQuotationDecision(std::string info)
{
	APJsonReader jr;
	jr.initWithString(info);
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

void APQuotationMonitor::initFixGrids(std::string info)
{
	APJsonReader jr;
	jr.initWithString(info);
	std::string strDirection = jr.getStrValue("Direction");
	if (strDirection == "Buy") {
		m_direction = TD_Buy;
	}
	else if (strDirection == "Sell") {
		m_direction = TD_Sell;
	}

	m_waitCeil = jr.getDoubleValue("Ceil");
	m_waitFloor = jr.getDoubleValue("Floor");
	int count = jr.getIntValue("GridsCount");

	m_openValues.clear();
	m_closeValues.clear();
	for (int i = 0; i < count; i++) {
		m_openValues.push_back(jr.getArrayDoubleValue("OpenVals", i));
		m_closeValues.push_back(jr.getArrayDoubleValue("CloseVals", i));
		m_openPriorities.push_back((APMonitorPriorityLevel)jr.getArrayIntValue("OpenPriorities", i));
		m_closePriorities.push_back((APMonitorPriorityLevel)jr.getArrayIntValue("ClosePriorities", i));
	}
}

void APQuotationMonitor::initRatioGrids(std::string info)
{
	APJsonReader jr;
	jr.initWithString(info);
	std::string strDirection = jr.getStrValue("Direction");
	if (strDirection == "Buy") {
		m_direction = TD_Buy;
	}
	else if (strDirection == "Sell") {
		m_direction = TD_Sell;
	}

	m_waitCeil = jr.getDoubleValue("Ceil");
	m_waitFloor = jr.getDoubleValue("Floor");
	double percent = jr.getDoubleValue("Percent");
	int count = jr.getIntValue("GridsCount");
	double longVal = jr.getDoubleValue("LongVal");
	double shortVal = jr.getDoubleValue("ShortVal");

	if (m_direction == TD_Buy) {
		buildEqualRatioArray(m_closeValues, longVal, 1.0 + percent, count);
		buildEqualRatioArray(m_openValues, shortVal, 1.0 - percent, count);
	}
	else if (m_direction == TD_Sell) {
		buildEqualRatioArray(m_openValues, longVal, 1.0 + percent, count);
		buildEqualRatioArray(m_closeValues, shortVal, 1.0 - percent, count);
	}

	int imptIdx = jr.getIntValue("ImportantIndex");
	for (int i = 0; i < count; i++) {
		APMonitorPriorityLevel pl = MPL_Normal;
		if (i >= imptIdx) {
			pl = MPL_Important;
		}
		m_openPriorities.push_back(pl);
		m_closePriorities.push_back(pl);
	}
}

void APQuotationMonitor::initDiffGrids(std::string info)
{
	APJsonReader jr;
	jr.initWithString(info);
	std::string strDirection = jr.getStrValue("Direction");
	if (strDirection == "Buy") {
		m_direction = TD_Buy;
	}
	else if (strDirection == "Sell") {
		m_direction = TD_Sell;
	}

	m_waitCeil = jr.getDoubleValue("Ceil");
	m_waitFloor = jr.getDoubleValue("Floor");
	double diff = jr.getDoubleValue("Diff");
	int count = jr.getIntValue("GridsCount");
	double longVal = jr.getDoubleValue("LongVal");
	double shortVal = jr.getDoubleValue("ShortVal");

	if (m_direction == TD_Buy) {
		buildEqualDiffArray(m_openValues, shortVal, -diff, count);
		buildEqualDiffArray(m_closeValues, longVal, diff, count);
	}
	else if (m_direction == TD_Sell) {
		buildEqualDiffArray(m_closeValues, shortVal, -diff, count);
		buildEqualDiffArray(m_openValues, longVal, diff, count);
	}

	int imptIdx = jr.getIntValue("ImportantIndex");
	for (int i = 0; i < count; i++) {
		APMonitorPriorityLevel pl = MPL_Normal;
		if (i >= imptIdx) {
			pl = MPL_Important;
		}
		m_openPriorities.push_back(pl);
		m_closePriorities.push_back(pl);
	}
}

