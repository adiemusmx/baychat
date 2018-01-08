// baychat.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "mic_device_adapter.h"
#include "file_formatter.h"
#include "asr_service.h"
#include "input_helper.h"

using namespace BayChat;

int main()
{
	Logger::instance()->setLevel(LogLevel_trace);
	Logger::instance()->setFilePath(L"log");
	Logger::instance()->setFileEnable(TRUE);

	InputHelper::instance()->run();

	while (TRUE);

	return 0;
}