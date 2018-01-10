#pragma once
class APQuotationDecision
{
public:
	APQuotationDecision();
	APQuotationDecision(double upper, double lower);
	~APQuotationDecision();

	virtual double getValueReference() = 0;
	virtual bool isValueRefValid(double valRef);

protected:
	double m_upperLimit;
	double m_lowerLimit;
};

