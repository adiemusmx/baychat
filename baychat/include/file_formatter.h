#pragma once

#include "basic_types.h"

namespace BayChat
{

class AudioFormatter
{
public:
	static BOOL pcm2wav(const WCHAR * pcmFileName, const WCHAR * wavFileName);

private:
	AudioFormatter();
	~AudioFormatter();

	AudioFormatter(const AudioFormatter&);
	AudioFormatter& operator=(const AudioFormatter&);
};

}