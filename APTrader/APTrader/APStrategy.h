#pragma once
#include "APDef.h"
#include <string>
#include "Common/ParentChildrenHierarchy.h"
#include "APTypes.h"

class APPositionCtrl;
class APQuotationDecision;

class APStrategy : public ParentChildrenHierarchy<APStrategy>
{
public:
	APStrategy();
	~APStrategy();

	virtual void init(std::string strategyInfo);
	virtual void update() = 0;
	virtual void alert() = 0;

	void setPriority(UINT priority);
	UINT getPriority();

	APPositionCtrl* getPositionCtrl();

	virtual bool equals(APStrategy* strategy);

	UINT getID();
	void setID(UINT id);

	void setInstrumentID(APASSETID instrumentID);

	void setCloseOnly(bool closeOnly);

protected:
	void initQuotationDecision(std::string quotationInfo);	

protected:
	APASSETID m_instrumentID;

	APPositionCtrl* m_positionCtrl;
	APQuotationDecision* m_quotationDecision;

	UINT m_priority;
	bool m_isSubStrategy;
	UINT m_id;

	bool m_closeOnly;
};

