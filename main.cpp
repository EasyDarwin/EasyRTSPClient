/*
	Copyright (c) 2013-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include <stdio.h>
#include <string.h>
#include "getopt.h"
#include "EasyRTSPClientAPI.h"

#ifdef _WIN32
#define KEY "79393674363469576B5A754144474A636F35337A4A65314659584E35556C525455454E73615756756443356C65475745567778576F502B6C3430566863336C4559584A33615735555A57467453584E55614756435A584E30514449774D54686C59584E35"
#else //linux
#define KEY "7939367436354F576B596F41753242636F3539457066466C59584E35636E527A63474E736157567564477858444661672F36586A5257467A65555268636E6470626C526C5957314A6331526F5A554A6C633352414D6A41784F47566863336B3D"
#endif

FILE* fVideo = NULL;
FILE* fAudio = NULL;

char* fRTSPURL = NULL;		//rtsp source addrs
int fTransType = 0;			//0 : TCP    1 : UDP
bool fSaveFile = true;		//true : save file     false �� don't save


Easy_RTSP_Handle fRTSPHandle = 0;


/* RTSPClient���ݻص� */
int Easy_APICALL __RTSPClientCallBack( int _chid, void *_chPtr, int _frameType, char *_pBuf, RTSP_FRAME_INFO* _frameInfo)
{
	if (_frameType == EASY_SDK_VIDEO_FRAME_FLAG)//�ص���Ƶ���ݣ�����00 00 00 01ͷ
	{
		if (_frameInfo->codec == EASY_SDK_VIDEO_CODEC_H264)
		{
			if(fSaveFile)
			{
				if(fVideo == NULL)
				{
					char filename[100] = {0};
					sprintf(filename, "./video_%s.H264", fTransType?"udp":"tcp");
					fVideo = ::fopen(filename,"wb");
				}

				::fwrite(_pBuf, 1, _frameInfo->length, fVideo);
			}
			/* 
				ÿһ��I�ؼ�֡����SPS+PPS+IDR�����
				|---------------------|----------------|-------------------------------------|
				|                     |                |                                     |
				0-----------------reserved1--------reserved2-------------------------------length
			*/
			if (_frameInfo->type == EASY_SDK_VIDEO_FRAME_I)
			{
				char sps[2048] = { 0 };
				char pps[2048] = { 0 };
				char* IFrame = NULL;
				unsigned int spsLen,ppsLen,iFrameLen = 0;

				spsLen = _frameInfo->reserved1;							// SPS���ݳ���
				ppsLen = _frameInfo->reserved2 - _frameInfo->reserved1;	// PPS���ݳ���
				iFrameLen = _frameInfo->length - spsLen - ppsLen;		// IDR���ݳ���

				memcpy(sps, _pBuf, spsLen);			//SPS���ݣ�����00 00 00 01
				memcpy(pps, _pBuf+spsLen, ppsLen);	//PPS���ݣ�����00 00 00 01
				IFrame = _pBuf + spsLen + ppsLen;	//IDR���ݣ�����00 00 00 01

				printf("Get I H264(%d * %d) SPS/PPS/IDR Len:%d/%d/%d \ttimestamp:%u.%u\n",_frameInfo->width, _frameInfo->height, spsLen, ppsLen, iFrameLen, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
			}
			else if (_frameInfo->type == EASY_SDK_VIDEO_FRAME_P)
			{
				printf("Get P H264(%d * %d) Len:%d \ttimestamp:%u.%u\n",_frameInfo->width, _frameInfo->height, _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
			}
		}
		else if(_frameInfo->codec == EASY_SDK_VIDEO_CODEC_H265)
		{
			if (_frameInfo->type == EASY_SDK_VIDEO_FRAME_I)
			{
				printf("Get I H265(%d * %d) Len:%d \ttimestamp:%u.%u\n",_frameInfo->width, _frameInfo->height, _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
			}
			else if (_frameInfo->type == EASY_SDK_VIDEO_FRAME_P)
			{
				printf("Get P H265(%d * %d) Len:%d \ttimestamp:%u.%u\n",_frameInfo->width, _frameInfo->height, _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
			}
		}
		else if (_frameInfo->codec == EASY_SDK_VIDEO_CODEC_MJPEG)
		{
			if(fSaveFile)
			{
				if(fVideo == NULL)
				{
					char filename[100] = {0};
					sprintf(filename, "./video_%s.mjpeg", fTransType?"udp":"tcp");
					fVideo = ::fopen(filename,"wb");
				}

				::fwrite(_pBuf, 1, _frameInfo->length, fVideo);
			}

			printf("Get MJPEG Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		}
		else if (_frameInfo->codec == EASY_SDK_VIDEO_CODEC_MPEG4)
		{
			if(fSaveFile)
			{
				if(fVideo == NULL)
				{
					char filename[100] = {0};
					sprintf(filename, "./video_%s.mpeg4", fTransType?"udp":"tcp");
					fVideo = ::fopen(filename,"wb");
				}

				::fwrite(_pBuf, 1, _frameInfo->length, fVideo);
			}

			printf("Get MPEG4 Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		}
	}
	else if (_frameType == EASY_SDK_AUDIO_FRAME_FLAG)//�ص���Ƶ����
	{
		if (_frameInfo->codec == EASY_SDK_AUDIO_CODEC_AAC)
		{
			if(fSaveFile)
			{
				if(fAudio == NULL)
				{
					char filename[100] = {0};
					sprintf(filename, "./audio_%s.aac", fTransType?"udp":"tcp");
					fAudio = ::fopen(filename,"wb");
				}
			}
			printf("Get AAC Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		}
		else if (_frameInfo->codec == EASY_SDK_AUDIO_CODEC_G711A)
		{
			if(fSaveFile)
			{
				if(fAudio == NULL)
				{
					char filename[100] = {0};
					sprintf(filename, "./audio_%s.pcma", fTransType?"udp":"tcp");
					fAudio = ::fopen(filename,"wb");
				}
			}
			printf("Get PCMA Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		}
		else if (_frameInfo->codec == EASY_SDK_AUDIO_CODEC_G711U)
		{
			if(fSaveFile)
			{
				if(fAudio == NULL)
				{
					char filename[100] = {0};
					sprintf(filename, "./audio_%s.pcmu", fTransType?"udp":"tcp");
					fAudio = ::fopen(filename,"wb");
				}
			}
			printf("Get PCMU Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		}
		else if (_frameInfo->codec == EASY_SDK_AUDIO_CODEC_G726)
		{
			if(fSaveFile)
			{
				if(fAudio == NULL)
				{
					char filename[100] = {0};
					sprintf(filename, "./audio_%s.g726", fTransType?"udp":"tcp");
					fAudio = ::fopen(filename,"wb");
				}
			}

			printf("Get G.726 Len:%d \ttimestamp:%u.%u\n", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
		}

		if(fSaveFile)
			::fwrite(_pBuf, 1, _frameInfo->length, fAudio);
	}
	else if (_frameType == EASY_SDK_EVENT_FRAME_FLAG)//�ص�����״̬�¼�
	{
		// EasyRTSPClient��ʼ�������ӣ�����EasyRTSPClient�����߳�
		if (NULL == _pBuf && NULL == _frameInfo)
		{
			printf("Connecting:%s ...\n", fRTSPURL);
		}

		// EasyRTSPClient RTSPClient���Ӵ��󣬴�����ͨ��EasyRTSP_GetErrCode()�ӿڻ�ȡ������404
		else if (NULL != _frameInfo && _frameInfo->codec == EASY_SDK_EVENT_CODEC_ERROR)
		{
			printf("Error:%s��%d :%s ...\n", fRTSPURL, EasyRTSP_GetErrCode(fRTSPHandle), _pBuf?_pBuf:"null" );
		}

		// EasyRTSPClient�����߳��˳�����ʱ�ϲ�Ӧ��ֹͣ��ص��ã���λ���Ӱ�ť��״̬
		else if (NULL != _frameInfo && _frameInfo->codec == EASY_SDK_EVENT_CODEC_EXIT)
		{
			printf("Exit:%s,Error:%d ...\n", fRTSPURL, EasyRTSP_GetErrCode(fRTSPHandle));
		}
	}
	else if (_frameType == EASY_SDK_MEDIA_INFO_FLAG)//�ص���ý����Ϣ
	{
		if(_pBuf != NULL)
		{
			EASY_MEDIA_INFO_T mediainfo;
			memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
			memcpy(&mediainfo, _pBuf, sizeof(EASY_MEDIA_INFO_T));
			printf("RTSP DESCRIBE Get Media Info: video:%u fps:%u audio:%u channel:%u sampleRate:%u \n", 
				mediainfo.u32VideoCodec, mediainfo.u32VideoFps, mediainfo.u32AudioCodec, mediainfo.u32AudioChannel, mediainfo.u32AudioSamplerate);
		}
	}
	return 0;
}

void usage(char const* progName) 
{
  printf("Usage: %s <rtsp-url> \n", progName);
}

void PrintUsage(char const* progName)
{
	printf("Usage:\n");
	printf("--------------------------------------------------------------\n");
	printf("%s -d <rtsp-url>[ -m <transport-mode> -s <save-file>]\n", progName);
	printf("Help Mode:   %s -h \n", progName );
	printf("rtsp-url : source rtsp address\ntransport-mode : tcp or udp, default is tcp\nsave-file : yes or no, default is yes\n");
	printf("For example: %s -d rtsp://admin:admin@192.168.2.100/11 -m tcp -s yes\n", progName); 
	printf("--------------------------------------------------------------\n");
}

int main(int argc, char** argv)
{
	int isActivated = 0;
	int ch;
	// We need at least one "rtsp://" URL argument:
	if (argc < 2) 
	{
		PrintUsage(argv[0]);
		printf("Press Enter exit...\n");
		getchar();
		return 1;
	}

	while ((ch = getopt(argc,argv, "h:d:m:s:")) != EOF) 
	{
		switch(ch)
		{
		case 'h':
			PrintUsage(argv[0]);
			return 0;
			break;
		case 'd':
			fRTSPURL = optarg;
			break;
		case 'm':
			if((strlen(optarg) == 3) && ((0 == strcmp(optarg, "UDP"))|| (0 == strcmp(optarg, "udp"))))
			{
				fTransType = 1;
			}
			else
			{
				fTransType = 0;//Ĭ��tcp
			}

			break;
		case 's':
			if((strlen(optarg) == 2) && ((0 == strcmp(optarg, "NO"))|| (0 == strcmp(optarg, "no"))))
			{
				fSaveFile = false;
			}
			else
			{
				fSaveFile = true;//Ĭ�ϴ��ļ�
			}
			break;
		default:
			break;
		}
	}

	isActivated = EasyRTSP_Activate(KEY);
	switch(isActivated)
	{
	case EASY_ACTIVATE_INVALID_KEY:
		printf("KEY is EASY_ACTIVATE_INVALID_KEY!\n");
		break;
	case EASY_ACTIVATE_TIME_ERR:
		printf("KEY is EASY_ACTIVATE_TIME_ERR!\n");
		break;
	case EASY_ACTIVATE_PROCESS_NAME_LEN_ERR:
		printf("KEY is EASY_ACTIVATE_PROCESS_NAME_LEN_ERR!\n");
		break;
	case EASY_ACTIVATE_PROCESS_NAME_ERR:
		printf("KEY is EASY_ACTIVATE_PROCESS_NAME_ERR!\n");
		break;
	case EASY_ACTIVATE_VALIDITY_PERIOD_ERR:
		printf("KEY is EASY_ACTIVATE_VALIDITY_PERIOD_ERR!\n");
		break;
	case EASY_ACTIVATE_PLATFORM_ERR:
		printf("EASY_ACTIVATE_PLATFORM_ERR!\n");
		break;
	case EASY_ACTIVATE_COMPANY_ID_LEN_ERR:
		printf("EASY_ACTIVATE_COMPANY_ID_LEN_ERR!\n");
		break;
	case EASY_ACTIVATE_SUCCESS:
		printf("KEY is EASY_ACTIVATE_SUCCESS!\n");
		break;
	}

	if(EASY_ACTIVATE_SUCCESS != isActivated)
	{
		getchar();
		return -1;
	}

	//����RTSPSource
	EasyRTSP_Init(&fRTSPHandle);

	// ���Ը���fRTSPHanlde�жϣ�Ҳ���Ը���EasyRTSP_Init�Ƿ񷵻�0�ж�
	if (NULL == fRTSPHandle) return 0;
	
	// �������ݻص�����
	EasyRTSP_SetCallback(fRTSPHandle, __RTSPClientCallBack);

	// ���ý�����Ƶ����Ƶ����
	unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG;

	// ��RTSP�������fRTSPURL���Ѿ�Я�����û������룬��ô_username��_password���������Ͳ������ˣ������ͬʱ��һ��Ҫ����һ��
	if(fTransType == 0)
		EasyRTSP_OpenStream(fRTSPHandle, 0, fRTSPURL, EASY_RTP_OVER_TCP, mediaType, NULL, NULL, NULL, 1000, 0, 0x01, 3);
	else
		EasyRTSP_OpenStream(fRTSPHandle, 0, fRTSPURL, EASY_RTP_OVER_UDP, mediaType, NULL, NULL, NULL, 1000, 0, 0x01, 3);

	printf("Press Enter exit...\n");
	getchar();
   
	// �ر�RTSPClient
	EasyRTSP_CloseStream(fRTSPHandle);

	// �ͷ�RTSPHandle
	EasyRTSP_Deinit(&fRTSPHandle);
	fRTSPHandle = NULL;

    return 0;
}