#include "APIntAccumulator.h"



APIntAccumulator::APIntAccumulator()
{
	m_id = 0;
}


APIntAccumulator::~APIntAccumulator()
{
}

UINT APIntAccumulator::generateID() {
	return ++m_id;
}
