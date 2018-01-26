#pragma once
class APInstrumentQuotation;

class APQuotationDecision
{
public:
	APQuotationDecision();
	APQuotationDecision(double upper, double lower);
	~APQuotationDecision();

	virtual double getValueReference() = 0;
	virtual bool isValueRefValid(double valRef);

	virtual APInstrumentQuotation* getQuotationByIndex(int index = 0) = 0;

protected:
	double m_upperLimit;
	double m_lowerLimit;
};

