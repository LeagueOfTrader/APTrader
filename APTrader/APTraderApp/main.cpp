#include "Strategy\APStandardGridStrategy.h"
#include "APFramework.h"

void main() {
	APStandardGridStrategy* gs = new APStandardGridStrategy();
	gs->init("grid1.cfg");
	gs->printGrids();
}

//void mainLoop() {
//	APFramework* frame = new APFramework();
//	frame->init();
//	while (1) {
//		frame->update();
//	}
//	frame->exit();
//
//	delete frame;
//}