// baychat.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "mic_device_adapter.h"
#include "file_formatter.h"

using namespace BayChat;

int main()
{
	MicDeviceAdapter* adapter = new MicDeviceAdapter_Wave();

	File file;
	file.open(L"input.pcm", FileMode_readAndWrite);

	adapter->openSession();
	LOGGER_INFO_LOG("Open Session.");

	int i = 5;
	while (i--)
	{
		LOGGER_TRACE_LOG("second [%d]", i);
		adapter->read(1000);
		file.write(adapter->getBuffer()->m_buffer, sizeof(BYTE), adapter->getBuffer()->m_validSize);
	}

	LOGGER_INFO_LOG("Close Session.");
	adapter->closeSession();

	file.close();

	LOGGER_INFO_LOG("Convert File Format.");
	AudioFormatter::pcm2wav(L"input.pcm", L"output.wav");

	return 0;
}

