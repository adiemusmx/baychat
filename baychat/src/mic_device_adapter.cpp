#include "stdafx.h"
#include "mic_device_adapter.h"

#define MIC_DEVICE_ADAPTER_DEFAULT_SIZE 320000

namespace BayChat
{

VoiceData::VoiceData(size_t sizeOfBuffer)
{
	m_buffer = new BYTE[sizeOfBuffer];
	m_validSize = 0;
}

VoiceData::~VoiceData()
{
	if (m_buffer != NULL)
	{
		delete[] m_buffer;
		m_buffer = NULL;
	}
}

MicDeviceAdapter::MicDeviceAdapter()
{
	m_sizeOfBuffer = MIC_DEVICE_ADAPTER_DEFAULT_SIZE;
	m_buffer = new VoiceData(m_sizeOfBuffer);
	m_mutex = new Mutex();
}

MicDeviceAdapter::~MicDeviceAdapter()
{
	if (m_buffer != NULL)
	{
		delete m_buffer;
		m_buffer = NULL;
	}

	if (m_mutex != NULL)
	{
		delete m_mutex;
		m_mutex = NULL;
	}
}

void MicDeviceAdapter::resizeBuffer(size_t sizeOfBuffer)
{
	LOGGER_TRACE_LOG("%llu", sizeOfBuffer);

	m_mutex->lock();

	if (m_buffer)
	{
		delete[] m_buffer;
	}
	m_sizeOfBuffer = sizeOfBuffer;
	m_buffer = new VoiceData(m_sizeOfBuffer);

	m_mutex->unlock();
}

const VoiceData * MicDeviceAdapter::getBuffer()
{
	return m_buffer;
}

size_t MicDeviceAdapter::getSizeOfBuffer()
{
	return m_sizeOfBuffer;
}

MicDeviceAdapter_Wave::MicDeviceAdapter_Wave()
{
	m_hWaveIn = NULL;
	m_waveform.wFormatTag = WAVE_FORMAT_PCM;		// ������ʽΪPCM
	m_waveform.nSamplesPerSec = 16000;				// �����ʣ�16000��/��
	m_waveform.wBitsPerSample = 8;					// �������أ�16bits/��
	m_waveform.nChannels = 1;						// ������������������
	m_waveform.nAvgBytesPerSec = 16000;				// ÿ��������ʣ�����ÿ���ܲɼ������ֽڵ�����
	m_waveform.nBlockAlign = (m_waveform.nChannels * m_waveform.wBitsPerSample) / 8; // һ����Ĵ�С������bit���ֽ�������������
	m_waveform.cbSize = 0;							// һ��Ϊ0
}

MicDeviceAdapter_Wave::~MicDeviceAdapter_Wave()
{
}

void MicDeviceAdapter_Wave::openSession()
{
	LOGGER_TRACE_LOG_START();
	m_mutex->lock();

	//ʹ��waveInOpen����������Ƶ�ɼ�
	HANDLE wait = CreateEvent(NULL, 0, 0, NULL);
	if (m_hWaveIn == NULL)
	{
		waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);
	}

	m_mutex->unlock();
	LOGGER_TRACE_LOG_END();
}

void MicDeviceAdapter_Wave::read(size_t timeInMillisecond)
{
	LOGGER_TRACE_LOG_START();
	LOGGER_TRACE_LOG("time[%llu]", timeInMillisecond);

	m_mutex->lock();

	WAVEHDR wHdr1; //�ɼ���Ƶʱ�������ݻ���Ľṹ��
	wHdr1.lpData = (LPSTR)m_buffer->m_buffer;
	wHdr1.dwBufferLength = (DWORD)getSizeOfBuffer();
	wHdr1.dwBytesRecorded = 0;
	wHdr1.dwUser = 0;
	wHdr1.dwFlags = 0;
	wHdr1.dwLoops = 1;
	waveInPrepareHeader(m_hWaveIn, &wHdr1, sizeof(WAVEHDR));//׼��һ���������ݿ�ͷ����¼��
	waveInAddBuffer(m_hWaveIn, &wHdr1, sizeof (WAVEHDR));//ָ���������ݿ�Ϊ¼�����뻺��
	waveInStart(m_hWaveIn);//��ʼ¼��
	Sleep((DWORD)timeInMillisecond);//�ȴ�����¼��1s
	waveInReset(m_hWaveIn);//ֹͣ¼��

	m_buffer->m_validSize = wHdr1.dwBytesRecorded;
	if (m_buffer->m_validSize >= wHdr1.dwBufferLength)
	{
		LOGGER_WARNING_LOG("Some Data May Lost.");
	}

	m_mutex->unlock();
	LOGGER_TRACE_LOG_END();
}

void MicDeviceAdapter_Wave::closeSession()
{
	LOGGER_TRACE_LOG_START();
	m_mutex->lock();

	if (m_hWaveIn != NULL)
	{
		waveInClose(m_hWaveIn);
		m_hWaveIn = NULL;
	}

	m_mutex->unlock();
	LOGGER_TRACE_LOG_END();
}

BOOL MicDeviceAdapter_Wave::isInSession()
{
	m_mutex->lock();
	BOOL ret = (m_hWaveIn != NULL);
	m_mutex->unlock();
	return ret;
}

}