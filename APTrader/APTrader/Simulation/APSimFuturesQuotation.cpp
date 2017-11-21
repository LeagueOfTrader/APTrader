#include "APSimFuturesQuotation.h"
#include <fstream>
#include <iostream>
#include "../Utils/APLog.h"

std::string simDataPath = "Data/Sim/";

void splitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2) {
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}

	if (pos1 != s.length()) {
		v.push_back(s.substr(pos1));
	}
}

APSimFuturesQuotation::APSimFuturesQuotation(APASSETID commodityID) : APFuturesQuotation(commodityID)
{
	m_loop = false;
	m_finished = false;
	init(commodityID);
}


APSimFuturesQuotation::~APSimFuturesQuotation()
{
}

void APSimFuturesQuotation::queryQuotation()
{
	if (m_finished) {
		return;
	}

	m_curPrice = m_simPrices[m_curIndex++];
	APLogger->log("Sim Quotation, id:%s, price:%f. ", m_commodityID.c_str(), m_curPrice);
	if (m_curIndex >= m_simPrices.size()) {
		if (m_loop) {
			m_curIndex = 0;
		}
		else {
			m_finished = true;
		}
	}
}

int APSimFuturesQuotation::getSimPricesCount()
{
	return m_simPrices.size();
}

void APSimFuturesQuotation::init(APASSETID commodityID)
{
	m_curIndex = 0;

	std::string filename = simDataPath + commodityID + ".txt";

	std::fstream file;
	file.open(filename, std::ios::in, _SH_DENYNO);
	std::string content;
	file >> content;
	std::vector<std::string> data;
	splitString(content, data, ",");

	file.close();

	for (int i = 0; i < data.size(); i++) {
		m_simPrices.push_back(atof(data[i].c_str()));
	}
}


