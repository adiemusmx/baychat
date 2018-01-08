#pragma once

#include "basic_types.h"
#include <mutex>

namespace BayChat
{

/*
 * �������ݻ���
 */
class VoiceData
{
public:
	VoiceData(size_t sizeOfBuffer);
	virtual ~VoiceData();

	/* �������ݵ�buffer */
	BYTE * m_buffer;

	/* ��������Buffer����Ч���ݵĳ��� */
	size_t m_validSize;
};

/*
 * ���Mic�豸��Ϣ
 */
class MicDeviceAdapter
{
public:
	MicDeviceAdapter();
	virtual ~MicDeviceAdapter();

	/* ����/�رջػ� */
	virtual void openSession() = 0;
	virtual void read(size_t timeInMillisecond) = 0;
	virtual void closeSession() = 0;
	virtual BOOL isInSession() = 0;

	/* Session��õ����ݣ�Ĭ��10K�Ŀռ䣬Լ1000ms�� */
	virtual void resizeBuffer(size_t sizeOfBuffer);
	virtual const VoiceData * getBuffer();
	virtual size_t getSizeOfBuffer();

protected:
	/* �� */
	std::mutex* m_mutex;

	/* ���� */
	VoiceData * m_buffer;
	size_t m_sizeOfBuffer;
};

/*
 * wave in ϵ��Adapter
 */
class MicDeviceAdapter_Wave : public MicDeviceAdapter
{
public:
	MicDeviceAdapter_Wave();
	virtual ~MicDeviceAdapter_Wave();

	/* ����/�رջػ� */
	virtual void openSession();
	virtual void read(size_t timeInMillisecond);
	virtual void closeSession();
	virtual BOOL isInSession();

private:
	HWAVEIN m_hWaveIn;
	WAVEFORMATEX m_waveform;
};

}