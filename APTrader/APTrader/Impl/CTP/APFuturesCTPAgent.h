#pragma once
#include <string>
#include <mutex>
#include "../../Utils/APIntAccumulator.h"

typedef void(*InitCallback)();

class APFuturesCTPAgent
{
public:
	APFuturesCTPAgent();
	~APFuturesCTPAgent();

	virtual void init();
	virtual void initCTP();
	virtual void login();

	void onLogin();
	bool isLogin();

	void registerInitCallback(InitCallback callback);

	std::string getUserID();
	std::string getBrokerID();

protected:
	void readAccount();
	void readParam();
	int genReqID();

protected:
	std::string m_brokerID;
	std::string m_userID;
	std::string m_password;

	std::string m_tradeFront;
	std::string m_marketFront;

	APIntAccumulator* m_idAccumulator;

	bool m_loginSuccess;

	InitCallback m_initCallback;

	std::mutex m_mutex;
};

