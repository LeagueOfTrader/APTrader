#include "APFuturesFramework.h"


//#ifndef SIMNOW
//#define SIMNOW
//#endif
//
//std::string mdflowpath = "Data/CTP/mdflow/md.con";
//std::string ctpCfgFile = "Data/CTP/CTPAccount.cfg";
//
//#ifdef SIMNOW
//std::string ctpParamFile = "Data/CTP/CTPTestParam.cfg";
//#else
//std::string ctpParamFile = "Data/CTP/CTPParam.cfg";
//#endif

APFuturesFramework::APFuturesFramework()
{	
	m_finished = false;
}

APFuturesFramework::~APFuturesFramework()
{
}

void APFuturesFramework::init() 
{
}

void APFuturesFramework::update(float deltaTime)
{
}

void APFuturesFramework::exit()
{
}

bool APFuturesFramework::finished()
{
	return m_finished;
}




