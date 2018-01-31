#include "APInputCommandParser.h"
#include "../Futures/APFuturesFramework.h"
#include "../Utils/APLog.h"
#include "../APPositionManager.h"
#include "../APPositionCtrl.h"
#include "../APStrategyManager.h"
#include "../APStrategy.h"

//std::vector<std::string> splitString(std::string srcStr, std::string delimStr)
//{
//	std::vector<std::string> resultStringVector;
//	std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c) {
//		if (delimStr.find(c) != std::string::npos) 
//		{ 
//			return true; 
//		} 
//		else { 
//			return false; 
//		}
//	}/*pred*/, delimStr.at(0));//将出现的所有分隔符都替换成为一个相同的字符（分隔符字符串的第一个）  
//	size_t pos = srcStr.find(delimStr.at(0));
//	std::string addedString = "";
//	while (pos != std::string::npos) {
//		addedString = srcStr.substr(0, pos);
//		if (!addedString.empty() || !repeatedCharIgnored) {
//			resultStringVector.push_back(addedString);
//		}
//		srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
//		pos = srcStr.find(delimStr.at(0));
//	}
//	addedString = srcStr;
//	if (!addedString.empty() || !repeatedCharIgnored) {
//		resultStringVector.push_back(addedString);
//	}
//	return resultStringVector;
//}

const UINT CMD_QUIT = 1;
const UINT CMD_RUN_STRATEGY = 10;
const UINT CMD_STOP_STRATEGY = 11;
const UINT CMD_SET_POSCTRL = 100;

std::map<std::string, UINT> APInputCommandParser::ms_commandMap;

std::vector<std::string> splitString(std::string strtem, char a)
{
	std::vector<std::string> strvec;

	std::string::size_type pos1, pos2;
	pos2 = strtem.find(a);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		strvec.push_back(strtem.substr(pos1, pos2 - pos1));

		pos1 = pos2 + 1;
		pos2 = strtem.find(a, pos1);
	}
	strvec.push_back(strtem.substr(pos1));
	return strvec;
}

void APInputCommandParser::init() 
{
	ms_commandMap["quit"] = CMD_QUIT;
	ms_commandMap["setpc"] = CMD_SET_POSCTRL;
	ms_commandMap["run"] = CMD_RUN_STRATEGY;
	ms_commandMap["stop"] = CMD_STOP_STRATEGY;
}

void APInputCommandParser::parseCommand(std::string cmd) 
{
	bool executed = false;
	std::vector<std::string> commands = splitString(cmd, ' ');
	if (ms_commandMap.find(commands[0]) != ms_commandMap.end()) {
		executed = processCommand(commands);
	}

	if(!executed)
	{
		APLogger->log("Invalid command:[ %s ].", cmd.c_str());
	}
}

bool APInputCommandParser::processCommand(std::vector<std::string> param) 
{
	bool ret = false;
	UINT cmdID = ms_commandMap[param[0]];
	switch (cmdID)
	{
	case CMD_QUIT:
		APFuturesFramework::getInstance()->terminate();
		ret = true;
		break;
	case CMD_SET_POSCTRL:
		if (param.size() > 3) {
			UINT posCtrlID = atoi(param[1].c_str());
			
			APPositionCtrl* posCtrl = APPositionManager::getInstance()->getPositionCtrl(posCtrlID);
			if (posCtrl == NULL) {
				ret = false;
				break;
			}

			std::string posItem = param[2];
			if (posItem == "h") {
				long vol = atoi(param[3].c_str());
				posCtrl->setHoldPosition(vol);
			}
			else if (posItem == "f") {
				long vol = atoi(param[3].c_str());
				posCtrl->setFrozenPosition(vol);
			}
			else if (posItem == "order") {
				APORDERID orderID = param[3];
				posCtrl->relateOrder(orderID);
			}
			ret = true;
		}
		break;
	case CMD_RUN_STRATEGY:
		if (param.size() > 2) {
			std::string strategyName = param[1];
			APStrategy* pStrategy = APStrategyManager::getInstance()->getStrategy(strategyName);
			if (pStrategy == NULL) {
				ret = false;
				break;
			}
			pStrategy->setWork(true);
		}
		else {
			APStrategyManager::getInstance()->runAllStrategies();			
		}
		ret = true;
		break;
	case CMD_STOP_STRATEGY:
		if (param.size() > 2) {
			std::string strategyName = param[1];
			APStrategy* pStrategy = APStrategyManager::getInstance()->getStrategy(strategyName);
			if (pStrategy == NULL) {
				ret = false;
				break;
			}
			pStrategy->setWork(false);
		}
		else {
			APStrategyManager::getInstance()->stopAllStrategies();
		}
		ret = true;
		break;
	default:
		break;
	}

	return ret;
}
