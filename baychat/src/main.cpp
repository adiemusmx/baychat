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
	file.open(L"D:/pcm/input.pcm", FileMode_readAndWrite);

	adapter->openSession();
	int i = 5;
	while (i--)
	{
		LOGGER_INFO_LOG("second [%d]", i);
		adapter->read(1000);
		file.write(adapter->getBuffer()->m_buffer, sizeof(BYTE), adapter->getBuffer()->m_validSize);
	}
	adapter->closeSession();

	file.close();

	AudioFormatter::pcm2wav(L"D:/pcm/input.pcm", L"D:/pcm/output.wav");

	return 0;
}

