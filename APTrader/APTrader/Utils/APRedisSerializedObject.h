#pragma once
#include "../Common/SerializedObject.h"
#include <string>

class APRedisSerializedObject : public SerializedObject
{
public:
	virtual void archive(std::string key);
	virtual void unarchive(std::string key);

	virtual bool load();
	virtual void save();

protected:
	virtual std::string generateRedisKey();
};

