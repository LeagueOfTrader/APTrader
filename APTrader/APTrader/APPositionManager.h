#pragma once
#include "Common/Singleton.h"
#include <map>
#include "Utils/APIntAccumulator.h"
#include "APTypes.h"

class APPositionCtrl;

class APPositionManager : public Singleton<APPositionManager>
{
public:
	APPositionManager();
	~APPositionManager();

	APPositionCtrl* createPositionCtrl(std::string positionInfo);
	APPositionCtrl* getPositionCtrl(UINT id);
	void removePositionCtrl(UINT id);
	void removePositionCtrl(APPositionCtrl* pc);

	void update();

	std::map<UINT, APPositionCtrl*>& getAllPositionCtrls();

private:
	std::map<UINT, APPositionCtrl*> m_positionCtrls;
	APIntAccumulator* m_idAccumulator;
};

