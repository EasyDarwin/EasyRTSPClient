/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#include "EasyRTSPClientAPI.h"
#include <windows.h>

#define RTSPURL "rtsp://admin:admin@wellsen.f3322.org/"

Easy_RTSP_Handle fRTSPHandle = 0;

/* RTSPClient数据回调 */
int Easy_APICALL __RTSPClientCallBack( int _chid, int *_chPtr, int _frameType, char *_pBuf, RTSP_FRAME_INFO* _frameInfo)
{
	if (_frameType == EASY_SDK_VIDEO_FRAME_FLAG)
	{
		printf("Get %s Video Len:%d \ttimestamp:%u.%u\n",_frameInfo->type==EASY_SDK_VIDEO_FRAME_I?"I":"P", _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
	}
	else if (_frameType == EASY_SDK_AUDIO_FRAME_FLAG)
	{
		printf("Get %u Audio Len:%d \ttimestamp:%u.%u\n", _frameInfo->codec, _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
	}
	else if (_frameType == EASY_SDK_EVENT_FRAME_FLAG)
	{
		if (NULL == _pBuf && NULL == _frameInfo)
		{
			printf("Connecting:%s ...\n", RTSPURL);
		}
		else if (NULL!=_frameInfo && _frameInfo->type==0xF1)
		{
			printf("Lose Packet:%s ...\n", RTSPURL);
		}
	}
	return 0;
}

int main()
{
	//创建RTSPSource
	EasyRTSP_Init(&fRTSPHandle);

	if (NULL == fRTSPHandle) return 0;
	
	// 设置数据回调处理
	EasyRTSP_SetCallback(fRTSPHandle, __RTSPClientCallBack);

	unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG;

	// 打开RTSP流
	EasyRTSP_OpenStream(fRTSPHandle, 0, RTSPURL, RTP_OVER_TCP, mediaType, 0, 0, NULL, 1000, 0);

	getchar();
   
	EasyRTSP_CloseStream(fRTSPHandle);
	EasyRTSP_Deinit(&fRTSPHandle);
	fRTSPHandle = NULL;

    return 0;
}