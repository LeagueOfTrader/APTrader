#pragma once
class APFundPool
{
public:
	APFundPool();
	~APFundPool();

	void syncFunds();

private:
	double m_totalFund;
	double m_availableFund;
	double m_frozenFund;

	//
};

