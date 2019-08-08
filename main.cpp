/*
	Copyright (c) 2014-2019 TSINGSEE.com.  All rights reserved.
	Github: https://github.com/tsingsee
	WEChat: tsingsee
	Website: http://open.tsingsee.com
*/
#include <stdio.h>
#include <string.h>
#include "getopt.h"
#include "EasyRTSPClientAPI.h"

#ifdef _WIN32
#define KEY "6D75724D7A4969576B5A7541725370636F3956524575314659584E35556C525455454E73615756756443356C65475570567778576F5036532F69426C59584E35"
#else //Linux
#define KEY "6D75724D7A4A4F576B597141725370636F3956524576466C59584E35636E527A63474E736157567564434E58444661672F704C2B4947566863336B3D"
#endif

FILE* fVideo = NULL;
FILE* fAudio = NULL;

char* fRTSPURL = NULL;		//rtsp source addrs
int fTransType = 0;			//0 : TCP    1 : UDP
bool fSaveFile = true;		//true : save file     false : don't save


Easy_Handle fRTSPHandle = 0;

int Easy_APICALL __RTSPClientCallBack( int _chid, void *_chPtr, int _frameType, char *_pBuf, EASY_FRAME_INFO* _frameInfo)
{
	if (_frameType == EASY_SDK_VIDEO_FRAME_FLAG)
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
				|SPS+PPS+IDR|
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

				spsLen = _frameInfo->reserved1;							// SPS
				ppsLen = _frameInfo->reserved2 - _frameInfo->reserved1;	// PPS
				iFrameLen = _frameInfo->length - spsLen - ppsLen;		// IDR

				memcpy(sps, _pBuf, spsLen);			//SPS
				memcpy(pps, _pBuf+spsLen, ppsLen);	//PPS
				IFrame = _pBuf + spsLen + ppsLen;	//IDR

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
	else if (_frameType == EASY_SDK_AUDIO_FRAME_FLAG)
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
	else if (_frameType == EASY_SDK_EVENT_FRAME_FLAG)
	{
		if (NULL == _pBuf && NULL == _frameInfo)
		{
			printf("Connecting:%s ...\n", fRTSPURL);
		}

		else if (NULL != _frameInfo && _frameInfo->codec == EASY_SDK_EVENT_CODEC_ERROR)
		{
			printf("Error:%s, %d :%s ...\n", fRTSPURL, EasyRTSP_GetErrCode(fRTSPHandle), _pBuf?_pBuf:"null" );
		}

		else if (NULL != _frameInfo && _frameInfo->codec == EASY_SDK_EVENT_CODEC_EXIT)
		{
			printf("Exit:%s,Error:%d ...\n", fRTSPURL, EasyRTSP_GetErrCode(fRTSPHandle));
		}
	}
	else if (_frameType == EASY_SDK_MEDIA_INFO_FLAG)
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
	printf("\n*****************EasyRTSPClient-v3.0.190415*******************\n");
	int activeRet = 0;
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
				fTransType = 0;
			}

			break;
		case 's':
			if((strlen(optarg) == 2) && ((0 == strcmp(optarg, "NO"))|| (0 == strcmp(optarg, "no"))))
			{
				fSaveFile = false;
			}
			else
			{
				fSaveFile = true;
			}
			break;
		default:
			break;
		}
	}

	activeRet = EasyRTSP_Activate(KEY);
	switch(activeRet)
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
		//printf("KEY is EASY_ACTIVATE_SUCCESS!\n");
		break;
	}

	if(activeRet < 0)
	{
		printf("EasyRTSP_Activate fail:%d!\n", activeRet);
		getchar();
		return -1;
	}

	EasyRTSP_Init(&fRTSPHandle);

	if (NULL == fRTSPHandle) return 0;
	
	EasyRTSP_SetCallback(fRTSPHandle, __RTSPClientCallBack);

	unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG;

	if(fTransType == 0)
		EasyRTSP_OpenStream(fRTSPHandle, 0, fRTSPURL, EASY_RTP_OVER_TCP, mediaType, NULL, NULL, NULL, 1000, 0, 0x01, 3);
	else
		EasyRTSP_OpenStream(fRTSPHandle, 0, fRTSPURL, EASY_RTP_OVER_UDP, mediaType, NULL, NULL, NULL, 1000, 0, 0x01, 3);

	printf("Press Enter exit...\n");
	getchar();
   
	EasyRTSP_CloseStream(fRTSPHandle);

	EasyRTSP_Deinit(&fRTSPHandle);
	fRTSPHandle = NULL;

    return 0;
}