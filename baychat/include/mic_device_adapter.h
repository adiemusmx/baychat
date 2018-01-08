#pragma once

#include "basic_types.h"
#include <mutex>

namespace BayChat
{

/*
 * 声音数据缓存
 */
class VoiceData
{
public:
	VoiceData(size_t sizeOfBuffer);
	virtual ~VoiceData();

	/* 声音数据的buffer */
	BYTE * m_buffer;

	/* 声音数据Buffer中有效数据的长度 */
	size_t m_validSize;
};

/*
 * 输出Mic设备信息
 */
class MicDeviceAdapter
{
public:
	MicDeviceAdapter();
	virtual ~MicDeviceAdapter();

	/* 开启/关闭回话 */
	virtual void openSession() = 0;
	virtual void read(size_t timeInMillisecond) = 0;
	virtual void closeSession() = 0;
	virtual BOOL isInSession() = 0;

	/* Session获得的数据（默认10K的空间，约1000ms） */
	virtual void resizeBuffer(size_t sizeOfBuffer);
	virtual const VoiceData * getBuffer();
	virtual size_t getSizeOfBuffer();

protected:
	/* 锁 */
	std::mutex* m_mutex;

	/* 缓存 */
	VoiceData * m_buffer;
	size_t m_sizeOfBuffer;
};

/*
 * wave in 系列Adapter
 */
class MicDeviceAdapter_Wave : public MicDeviceAdapter
{
public:
	MicDeviceAdapter_Wave();
	virtual ~MicDeviceAdapter_Wave();

	/* 开启/关闭回话 */
	virtual void openSession();
	virtual void read(size_t timeInMillisecond);
	virtual void closeSession();
	virtual BOOL isInSession();

private:
	HWAVEIN m_hWaveIn;
	WAVEFORMATEX m_waveform;
};

}