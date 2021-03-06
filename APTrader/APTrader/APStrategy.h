#pragma once
#include "APDef.h"
#include <string>
#include "Common/ParentChildrenHierarchy.h"
#include "Common/IdentifiedObject.h"
#include "APTypes.h"

class APPositionCtrl;
class APQuotationDecision;

enum APGridSectionType {
	GST_Open = 1,
	GST_Close = -1,
	GST_Indeterminate = 0
};

class APStrategy : public IdentifiedObject //: public ParentChildrenHierarchy<APStrategy>
{
public:
	APStrategy();
	~APStrategy();

	virtual void init(std::string strategyInfo);
	virtual void update() = 0;
	virtual void alert() = 0;
	virtual void exit();

	void setPriority(UINT priority);
	UINT getPriority();

	void setPositionCtrl(APPositionCtrl* posCtrl);
	APPositionCtrl* getPositionCtrl();

	virtual bool equals(APStrategy* strategy);

	void setInstrumentID(APASSETID instrumentID);

	void setCloseOnly(bool closeOnly);
	void setWork(bool work);

	bool isCloseOnly();
	bool isWork();

protected:
	void initQuotationDecision(std::string quotationInfo);	

	virtual void open(long volume);
	virtual void close(long volume);
	virtual void open(double price, long volume);
	virtual void close(double price, long volume);

protected:
	APASSETID m_instrumentID;

	APPositionCtrl* m_positionCtrl;
	APQuotationDecision* m_quotationDecision;

	UINT m_priority;
	bool m_isSubStrategy;
	UINT m_id;

	bool m_work;
	bool m_closeOnly;

	std::string m_tag;
};

