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
	const CHAR* login_params			=	"appid = 5a07c82a, work_dir = ."; //��¼����,appid��msc���,��������Ķ�
	/*
	* sub:             ����ҵ������
	* result_type:     ʶ������ʽ
	* result_encoding: ��������ʽ
	*
	* ��ϸ����˵������ġ�Ѷ��������MSC--API�ĵ���
	*/
	const CHAR*	session_begin_params	=	"sub = asr, result_type = plain, result_encoding = gb2312,sample_rate = 16000,aue = speex-wb";

	/* �û���¼ */
	ret = MSPLogin(NULL, NULL, login_params); //��һ���������û������ڶ������������룬����NULL���ɣ������������ǵ�¼����
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
	CHAR			hints[HINTS_SIZE]				= {'\0'}; //hintsΪ�������λỰ��ԭ�����������û��Զ���
	uint32			total_len						= 0;
	int32 			aud_stat 						= MSP_AUDIO_SAMPLE_CONTINUE;		//��Ƶ״̬
	int32 			ep_stat 						= MSP_EP_LOOKING_FOR_SPEECH;		//�˵���
	int32 			rec_stat 						= MSP_REC_STATUS_SUCCESS;			//ʶ��״̬
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
	pcm_size = ftell(f_pcm); //��ȡ��Ƶ�ļ���С
	fseek(f_pcm, 0, SEEK_SET);

	p_pcm = (char*)malloc(pcm_size);
	if (NULL == p_pcm)
	{
		printf("\nout of memory!\n");
		goto asr_exit;
	}

	read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm); //��ȡ��Ƶ�ļ�����
	if (read_size != pcm_size)
	{
		printf("\nread [%s] failed!\n", audio_file);
		goto asr_exit;
	}

	printf("\n��ʼ����ʶ�� ...\n");
	session_id = QISRSessionBegin(grammar_id, params, &errcode);
	if (MSP_SUCCESS != errcode)
	{
		printf("\nQISRSessionBegin failed, error code:%d\n", errcode);
		goto asr_exit;
	}

	while (1)
	{
		unsigned int len = 10 * FRAME_LEN; // ÿ��д��200ms��Ƶ(16k��16bit)��1֡��Ƶ20ms��10֡=200ms��16k�����ʵ�16λ��Ƶ��һ֡�Ĵ�СΪ640Byte
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
		Sleep(200); //ģ����˵��ʱ���϶��10֡����Ƶ����Ϊ200ms
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
		Sleep(150); //��ֹƵ��ռ��CPU
	}
	printf("\n����ʶ�����\n");
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
	MSPLogout(); //�˳���¼

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
	grammar_len = fp.length(); //��ȡ�﷨�ļ���С

	grammar = (CHAR*)malloc(grammar_len + 1);
	if (NULL == grammar)
	{
		//printf("\nout of memory!\n");
		LOGGER_ERROR_LOG("Out Of Memory!");
		goto grammar_exit;
	}

	//read_len = fread((void *)grammar, 1, grammar_len, fp); //��ȡ�﷨����
	read_len = fp.read((void *)grammar, 1, grammar_len);
	if (read_len != grammar_len)
	{
		//printf("\nread grammar error!\n");
		LOGGER_ERROR_LOG("Read Greammar Error!");
		goto grammar_exit;
	}
	grammar[grammar_len] = '\0';

	ret_id = MSPUploadData("usergram", grammar, grammar_len, "dtt = abnf, sub = asr", &ret); //�ϴ��﷨
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
	//printf("grammarId: \"%s\" \n", grammarId); //�´ο���ֱ��ʹ�ø�ID�������ظ��ϴ��﷨��
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
	run_asr("wav/weather.pcm", session_begin_params, grammar_id); //iflytek01��Ӧ����Ƶ���ݣ���18012345678��

exit:


	return 0;
}
