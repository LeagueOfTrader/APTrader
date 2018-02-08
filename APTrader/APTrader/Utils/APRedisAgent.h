#pragma once
#include "../Common/Singleton.h"
#include "../Common/InitializableObject.h"
//#include "../Common/Cyclebuffer.h"
#include <string>
#include "hiredis.h"
#include "../Common/ThreadSafeQueue.h"

class APRedisAgent : public InitializableObject, public Singleton<APRedisAgent>
{
public:
	APRedisAgent();
	~APRedisAgent();

	virtual void init();

	void write(std::string key, std::string value);
	void writeAsync(std::string key, std::string value);
	std::string read(std::string key);

	bool hasKey(std::string key);

	virtual void exit();

private:
	static void redisThreadFunc();
	void processWriteCmd();

private:
	redisContext* m_context;
	//CycleBuffer<const char*>* m_commandBuffer;
	threadsafe_queue<std::string> m_commandBuffer;
	int m_threadInterval;
	bool m_quit;
};

