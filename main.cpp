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

#define RTSPURL "rtsp://admin:admin@192.168.1.106/"

Easy_RTSP_Handle fRTSPHandle = 0;

/* RTSPSource从RTSPClient获取数据后回调给上层 */
int Easy_APICALL __RTSPClientCallBack( int _chid, int *_chPtr, int _mediatype, char *pbuf, RTSP_FRAME_INFO *frameinfo)
{
	if (NULL != frameinfo)
	{
		if (frameinfo->height==1088)		frameinfo->height=1080;
		else if (frameinfo->height==544)	frameinfo->height=540;
	}

	if (_mediatype == MEDIA_TYPE_VIDEO)
	{
		printf("Get %s Video Len:%d tm:%d rtp:%d\n",frameinfo->type==FRAMETYPE_I?"I":"P", frameinfo->length, frameinfo->timestamp_sec, frameinfo->rtptimestamp);
	}
	else if (_mediatype == MEDIA_TYPE_AUDIO)
	{
		printf("Get Audio Len:%d tm:%d rtp:%d\n", frameinfo->length, frameinfo->timestamp_sec, frameinfo->rtptimestamp);
	}
	else if (_mediatype == MEDIA_TYPE_EVENT)
	{
		if (NULL == pbuf && NULL == frameinfo)
		{
			printf("Connecting:%s ...\n", RTSPURL);
		}
		else if (NULL!=frameinfo && frameinfo->type==0xF1)
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

	unsigned int mediaType = MEDIA_TYPE_VIDEO;
	mediaType |= MEDIA_TYPE_AUDIO;	//换为RTSPSource, 屏蔽声音
	
	//设置数据回调处理
	EasyRTSP_SetCallback(fRTSPHandle, __RTSPClientCallBack);
	//打开RTSP流
	EasyRTSP_OpenStream(fRTSPHandle, 0, RTSPURL, RTP_OVER_TCP, mediaType, 0, 0, NULL, 1000, 0);

	while(1)
	{
		Sleep(10);	
	};
   
	EasyRTSP_CloseStream(fRTSPHandle);
	EasyRTSP_Deinit(&fRTSPHandle);
	fRTSPHandle = NULL;

    return 0;
}