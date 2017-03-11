/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include <stdio.h>
#include <string.h>
#include "getopt.h"
#include "EasyRTSPClientAPI.h"

#ifdef _WIN32
#define KEY "79393674363469576B5A7541374D4659707A34544A65314659584E35556C525455454E73615756756443356C65475570567778576F502B6C34456468646D6C754A6B4A68596D397A595541794D4445325257467A65555268636E6470626C526C5957316C59584E35"
#else //x86 linux
#define KEY "7939367436354F576B597141374D4659707A34544A66466C59584E35636E527A63474E7361575675644C3558444661672F365867523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"
#endif

FILE* fVideo = NULL;
FILE* fAudio = NULL;

char* fRTSPURL = NULL;		//rtsp source addrs
int fTransType = 0;			//0 : TCP    1 : UDP
bool fSaveFile = true;		//true : save file     false ： don't save


Easy_RTSP_Handle fRTSPHandle = 0;


/* RTSPClient数据回调 */
int Easy_APICALL __RTSPClientCallBack( int _chid, void *_chPtr, int _frameType, char *_pBuf, RTSP_FRAME_INFO* _frameInfo)
{
	if (_frameType == EASY_SDK_VIDEO_FRAME_FLAG)//回调视频数据，包含00 00 00 01头
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
				每一个I关键帧都是SPS+PPS+IDR的组合
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

				spsLen = _frameInfo->reserved1;							// SPS数据长度
				ppsLen = _frameInfo->reserved2 - _frameInfo->reserved1;	// PPS数据长度
				iFrameLen = _frameInfo->length - spsLen - ppsLen;		// IDR数据长度

				memcpy(sps, _pBuf, spsLen);			//SPS数据，包含00 00 00 01
				memcpy(pps, _pBuf+spsLen, ppsLen);	//PPS数据，包含00 00 00 01
				IFrame = _pBuf + spsLen + ppsLen;	//IDR数据，包含00 00 00 01

				printf("Get I H264(%d * %d) SPS/PPS/IDR Len:%d/%d/%d \ttimestamp:%u.%u\n",_frameInfo->width, _frameInfo->height, spsLen, ppsLen, iFrameLen, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
			}
			else if (_frameInfo->type == EASY_SDK_VIDEO_FRAME_P)
			{
				printf("Get P H264(%d * %d) Len:%d \ttimestamp:%u.%u\n",_frameInfo->width, _frameInfo->height, _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
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
	else if (_frameType == EASY_SDK_AUDIO_FRAME_FLAG)//回调音频数据
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
	else if (_frameType == EASY_SDK_EVENT_FRAME_FLAG)//回调连接状态事件
	{
		// EasyRTSPClient开始进行连接，建立EasyRTSPClient连接线程
		if (NULL == _pBuf && NULL == _frameInfo)
		{
			printf("Connecting:%s ...\n", fRTSPURL);
		}

		// EasyRTSPClient RTSPClient连接错误，错误码通过EasyRTSP_GetErrCode()接口获取，比如404
		else if (NULL != _frameInfo && _frameInfo->codec == EASY_SDK_EVENT_CODEC_ERROR)
		{
			printf("Error:%s：%d :%s ...\n", fRTSPURL, EasyRTSP_GetErrCode(fRTSPHandle), _pBuf?_pBuf:"null" );
		}

		// EasyRTSPClient连接线程退出，此时上层应该停止相关调用，复位连接按钮等状态
		else if (NULL != _frameInfo && _frameInfo->codec == EASY_SDK_EVENT_CODEC_EXIT)
		{
			printf("Exit:%s,Error:%d ...\n", fRTSPURL, EasyRTSP_GetErrCode(fRTSPHandle));
		}
	}
	else if (_frameType == EASY_SDK_MEDIA_INFO_FLAG)//回调出媒体信息
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
				fTransType = 0;//默认tcp
			}

			break;
		case 's':
			if((strlen(optarg) == 2) && ((0 == strcmp(optarg, "NO"))|| (0 == strcmp(optarg, "no"))))
			{
				fSaveFile = false;
			}
			else
			{
				fSaveFile = true;//默认存文件
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

	//创建RTSPSource
	EasyRTSP_Init(&fRTSPHandle);

	// 可以根据fRTSPHanlde判断，也可以根据EasyRTSP_Init是否返回0判断
	if (NULL == fRTSPHandle) return 0;
	
	// 设置数据回调处理
	EasyRTSP_SetCallback(fRTSPHandle, __RTSPClientCallBack);

	// 设置接收音频、视频数据
	unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG;

	// 打开RTSP流
	if(fTransType == 0)
		EasyRTSP_OpenStream(fRTSPHandle, 0, fRTSPURL, EASY_RTP_OVER_TCP, mediaType, "admin", "admin", NULL, 1000, 0, 0x01, 3);
	else
		EasyRTSP_OpenStream(fRTSPHandle, 0, fRTSPURL, EASY_RTP_OVER_UDP, mediaType, "admin", "admin", NULL, 1000, 0, 0x01, 3);

	printf("Press Enter exit...\n");
	getchar();
   
	// 关闭RTSPClient
	EasyRTSP_CloseStream(fRTSPHandle);

	// 释放RTSPHandle
	EasyRTSP_Deinit(&fRTSPHandle);
	fRTSPHandle = NULL;

    return 0;
}