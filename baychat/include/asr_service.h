#pragma once

#include "basic_types.h"

namespace BayChat
{

/*
 * 语音识别模块基类
 */
class AsrService
{
public:
	AsrService();
	virtual ~AsrService();

	virtual void openSession() = 0;
	virtual void update(const WCHAR* audio_file) = 0;
	virtual void closeSession() = 0;

};

class AsrService_IFly : public AsrService
{
public:
	AsrService_IFly();
	virtual ~AsrService_IFly();

	virtual void openSession();
	virtual void update(const WCHAR* audio_file);
	virtual void closeSession();

private:
	/* 获得数字的Grammar ID */
	int32 getNumberGrammarId(CHAR* grammarId, uint32 idLen);

	CHAR* m_grammarId;

};

}