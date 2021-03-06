// APTraderCmdLine.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "System/Win32/NamedPipeSender.h"
#include "Input/APInputCmdDef.h"

const int BUFFER_MAX_LEN = 1024;

int main()
{	
	char buf[BUFFER_MAX_LEN];
	NamedPipeSender* pipeSender = new NamedPipeSender("APTrader_CmdLine");
	pipeSender->init();
	bool quit = false;
	if (pipeSender == NULL || pipeSender->inited() == false) {
		printf("命令行初始化失败！\n");
		return -1;
	}

	printf("命令行初始化完毕，APTrader启动完毕后输入指令\n");
	while (!quit)
	{
		
		DWORD dwLen = 0;
		int bufSize;
		for (bufSize = 0; bufSize < BUFFER_MAX_LEN; bufSize++) {
			buf[bufSize] = getchar();
			if (buf[bufSize] == '\n') break;
		}
		//向服务端发送数据  
		if (pipeSender->send(buf, bufSize, dwLen)) {
			//printf("数据写入完毕共%d字节\n", dwLen);
			printf("----命令已发送----\n");
		}
		else
		{
			printf("----命令发送失败----\n");
		}

		if (strcmp(buf, CMD_QUIT_STR) == 0) {
			quit = true;
		}

		Sleep(1000);
	}

	pipeSender->exit();
    return 0;
}

