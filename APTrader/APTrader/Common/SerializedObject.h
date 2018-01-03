#pragma once
#include <string>
#include <fstream>

class SerializedObject
{
public:
	virtual void archive(std::string filename) {
		std::string serializedData = serialize();
		std::ofstream s(filename);
		s << serializedData;
		s.close();
	}

	virtual void unarchive(std::string filename) {
		std::ifstream s(filename);
		std::string str((std::istreambuf_iterator<char>(s)),
			std::istreambuf_iterator<char>());

		deserialize(str);
		s.close();
	}

protected:
	virtual std::string serialize() = 0;
	virtual void deserialize(std::string str) = 0;
};

