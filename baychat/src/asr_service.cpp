#include "stdafx.h"
#include "asr_service.h"

#include "ifly/qisr.h"
#include "ifly/msp_cmn.h"
#include "ifly/msp_errors.h"

#define	BUFFER_SIZE 2048
#define HINTS_SIZE  100
#define GRAMID_LEN	128
#define FRAME_LEN	640

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
	/*
	* sub:             请求业务类型
	* result_type:     识别结果格式
	* result_encoding: 结果编码格式
	*
	* 详细参数说明请参阅《讯飞语音云MSC--API文档》
	*/
	const CHAR*	session_begin_params	=	"sub = asr, result_type = plain, result_encoding = gb2312,sample_rate = 16000,aue = speex-wb";

	/* 用户登录 */
	ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数
	if (MSP_SUCCESS != ret)
	{
		//printf("MSPLogin failed, error code: %d.\n", ret);
		LOGGER_ERROR_LOG("MSPLogin Failed, Error Code: %d", ret);
		return;
	}

	if (NULL == m_grammarId)
	{
		m_grammarId = (char*)malloc(GRAMID_LEN);
	}
	if (NULL == m_grammarId)
	{
		//printf("out of memory !\n");
		LOGGER_ERROR_LOG("Out Of Memory!");
		return;
	}
	memset(m_grammarId, 0, GRAMID_LEN);

	ret = getGrammarId(m_grammarId, GRAMID_LEN);
	if (MSP_SUCCESS != ret)
	{
		return;
	}

	LOGGER_TRACE_LOG_END();
}

void AsrService_IFly::update(const CHAR* audio_file)
{
	const CHAR*		session_id						= NULL;
	CHAR			rec_result[BUFFER_SIZE]		 	= {'\0'};
	CHAR			hints[HINTS_SIZE]				= {'\0'}; //hints为结束本次会话的原因描述，由用户自定义
	uint32			total_len						= 0;
	int32 			aud_stat 						= MSP_AUDIO_SAMPLE_CONTINUE;		//音频状态
	int32 			ep_stat 						= MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	int32 			rec_stat 						= MSP_REC_STATUS_SUCCESS;			//识别状态
	int32 			errcode 						= MSP_SUCCESS;

	FILE*			f_pcm 							= NULL;
	CHAR*			p_pcm 							= NULL;
	long 			pcm_count 						= 0;
	long 			pcm_size 						= 0;
	long			read_size						= 0;

	if (NULL == audio_file)
		goto asr_exit;

	f_pcm = fopen(audio_file, "rb");
	if (NULL == f_pcm)
	{
		printf("\nopen [%s] failed!\n", audio_file);
		goto asr_exit;
	}

	fseek(f_pcm, 0, SEEK_END);
	pcm_size = ftell(f_pcm); //获取音频文件大小
	fseek(f_pcm, 0, SEEK_SET);

	p_pcm = (char*)malloc(pcm_size);
	if (NULL == p_pcm)
	{
		printf("\nout of memory!\n");
		goto asr_exit;
	}

	read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm); //读取音频文件内容
	if (read_size != pcm_size)
	{
		printf("\nread [%s] failed!\n", audio_file);
		goto asr_exit;
	}

	printf("\n开始语音识别 ...\n");
	session_id = QISRSessionBegin(grammar_id, params, &errcode);
	if (MSP_SUCCESS != errcode)
	{
		printf("\nQISRSessionBegin failed, error code:%d\n", errcode);
		goto asr_exit;
	}

	while (1)
	{
		unsigned int len = 10 * FRAME_LEN; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
		int ret = 0;

		if (pcm_size < 2 * len)
			len = pcm_size;
		if (len <= 0)
			break;

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		printf(">");
		ret = QISRAudioWrite(session_id, (const void *)&p_pcm[pcm_count], len, aud_stat, &ep_stat, &rec_stat);
		if (MSP_SUCCESS != ret)
		{
			printf("\nQISRAudioWrite failed, error code:%d\n", ret);
			goto asr_exit;
		}

		pcm_count += (long)len;
		pcm_size  -= (long)len;

		if (MSP_EP_AFTER_SPEECH == ep_stat)
			break;
		Sleep(200); //模拟人说话时间间隙，10帧的音频长度为200ms
	}
	errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
	if (MSP_SUCCESS != errcode)
	{
		printf("\nQISRAudioWrite failed, error code:%d\n", errcode);
		goto asr_exit;
	}

	while (MSP_REC_STATUS_COMPLETE != rec_stat)
	{
		const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
		if (MSP_SUCCESS != errcode)
		{
			printf("\nQISRGetResult failed, error code: %d\n", errcode);
			goto asr_exit;
		}
		if (NULL != rslt)
		{
			unsigned int rslt_len = strlen(rslt);
			total_len += rslt_len;
			if (total_len >= BUFFER_SIZE)
			{
				printf("\nno enough buffer for rec_result !\n");
				goto asr_exit;
			}
			strncat(rec_result, rslt, rslt_len);
		}
		Sleep(150); //防止频繁占用CPU
	}
	printf("\n语音识别结束\n");
	printf("=============================================================\n");
	printf("%s", rec_result);
	printf("=============================================================\n");

asr_exit:
	if (NULL != f_pcm)
	{
		fclose(f_pcm);
		f_pcm = NULL;
	}
	if (NULL != p_pcm)
	{
		free(p_pcm);
		p_pcm = NULL;
	}

	QISRSessionEnd(session_id, hints);
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

int32 AsrService_IFly::getGrammarId(CHAR* grammarId, uint32 idLen)
{
	//FILE*			fp				=	NULL;
	File			fp;
	CHAR*			grammar			=	NULL;
	uint32			grammar_len		=	0;
	uint32			read_len		=	0;
	const CHAR*		ret_id			=	NULL;
	uint32			ret_id_len		=	0;
	int32			ret				=	-1;

	if (NULL == grammarId)
		goto grammar_exit;

	//fp = fopen("gm_continuous_digit.abnf", "rb");
	fp.open(L"gm_continuous_digit.abnf", FileMode_readBinary);
	if (fp.isValid())
	{
		//printf("\nopen grammar file failed!\n");
		LOGGER_ERROR_LOG("Open Grammar File Failed!");
		goto grammar_exit;
	}

	//fseek(fp, 0, SEEK_END);
	//fp.seek(0, FileSeekMode_end);
	grammar_len = fp.length(); //获取语法文件大小

	grammar = (CHAR*)malloc(grammar_len + 1);
	if (NULL == grammar)
	{
		//printf("\nout of memory!\n");
		LOGGER_ERROR_LOG("Out Of Memory!");
		goto grammar_exit;
	}

	//read_len = fread((void *)grammar, 1, grammar_len, fp); //读取语法内容
	read_len = fp.read((void *)grammar, 1, grammar_len);
	if (read_len != grammar_len)
	{
		//printf("\nread grammar error!\n");
		LOGGER_ERROR_LOG("Read Greammar Error!");
		goto grammar_exit;
	}
	grammar[grammar_len] = '\0';

	ret_id = MSPUploadData("usergram", grammar, grammar_len, "dtt = abnf, sub = asr", &ret); //上传语法
	if (MSP_SUCCESS != ret)
	{
		//printf("\nMSPUploadData failed, error code: %d.\n", ret);
		LOGGER_ERROR_LOG("MSPUploadData Failed, Error Code: %d.", ret);
		goto grammar_exit;
	}

	ret_id_len = strlen(ret_id);
	if (ret_id_len >= idLen)
	{
		//printf("\nno enough buffer for grammarId!\n");
		LOGGER_ERROR_LOG("No Enough Buffer For Grammar Id!");
		goto grammar_exit;
	}
	strncpy(grammarId, ret_id, ret_id_len);
	//printf("grammarId: \"%s\" \n", grammarId); //下次可以直接使用该ID，不必重复上传语法。
	LOGGER_ERROR_LOG("Grammar Id: %s", grammarId);

grammar_exit:
	if (fp.isValid())
	{
		fp.close();
	}
	if (NULL != grammar)
	{
		free(grammar);
		grammar = NULL;
	}
	return ret;
}

}

void run_asr(const char* audio_file, const char* params, char* grammar_id)
{

}

int main(int argc, char* argv[])
{
	run_asr("wav/weather.pcm", session_begin_params, grammar_id); //iflytek01对应的音频内容：“18012345678”

exit:


	return 0;
}
