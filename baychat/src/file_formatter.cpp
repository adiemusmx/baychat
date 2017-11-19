#include "stdafx.h"
#include "file_formatter.h"

namespace BayChat
{

AudioFormatter::AudioFormatter()
{
}


AudioFormatter::~AudioFormatter()
{
}

BOOL AudioFormatter::pcm2wav(const WCHAR * pcmFileName, const WCHAR * wavFileName)
{
	typedef struct WAVE_HEADER
	{
		char    fccID[4];       //内容为""RIFF
		unsigned long dwSize;   //最后填写，WAVE格式音频的大小
		char    fccType[4];     //内容为"WAVE"
	} WAVE_HEADER;

	typedef struct WAVE_FMT
	{
		char    fccID[4];          //内容为"fmt "
		unsigned long  dwSize;     //内容为WAVE_FMT占的字节数，为16
		unsigned short wFormatTag; //如果为PCM，改值为 1
		unsigned short wChannels;  //通道数，单通道=1，双通道=2
		unsigned long  dwSamplesPerSec;//采用频率
		unsigned long  dwAvgBytesPerSec;/* ==dwSamplesPerSec*wChannels*uiBitsPerSample/8 */
		unsigned short wBlockAlign;//==wChannels*uiBitsPerSample/8
		unsigned short uiBitsPerSample;//每个采样点的bit数，8bits=8, 16bits=16
	} WAVE_FMT;

	typedef struct WAVE_DATA
	{
		char    fccID[4];       //内容为"data"
		unsigned long dwSize;   //==NumSamples*wChannels*uiBitsPerSample/8
	} WAVE_DATA;

	uint32 channels = 2;
	uint32 sample_rate = 44100;
	int bits = 16;

	WAVE_HEADER pcmHEADER;
	WAVE_FMT    pcmFMT;
	WAVE_DATA   pcmDATA;

	unsigned short m_pcmData;
	File pcmFile, wavFile;

	pcmFile.open(pcmFileName, FileMode_readBinary);
	if(!pcmFile.isValid())
	{
		LOGGER_WARNING_LOG("open pcm file failed.");
		return FALSE;
	}

	wavFile.open(wavFileName, FileMode_readAndWriteBinary);
	if(!wavFile.isValid())
	{
		LOGGER_WARNING_LOG("open wav file failed.");
		return FALSE;
	}

	/* WAVE_HEADER */
	memcpy(pcmHEADER.fccID, "RIFF", strlen("RIFF"));
	memcpy(pcmHEADER.fccType, "WAVE", strlen("WAVE"));
	pcmFile.seek(sizeof(WAVE_HEADER), FileSeekMode_cur);

	/* WAVE_FMT */
	memcpy(pcmFMT.fccID, "fmt ", strlen("fmt "));
	pcmFMT.dwSize = 16;
	pcmFMT.wFormatTag = WAVE_FORMAT_PCM;
	pcmFMT.wChannels = 1;
	pcmFMT.dwSamplesPerSec = 8000;
	pcmFMT.uiBitsPerSample = 16;
	pcmFMT.dwAvgBytesPerSec = pcmFMT.dwSamplesPerSec * pcmFMT.wChannels * pcmFMT.uiBitsPerSample / 8;
	pcmFMT.wBlockAlign = pcmFMT.wChannels * pcmFMT.uiBitsPerSample / 8;

	pcmFile.write(&pcmFMT, sizeof(WAVE_FMT), FileSeekMode_cur);

	/* WAVE_DATA */
	memcpy(pcmDATA.fccID, "data", strlen("data"));
	pcmDATA.dwSize = 0;
	wavFile.seek(sizeof(WAVE_DATA), FileSeekMode_cur);

	pcmFile.read(&m_pcmData, sizeof(unsigned short), FileSeekMode_cur);

	while(!pcmFile.isEndOfFile())
	{
		pcmDATA.dwSize += 2;
		wavFile.write(&m_pcmData, sizeof(unsigned short), FileSeekMode_cur);
		pcmFile.read(&m_pcmData, sizeof(unsigned short), FileSeekMode_cur);
	}

	pcmHEADER.dwSize = 44 + pcmDATA.dwSize;

	wavFile.rewind();

	wavFile.write(&pcmHEADER, sizeof(WAVE_HEADER), FileSeekMode_cur);
	wavFile.seek(sizeof(WAVE_FMT), FileSeekMode_cur);
	wavFile.write(&pcmDATA, sizeof(WAVE_DATA), FileSeekMode_cur);

	pcmFile.close();
	wavFile.close();
}

}