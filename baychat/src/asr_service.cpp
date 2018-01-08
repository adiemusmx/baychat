#include "stdafx.h"
#include "asr_service.h"

#include "ifly/qisr.h"
#include "ifly/msp_cmn.h"
#include "ifly/msp_errors.h"

#define	BUFFER_SIZE 2048
#define HINTS_SIZE  100
#define GRAMID_LEN	128
#define FRAME_LEN	640

#define GRAMMAR_ID_DIGIT_ABNF L"gm_continuous_digit.abnf"

namespace BayChat
{
AsrService::AsrService()
{
}

AsrService::~AsrService()
{
}

AsrService_IFly::AsrService_IFly()
{
	m_grammarId = NULL;
}

AsrService_IFly::~AsrService_IFly()
{
}

void AsrService_IFly::openSession()
{
	LOGGER_TRACE_LOG_START();

	int32		ret						=	MSP_SUCCESS;
	const CHAR* login_params			=	"appid = 5a07c82a, work_dir = ."; //登录参数,appid与msc库绑定,请勿随意改动

	/* 用户登录 */
	ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数
	if (MSP_SUCCESS != ret)
	{
		LOGGER_ERROR_LOG("MSPLogin Failed, Error Code: %d", ret);
		return;
	}

	if (NULL == m_grammarId)
	{
		m_grammarId = (char*)malloc(GRAMID_LEN);
	}
	if (NULL == m_grammarId)
	{
		LOGGER_ERROR_LOG("Out Of Memory!");
		return;
	}
	memset(m_grammarId, 0, GRAMID_LEN);

	ret = getNumberGrammarId(m_grammarId, GRAMID_LEN);
	if (MSP_SUCCESS != ret)
	{
		LOGGER_ERROR_LOG("Grammar Id Failed. [%d]", ret);
		return;
	}

	LOGGER_TRACE_LOG_END();
}

void AsrService_IFly::update(const WCHAR* audio_file)
{
	LOGGER_TRACE_LOG_START();

	const CHAR*		session_id						= NULL;
	CHAR			rec_result[BUFFER_SIZE]		 	= {'\0'};
	CHAR			hints[HINTS_SIZE]				= {'\0'}; //hints为结束本次会话的原因描述，由用户自定义
	size_t			total_len						= 0;
	int32 			aud_stat 						= MSP_AUDIO_SAMPLE_CONTINUE;		//音频状态
	int32 			ep_stat 						= MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	int32 			rec_stat 						= MSP_REC_STATUS_SUCCESS;			//识别状态
	int32 			errcode 						= MSP_SUCCESS;

	//FILE*			f_pcm 							= NULL;
	File			f_pcm;
	CHAR*			p_pcm 							= NULL;
	long 			pcm_count 						= 0;
	size_t 			pcm_size 						= 0;
	size_t			read_size						= 0;

	/*
	* sub:             请求业务类型
	* result_type:     识别结果格式
	* result_encoding: 结果编码格式
	*
	* 详细参数说明请参阅《讯飞语音云MSC--API文档》
	*/
	//const CHAR*	session_begin_params	=	"sub = asr, result_type = plain, result_encoding = gb2312,sample_rate = 16000,aue = speex-wb";
	const CHAR*	session_begin_params =
	    "sub = iat, \
		result_type = json, \
		result_encoding = gb2312, \
		sample_rate = 16000, \
		aue = speex-wb";

	if (NULL == audio_file)
		goto asr_exit;

	//f_pcm = fopen(audio_file, "rb");
	f_pcm.open(audio_file, FileMode_readBinary);

	if (!f_pcm.isOpen())
	{
		LOGGER_ERROR_LOG("Open [%S] failed!", audio_file);
		goto asr_exit;
	}

	f_pcm.seek(0, FileSeekMode_end);
	pcm_size = f_pcm.length();
	f_pcm.seek(0, FileSeekMode_set);

	LOGGER_TRACE_LOG("file size[%zu]", pcm_size);

	p_pcm = (char*)malloc(pcm_size);
	if (NULL == p_pcm)
	{
		LOGGER_ERROR_LOG("Out Of Memory!");
		goto asr_exit;
	}

	read_size = f_pcm.read(p_pcm, 1, pcm_size);
	if (read_size != pcm_size)
	{
		LOGGER_ERROR_LOG("Read Failed!");
		goto asr_exit;
	}

	LOGGER_INFO_LOG("Start Voice Recognation...");
	session_id = QISRSessionBegin(m_grammarId, session_begin_params, &errcode);
	if (MSP_SUCCESS != errcode)
	{
		LOGGER_ERROR_LOG("QISRSessionBegin Failed, Error Code:%d", errcode);
		goto asr_exit;
	}

	while (TRUE)
	{
		size_t len = 10 * FRAME_LEN; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
		int ret = 0;

		if (pcm_size < 2 * len)
			len = pcm_size;
		if (len <= 0)
			break;

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		ret = QISRAudioWrite(session_id, (const void *)&p_pcm[pcm_count], (unsigned int)len, aud_stat, &ep_stat, &rec_stat);
		if (MSP_SUCCESS != ret)
		{
			LOGGER_ERROR_LOG("QISRAudioWrite Failed, Error Code:%d", ret);
			goto asr_exit;
		}

		pcm_count += (long)len;
		pcm_size  -= (long)len;

		if (MSP_EP_AFTER_SPEECH == ep_stat)
			break;

		//Sleep(200); //模拟人说话时间间隙，10帧的音频长度为200ms
	}
	LOGGER_TRACE_LOG("upload size[%d]", pcm_count);
	errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
	if (MSP_SUCCESS != errcode)
	{
		LOGGER_ERROR_LOG("QISRAudioWrite Failed, Error Code:%d", errcode);
		goto asr_exit;
	}

	while (MSP_REC_STATUS_COMPLETE != rec_stat)
	{
		const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
		if (MSP_SUCCESS != errcode)
		{
			LOGGER_ERROR_LOG("QISRGetResult Failed, Error Code: %d", errcode);
			goto asr_exit;
		}
		if (NULL != rslt)
		{
			size_t rslt_len = strlen(rslt);
			total_len += rslt_len;
			if (total_len >= BUFFER_SIZE)
			{
				LOGGER_ERROR_LOG("No Enough Buffer For rec_result!");
				goto asr_exit;
			}
			strncat_s(rec_result, rslt, rslt_len);
		}
		Sleep(150); //防止频繁占用CPU
	}

	LOGGER_INFO_LOG("Recognation Completed. --> [%s]", rec_result);

asr_exit:
	if (f_pcm.isOpen())
	{
		f_pcm.close();
	}
	if (p_pcm != NULL)
	{
		free(p_pcm);
		p_pcm = NULL;
	}

	QISRSessionEnd(session_id, hints);

	LOGGER_TRACE_LOG_END();
}

void AsrService_IFly::closeSession()
{
	LOGGER_TRACE_LOG_START();

	if (NULL != m_grammarId)
	{
		free(m_grammarId);
		m_grammarId = NULL;
	}
	MSPLogout(); //退出登录

	LOGGER_TRACE_LOG_END();
}

int32 AsrService_IFly::getNumberGrammarId(CHAR* grammarId, uint32 idLen)
{
	LOGGER_TRACE_LOG_START();

	//FILE*			fp				=	NULL;
	File			fp;
	CHAR*			grammar			=	NULL;
	size_t			grammar_len		=	0;
	size_t			read_len		=	0;
	const CHAR*		ret_id			=	NULL;
	size_t			ret_id_len		=	0;
	int32			ret				=	-1;

	if (NULL == grammarId)
		goto grammar_exit;

	fp.open(GRAMMAR_ID_DIGIT_ABNF, FileMode_readBinary);
	if (!fp.isOpen())
	{
		LOGGER_ERROR_LOG("Open Grammar File Failed!");
		goto grammar_exit;
	}

	grammar_len = fp.length(); //获取语法文件大小

	grammar = (CHAR*)malloc(grammar_len + 1);
	if (NULL == grammar)
	{
		LOGGER_ERROR_LOG("Out Of Memory!");
		goto grammar_exit;
	}

	read_len = fp.read((void *)grammar, 1, grammar_len);
	if (read_len != grammar_len)
	{
		LOGGER_ERROR_LOG("Read Greammar Error!");
		goto grammar_exit;
	}
	grammar[grammar_len] = '\0';

	ret_id = MSPUploadData("usergram", grammar, (unsigned int)grammar_len, "dtt = abnf, sub = asr", &ret); //上传语法
	if (MSP_SUCCESS != ret)
	{
		LOGGER_ERROR_LOG("MSPUploadData Failed, Error Code: %d.", ret);
		goto grammar_exit;
	}

	ret_id_len = strlen(ret_id);
	if (ret_id_len >= idLen)
	{
		LOGGER_ERROR_LOG("No Enough Buffer For Grammar Id!");
		goto grammar_exit;
	}
	strncpy_s(grammarId, idLen, ret_id, ret_id_len);
	LOGGER_INFO_LOG("Grammar Id: %s", grammarId);

grammar_exit:
	if (fp.isOpen())
	{
		fp.close();
	}
	if (NULL != grammar)
	{
		free(grammar);
		grammar = NULL;
	}

	LOGGER_TRACE_LOG_END();
	return ret;
}

}
