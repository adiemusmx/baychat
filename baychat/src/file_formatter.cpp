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

extern "C" {
	typedef struct WAVE_HEADER
	{
		char    fccID[4];       //����Ϊ""RIFF
		unsigned long dwSize;   //�����д��WAVE��ʽ��Ƶ�Ĵ�С
		char    fccType[4];     //����Ϊ"WAVE"
	} WAVE_HEADER;

	typedef struct WAVE_FMT
	{
		char    fccID[4];          //����Ϊ"fmt "
		unsigned long  dwSize;     //����ΪWAVE_FMTռ���ֽ�����Ϊ16
		unsigned short wFormatTag; //���ΪPCM����ֵΪ 1
		unsigned short wChannels;  //ͨ��������ͨ��=1��˫ͨ��=2
		unsigned long  dwSamplesPerSec;//����Ƶ��
		unsigned long  dwAvgBytesPerSec;/* ==dwSamplesPerSec*wChannels*uiBitsPerSample/8 */
		unsigned short wBlockAlign;//==wChannels*uiBitsPerSample/8
		unsigned short uiBitsPerSample;//ÿ���������bit����8bits=8, 16bits=16
	} WAVE_FMT;

	typedef struct WAVE_DATA
	{
		char    fccID[4];       //����Ϊ"data"
		unsigned long dwSize;   //==NumSamples*wChannels*uiBitsPerSample/8
	} WAVE_DATA;
}

BOOL AudioFormatter::pcm2wav(const WCHAR * pcmFileName, const WCHAR * wavFileName)
{
	WAVE_HEADER pcmHEADER;
	WAVE_FMT    pcmFMT;
	WAVE_DATA   pcmDATA;

	File pcmFile, wavFile;

	memset(&pcmHEADER, 0x00, sizeof(pcmHEADER));
	memset(&pcmFMT, 0x00, sizeof(pcmFMT));
	memset(&pcmDATA, 0x00, sizeof(pcmDATA));

	pcmFile.open(pcmFileName, FileMode_readBinary);
	if(!pcmFile.isValid())
	{
		LOGGER_WARNING_LOG("open pcm file failed.");
		return FALSE;
	}

	// ɾ�����ļ�
	if (File::isExist(wavFileName))
	{
		File::remove(wavFileName);
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
	wavFile.seek(sizeof(WAVE_HEADER), FileSeekMode_cur);

	/* WAVE_FMT */
	memcpy(pcmFMT.fccID, "fmt ", strlen("fmt "));
	pcmFMT.dwSize = 16;
	pcmFMT.wFormatTag = WAVE_FORMAT_PCM;
	pcmFMT.wChannels = 1;
	pcmFMT.dwSamplesPerSec = 16000;
	pcmFMT.uiBitsPerSample = 8;
	pcmFMT.dwAvgBytesPerSec = pcmFMT.dwSamplesPerSec * pcmFMT.wChannels * pcmFMT.uiBitsPerSample / 8;
	pcmFMT.wBlockAlign = pcmFMT.wChannels * pcmFMT.uiBitsPerSample / 8;

	wavFile.write(&pcmFMT, sizeof(WAVE_FMT), FileSeekMode_cur);

	/* WAVE_DATA */
	memcpy(pcmDATA.fccID, "data", strlen("data"));
	pcmDATA.dwSize = 0;
	wavFile.seek(sizeof(WAVE_DATA), FileSeekMode_cur);

	/* write pcm data */
	unsigned short pcmBuffer;
	pcmFile.read(&pcmBuffer, sizeof(unsigned short), 1);
	while(!pcmFile.isEndOfFile())
	{
		pcmDATA.dwSize += 2;
		wavFile.write(&pcmBuffer, sizeof(unsigned short), 1);
		pcmFile.read(&pcmBuffer, sizeof(unsigned short), 1);
	}

	pcmHEADER.dwSize = 44 + pcmDATA.dwSize;

	wavFile.rewind();

	wavFile.write(&pcmHEADER, sizeof(WAVE_HEADER), 1);
	wavFile.seek(sizeof(WAVE_FMT), FileSeekMode_cur);
	wavFile.write(&pcmDATA, sizeof(WAVE_DATA), 1);

	pcmFile.close();
	wavFile.close();

	return TRUE;
}

}