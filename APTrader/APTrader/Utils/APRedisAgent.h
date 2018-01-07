#pragma once
#include "../Common/Singleton.h"
//#include "../Common/Cyclebuffer.h"
#include <string>
#include "hiredis.h"
#include "../Common/ThreadSafeQueue.h"

class APRedisAgent : public Singleton<APRedisAgent>
{
public:
	APRedisAgent();
	~APRedisAgent();

	void init();

	void write(std::string key, std::string value);
	void writeAsync(std::string key, std::string value);
	std::string read(std::string key);

	void exit();

private:
	static void redisThreadFunc();
	void processWriteCmd();
	bool isOver();

private:
	bool m_over;
	redisContext* m_context;
	//CycleBuffer<const char*>* m_commandBuffer;
	threadsafe_queue<std::string> m_commandBuffer;
	int m_threadInterval;
};

